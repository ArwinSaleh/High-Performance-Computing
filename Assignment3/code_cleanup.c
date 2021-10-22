#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>

static inline double complex newton_iteration(double complex x);
static inline void initialize_roots(int d);
static inline uint8_t compare_roots(double complex x_iter);
static inline int number_of_digits(int number);
void print_attractors(uint8_t ** attractors);

#define NEWTON_ITERATIONS   100
#define MAX_DEGREE          9
#define ROOT_ERROR          0.000001

// Global parameters
int degree, lsize, n_threads;
double complex actual_roots[MAX_DEGREE];

// Structs

typedef struct
{
    int val;
    char pad[60];   // cacheline - sizeof(int)
}padded_int;

typedef struct
{
    uint8_t ** attractors;
    uint8_t ** convergences;
    int idx_begin;
    int step_size;
    int size;
    int thread_idx;
    mtx_t * mtx;
    cnd_t * cnd;
    padded_int * status;
}thread_info_t;

typedef struct
{
    uint8_t ** attractors;
    uint8_t ** convergences;
    FILE * attractor_file;
    FILE * convergence_file;
    int n_threads;
    int size;
    mtx_t * mtx;
    cnd_t * cnd;
    padded_int * status;
}write_thread_info_t;


int main_thread(void * args)
{
    const thread_info_t * thread_info = (thread_info_t*) args;
    uint8_t ** attractors = thread_info->attractors;
    uint8_t ** convergences = thread_info->convergences;
    const int idx_begin = thread_info->idx_begin;
    const int step_size = thread_info->step_size;
    const int size = thread_info->size;
    const int thread_idx = thread_info->thread_idx;
    mtx_t * mtx = thread_info->mtx;
    cnd_t * dnc = thread_info->cnd;
    padded_int * status = thread_info->status;

    double complex x_iter;
    uint8_t * attractors_row;
    uint8_t * convergence_row;
    uint8_t root_index;
    uint8_t runs;

    for (int ix = idx_begin; ix < size; ix+=step_size)
    {
        attractors_row = (uint8_t*) malloc(sz*sizeof(uint8_t));
        convergence_row = (uint8_t*) malloc(sz*sizeof(uint8_t));
        for (int jx = 0; jx < size; jx++)
        {
            root_index = 0;
            x_iter = (2. - 4.*ix * (1./lsize))*I + (-2. + 4.*jx * (1./lsize));
            runs = 0;
            while ((runs < NEWTON_ITERATIONS) && (root_index == 0))
            {
                x_iter = newton_iteration(x_iter);
                root_index = compare_roots(x_iter);
                runs++;
            }
            attractors_row[jx] = root_index;
            convergence_row[jx] = runs;
        }
        mtx_lock(mtx);
        attractors[ix] = attractors_row;
        convergences[ix] = convergence_row;
        status[tx].val = ix + step_size;
        mtx_unlock(mtx);
        cnd_signal(cnd);
    }
}

int main_write_thread(void * args)
{
    const write_thread_info_t *write_thread_info = (write_thread_info_t*) args;
    uint8_t ** attractors = write_thread_info->attractors;
    uint8_t ** convergences = write_thread_info->convergences;
    FILE * attractor_file = write_thread_info->attractor_file;
    FILE * convergence_file = write_thread_info->convergence_file;
    const int size = write_thread_info->size;
    const int n_threads = write_thread_info->n_threads;
    mtx_t *mtx = write_thread_info->mtx;
    cnd_t *cnd = write_thread_info->cnd;
    padded_int *status = write_thread_info->status;

    // Start off by writing headers to the files.
    {
        const int max_color_val = 255;
        const int attractor_header_size = 7 + 2*number_of_digits(lsize) + number_of_digits(max_color_val);
        const char * attractor_header = (char*) malloc(attractor_header_size * sizeof(char));
        const int convergence_header_size = 7 + 2*number_of_digits(lsize) + number_of_digits(NEWTON_ITERATIONS);
        const char* convergence_header = (char*) malloc(convergence_header_size * sizeof(char));

        snprintf(attractor_header, attractor_header_size, "P3\n%d %d\n%d\n", lsize, lsize, max_color_val);
        snprintf(convergence_header, convergence_header_size, "P3\n%d %d\n%d\n", lsize, lsize, NEWTON_ITERATIONS);

        fwrite(attractor_header, sizeof(char), strlen(attractor_header), attractor_file);
        fwrite(convergence_header, sizeof(char), strlen(convergence_header), convergence_file);

        free(headerAttr);
        free(headerConv);
    }
    char ** rgb_color_str = (char **) malloc((MAX_DEGREE + 1) * sizeof(char *));
    const int str_size = 12; // 255 255 255\n

    // Hardcode color strings
    rgb_color_str[0]     = "000 000 000\n";
    rgb_color_str[1]     = "000 255 000\n";
    rgb_color_str[2]     = "000 000 255\n";
    rgb_color_str[3]     = "255 255 000\n";
    rgb_color_str[4]     = "255 000 255\n";
    rgb_color_str[5]     = "000 255 255\n";
    rgb_color_str[6]     = "255 000 000\n";
    rgb_color_str[7]     = "255 255 255\n";
    rgb_color_str[8]     = "128 000 000\n";
    rgb_color_str[9]     = "000 128 000\n";
    rgb_color_str[10]    = "000 000 128\n";

    char ** gray_color_str = (char **) malloc((NEWTON_ITERATIONS + 1) * sizeof(char *));
    int buffer_size;
    for (int ix = 0; ix < NEWTON_ITERATIONS + 1; ix++){
        buffer_size = (3*number_of_digits(ix) + 4);
        gray_color_str[ix] = (char *) malloc(buffer_size * sizeof(char)); // num digits + whitespace
        snprintf(gray_color_str[ix], buffer_size, "%d %d %d\n", ix, ix, ix);
    }

    char * buffer_tmp = (char *) malloc(str_size * size * sizeof(char));
    int buffer_size = str_size * size; 
    uint8_t root_index;
    uint8_t iteration;
    int n_copied_rgb;
    int n_copied_gray;
    char * attractor_row_str = (char*) malloc(sizeof(char) * buffer_size);
    char * convergence_row_str = (char*) malloc(sizeof(char) * buffer_size);

    // We increments ix in the inner loop whenever a main_thread process has finished.

    for (int ix = 0, idx_end; ix < size;)
    {
        // We wait until new lines are available.
        for (mtx_lock(mtx); ;)
        {
            idx_end = size;
            for (int tx = 0; tx < n_threads; tx++)
            {
                if (idx_end > status[tx].val)
                {
                    idx_end = status[tx].val;
                }
            }
            if (idx_end <= ix)
            {
                // We rely on spurious wake-ups which happen in practice 
                // but are not guaranteed.
                cnd_wait(cnd, mtx);
            }
            else
            {
                mtx_unlock(mtx);
                break;
            }
        }
        for (; ix < idx_end; ix++)
        {
            n_copied_rgb = 0;
            n_copied_gray = 0;
            
            for (int jx = 0; jx < size; jx++)
            {
                root_index = attractors[ix][jx];
                memcpy(attractor_row_str + n_copied_rgb, rgb_color_str[root_index], str_size);
                n_copied_rgb += str_size;

                iteration = convergences[ix][jx];
                memcpy(convergence_row_str + n_copied_gray, gray_color_str[iteration], strlen(gray_color_str[iteration]));
                n_copied_gray += strlen(gray_color_str[iteration]);
            }
            fwrite(attractor_row_str, sizeof(char), n_copied_rgb, attractor_file);
            fwrite(convergence_row_str, sizeof(char), n_copied_gray, convergence_file);
        }
    }
    free(rgb_color_str);
    for (int ix = 0; ix < NEWTON_ITERATIONS; ix++)
        free(gray_color_str[ix]);
    free(gray_color_str);
    free(attractor_row_str);
    free(convergence_row_str);
}

int main(int argc, char * argv[])
{
    int opt;

    // We start off by parsing command line arguments.
    while((opt = getopt(argc, argv, "t:1:")) != -1)
    {
        switch (opt)
        {
        case 't':
            n_threads = atoi(optarg);
            if (n_threads < 1)
            {
                printf("\nERROR! Invalid number of threads.\n");
                exit(1);
            }
            break;
        case 'l':
            lsize = atoi(optarg);
            if (lsize < 1)
            {
                printf("\nERROR! Invalid image size.\n");
                exit(1);
            }
            break;
        default:
            break;
        
        default:
            break;
        }
    }
    degree = atoi(argv[3]);
    if(degree < 0 || degree > 9)
    {
        printf("\nError! Invalid degree value.\n");
        exit(1);
    }
    printf("\nThread count: %i\n Image size: %i\n Degree: %i\n", n_threads, lsize, degree);

    initialize_roots(degree);

    const int size = lsize;

    char attr_file_name[26];
    char conv_file_name[27];
    sprintf(attr_file_name, "newton_attractors_x%d.ppm", degree);
    sprintf(conv_file_name, "newton_convergence_x%d.ppm", degree);
    
    FILE * attrFile = fopen(attr_file_name, "w");
    FILE * convFile = fopen(conv_file_name, "w");

    uint8_t * attractors_entries = (uint8_t*) malloc(sz*sz*sizeof(uint8_t));
    uint8_t ** attractors = (uint8_t**) malloc(sz*sizeof(uint8_t*));

    uint8_t * convergence_entries = (uint8_t*) malloc(sz*sz*sizeof(uint8_t));
    uint8_t ** convergences = (uint8_t**) malloc(sz*sizeof(uint8_t*));

    for ( int ix = 0, jx = 0; ix < sz; ++ix, jx+=sz )
    {
        attractors[ix] = attractors_entries + jx;
        convergences[ix] = convergence_entries + jx;
    }

    thrd_t threads[n_threads];
    thread_info_t threads_info[n_threads];

    thrd_t write_thread;
    write_thread_info_t write_thread_info;

    mtx_t mtx;
    mtx_init(&mtx, mtx_plain);

    cnd_t cnd;
    cnd_init(&cnd);

    padded_int status[n_threads];

    for (int thread_idx = 0; thread_idx < n_threads; thread_idx++)
    {
        threads_info[thread_idx].attractors = attractors;
        threads_info[thread_idx].convergences = convergences;
        threads_info[thread_idx].idx_begin = idx_begin;
        threads_info[thread_idx].step_size = step_size;
        threads_info[thread_idx].thread_idx = thread_idx;
        threads_info[thread_idx].mtx = &mtx;
        threads_info[thread_idx].cnd = &cnd;
        threads_info[thread_idx].status = &status;
        status[thread_idx].val = -1;

        int r = thrd_create(thrds+tx, main_thread, (void*) (threads_info+thread_idx));
        if ( r != thrd_success ) {
            fprintf(stderr, "ERROR! Failed to create thread.\n");
            exit(1);
        }
        thrd_detach(threads[thread_idx]);
    }
    {
        write_thread_info.attractors = attractors;
        write_thread_info.convergences = convergences;
        write_thread_info.size = sz;
        write_thread_info.n_threads = nthrds;
        write_thread_info.mtx = &mtx;
        write_thread_info.cnd = &cnd;
        write_thread_info.status = status;
        write_thread_info.attractor_file = attractor_file;
        write_thread_info.convergence_file = convergence_file;

        int r = thrd_create(&write_thread, main_write_thread, (void*) (&write_thread_info));
        if ( r != thrd_success ) {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
    }
    {
        int r;
        thrd_join(write_thread, &r);
    }
    // Print out result in terminal as a test
    if (*argv[argc-1] == 'P')
    {
        print_attractors(attractors);
    }
    {
        free(attractors_entries);
        free(attractors);
        free(convergence_entries);
        free(convergences);

        mtx_destroy(&mtx);
        cnd_destroy(&cnd);

        fflush(attractor_file);
        fclose(attractor_file);
        fflush(convergence_file);
        fclose(convergence_file);
    }
}

static inline double complex newton_iteration(double complex x)
{
    switch (degree)
    {
    case 1:
        return 1.;
    case 2:
        return (x - (1./2.)*x + 1./(2.*x)); // = (x - (x*x - 1.0) / (2.0 * x))
    case 3:
        return (x - (1./3.)*x + 1./(3.*x*x));
    case 4:
        return (x - (1./4.)*x + 1./(4.*x*x*x));
    case 5:
        return (x - (1./5.)*x + 1./(5.*x*x*x*x));
    case 6:
        return (x - (1./6.)*x + 1./(6.*x*x*x*x*x));
    case 7:
        return (x - (1./7.)*x + 1./(7.*x*x*x*x*x*x));
    case 8:
        return (x - (1./8.)*x + 1./(8.*x*x*x*x*x*x*x));
    case 9:
        return (x - (1./9.)*x + 1./(9.*x*x*x*x*x*x*x*x));
    
    default:
        printf("Error, degree value is invalid!\n");
        break;
    }
}

static inline void initialize_roots(int d)
{
    switch (d)
    {
    case 1:
        actual_roots[0] = 1.+0.*I;
        break;
    case 2:
        actual_roots[0] = -1.+0.*I;
        actual_roots[1] = 1.+0.*I;
        break;
    case 3:
        actual_roots[0] = 1.+0.*I;
        actual_roots[1] = -0.500000-0.86603*I;
        actual_roots[2] = -0.500000+0.86603*I;
        break;
    case 4:
        actual_roots[0] = 1.+0.*I;
        actual_roots[1] = -1.+0.*I;
        actual_roots[2] = 0.+1.*I;
        actual_roots[3] = -0.+1.*I;
        break;
    case 5:
        actual_roots[0] = -0.80902-0.58779*I;
        actual_roots[1] = -0.80902+0.58779*I;
        actual_roots[2] = 0.30902-0.95106*I;
        actual_roots[3] = 0.30902+0.95106*I;
        actual_roots[4] = 1.+0.*I;
        break;
    case 6:
        actual_roots[0] = 1.+0.*I;
        actual_roots[1] = -1.+0.*I;
        actual_roots[2] = 0.50000+0.86603*I;
        actual_roots[3] = -0.50000-0.86603*I;
        actual_roots[4] = 0.50000-0.86603*I;
        actual_roots[5] = -0.50000+0.86603*I;
        break;
    case 7:
        actual_roots[0] = -0.90097-0.43388*I;
        actual_roots[1] = -0.90097+0.43388*I;
        actual_roots[2] = 0.62349+0.78183*I;
        actual_roots[3] = 0.62349-0.78183*I;
        actual_roots[4] = -0.22252-0.97493*I;
        actual_roots[5] = -0.22252+0.97493*I;
        actual_roots[6] = 1.+0.*I;
        break;
    case 8:
        actual_roots[0] = 1.+0.*I;
        actual_roots[1] = -1.+0.*I;
        actual_roots[2] = 0.+1.*I;
        actual_roots[3] = 0-1.*I;
        actual_roots[4] = -0.70711-0.70711*I;
        actual_roots[5] = 0.70711+0.70711*I;
        actual_roots[6] = -0.70711+0.70711*I;
        actual_roots[7] = 0.70711-0.70711*I;
        break;
    case 9:
        actual_roots[0] = 1.+0.*I;
        actual_roots[1] = -0.93969-0.34202*I;
        actual_roots[2] = -0.93969+0.34202*I;
        actual_roots[3] = 0.76604+0.64279*I;
        actual_roots[4] = 0.76604-0.64279*I;
        actual_roots[5] = -0.50000-0.86603*I;
        actual_roots[6] = -0.50000+0.86603*I;
        actual_roots[7] = 0.17365+0.98481*I;
        actual_roots[8] = 0.17365-0.98481*I;
        break;
    
    default:
        printf("\nINVALID DEGREE\n");
        exit(1);
    }
}

static inline uint8_t compare_roots(double complex x_iter)
{
    double error = 1.;
    uint8_t index = 0;

    double complex delta_complex;
    double delta_re, delta_im;
    
    for (int i = 0; i < degree; i++)
    {
        delta_complex = x_iter - actual_roots[i];
        delta_re = creal(delta_complex);
        delta_im = cimag(delta_complex);

        error = delta_re * delta_re + delta_im * delta_im;

        if (error < ROOT_ERROR)
        {
            index = i+1;
            break;
        }
    }
    return index;
}

void print_attractors(uint8_t ** attractors)
{
    for (int ix = 0; ix < lsize; ix++)
    {
        for (int jx = 0; jx < lsize; jx++)
        {
            printf("%d", attractors[ix][jx]);
        }
        printf("\n");
    }
}

static inline int number_of_digits(int number) {
    int count = 0;

    /* Run loop till num is greater than 0 */
    do {
        /* Increment digit count */
        count++;

        /* Remove last digit of 'num'. Division by 10 sufficies since it is an int */
        number /= 10;
    } while(number != 0);
    return count;
}