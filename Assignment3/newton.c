#include <stdlidx_begin.h>
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

#define NEWTON_ITERATIONS   99
#define MAX_DEGREE          9
#define ROOT_ERROR          0.000001

struct colors
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

int degree, lsize, nthreads;

double complex actual_roots[MAX_DEGREE];

typedef struct {
  int val;
  char pad[60]; // cacheline - sizeof(int)
} int_padded;

typedef struct {
    uint8_t ** attractors;
    uint8_t ** convergences;
    int idx_begin;
    int step_size;
    int size;
    int thread_idx;
    mtx_t * mtx;
    cnd_t * cnd;
    int_padded *status;
    } thrd_info_t;

typedef struct {
    uint8_t ** attractors;
    uint8_t ** convergences;
    FILE * attrFile;
    FILE * convFile;
    int n_threads;
    int size;
    mtx_t * mtx;
    cnd_t * cnd;
    int_padded *status;
    } thrd_info_write_t;

int main_thrd(void *args)
{
    const thrd_info_t *thrd_info = (thrd_info_t*) args;
    uint8_t ** attractors = thrd_info->attractors;
    uint8_t ** convergences = thrd_info->convergences;
    const int idx_begin = thrd_info->idx_begin;
    const int step_size = thrd_info->step_size;
    const int size = thrd_info->size;
    const int thread_idx = thrd_info->thread_idx;
    mtx_t * mtx = thrd_info->mtx;
    cnd_t * cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    double complex x_iter;
    uint8_t * attractors_row;
    uint8_t * convergence_row;
    uint8_t root_index;
    uint8_t runs;
    for ( int ix = idx_begin; ix < size; ix += step_size ) 
    {
        attractors_row = (uint8_t*) malloc(size*sizeof(uint8_t));
        convergence_row = (uint8_t*) malloc(size*sizeof(uint8_t));
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

        // In order to illustrate thrd_sleep and to force more synchronization
        // points, we sleep after each line for one micro seconds.
        thrd_sleep(&(struct timespec){.tv_sec=0, .tv_nsec=1000}, NULL);
    }

  return 0;
}

int
main_thrd_write(
    void *args
    )
{
    const thrd_info_write_t *thrd_info = (thrd_info_write_t*) args;
    uint8_t ** attractors = thrd_info->attractors;
    uint8_t ** convergences = thrd_info->convergences;
    FILE * attrFile = thrd_info->attrFile;
    FILE * convFile = thrd_info->convFile;
    const int size = thrd_info->size;
    const int nthrds = thrd_info->nthrds;
    mtx_t * mtx = thrd_info->mtx;
    cnd_t * cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    // Writing the headers to the files.
    {
        int maxColorVal = 255;
        int sss = number_of_digits(lsize);
        int headerSizeAttr = 7 + 2*number_of_digits(lsize) + number_of_digits(maxColorVal);
        char* headerAttr = (char*) malloc(headerSizeAttr * sizeof(char));
        snprintf(headerAttr, headerSizeAttr, "P3\n%d %d\n%d\n", lsize, lsize, maxColorVal);
        int headerSizeConv = 7 + 2*number_of_digits(lsize) + number_of_digits(NEWTON_ITERATIONS);
        char* headerConv = (char*) malloc(headerSizeConv * sizeof(char));
        snprintf(headerConv, headerSizeConv, "P3\n%d %d\n%d\n", lsize, lsize, NEWTON_ITERATIONS);

        fwrite(headerAttr, sizeof(char), strlen(headerAttr), attrFile);
        fwrite(headerConv, sizeof(char), strlen(headerConv), convFile);

        free(headerAttr);
        free(headerConv);
    }

    char** rgbColorsStr = (char **) malloc((MAX_DEGREE + 1) * sizeof(char *));
    int strsize = 12; // 255 255 255\n

    // Hardcode color strings
    rgbColorsStr[0]     = "000 000 000\n";
    rgbColorsStr[1]     = "000 255 000\n";
    rgbColorsStr[2]     = "000 000 255\n";
    rgbColorsStr[3]     = "255 255 000\n";
    rgbColorsStr[4]     = "255 000 255\n";
    rgbColorsStr[5]     = "000 255 255\n";
    rgbColorsStr[6]     = "255 000 000\n";
    rgbColorsStr[7]     = "255 255 255\n";
    rgbColorsStr[8]     = "128 000 000\n";
    rgbColorsStr[9]     = "000 128 000\n";
    rgbColorsStr[10]    = "000 000 128\n";

    char ** grayColorStr = (char **) malloc((NEWTON_ITERATIONS + 1) * sizeof(char *));
    for (int ix = 0; ix < NEWTON_ITERATIONS + 1; ix++){
        int buffersize = (3*number_of_digits(ix) + 4);
        grayColorStr[ix] = (char *) malloc(buffersize * sizeof(char)); // num digits + whitespace
        snprintf(grayColorStr[ix], buffersize, "%d %d %d\n", ix, ix, ix);
    }

    char * tmpBuffer = (char *) malloc(12 * size * sizeof(char));
    int buffersize;
    int attrBuffersize = strsize * size; 
    uint8_t root_index;
    uint8_t iteration;
    int rgbElementsCopied;
    int grayElementsCopied;
    char* attrRowStr = (char*) malloc(sizeof(char) * attrBuffersize);
    char* convRowStr = (char*) malloc(sizeof(char) * attrBuffersize);
    // We do not increment ix in this loop, but in the inner one.
    for ( int ix = 0, idx_beginnd; ix < size; ) {
        // If no new lines are available, we wait.
        for ( mtx_lock(mtx); ; ) {
        // We extract the minimum of all status variables.
        idx_beginnd = size;
            for ( int tx = 0; tx < nthrds; ++tx )
                if ( idx_beginnd > status[tx].val )
                    idx_beginnd = status[tx].val;

            if ( idx_beginnd <= ix )
                // We rely on spurious wake-ups, which in practice happen, but are not
                // guaranteed.
                cnd_wait(cnd,mtx);
            else {
                mtx_unlock(mtx);
                break;
            }
        }
        for ( ; ix < idx_beginnd; ++ix ) {
            rgbElementsCopied = 0;
            grayElementsCopied = 0;
            for (int jx = 0; jx < size; jx++) {
                root_index = attractors[ix][jx];
                memcpy(attrRowStr + rgbElementsCopied, rgbColorsStr[root_index], strsize);
                rgbElementsCopied += strsize;

                iteration = convergences[ix][jx];
                memcpy(convRowStr + grayElementsCopied, grayColorStr[iteration], strlen(grayColorStr[iteration]));
                grayElementsCopied += strlen(grayColorStr[iteration]);
            }

            fwrite(attrRowStr, sizeof(char), rgbElementsCopied, attrFile);
            fwrite(convRowStr, sizeof(char), grayElementsCopied, convFile);
        }
    }

    {
    free(rgbColorsStr);
    for (int ix = 0; ix < NEWTON_ITERATIONS; ix++)
        free(grayColorStr[ix]);
    free(grayColorStr);
    free(attrRowStr);
    free(convRowStr);
    }

    return 0;
}

int main(int argc, char * argv[])
{

    int opt;
    
    /* ------------------- Parsing cmd line args ------------------- */
    while((opt = getopt(argc, argv, "t:l:")) != -1){
        switch(opt){
            case 't':
                nthreads = atoi(optarg); // returns a char*.
                if(nthreads <= 0){
                    printf("Invalid number of threads!\n");
                    return -1;
                }
                break;
            case 'l':
                lsize = atoi(optarg);
                if(lsize <= 0){
                    printf("Invalid image size!\n");
                    return -1;
                }

                break;
            default:
                break;
        }
    }
    degree = atoi(argv[3]);
    if(degree < 0 || degree > 10){
        printf("Error, degree value is invalid!\n");
        return -1;
    }
    printf("Number of threads: %i\n image size: %i\n Degree: %i\n", 
            nthreads, lsize, degree);

    initialize_roots(degree);

    const int size = lsize;

    char attr_file_name[26];
    char conv_file_name[27];
    sprintf(attr_file_name, "newton_attractors_x%d.ppm", degree);
    sprintf(conv_file_name, "newton_convergence_x%d.ppm", degree);
    
    FILE * attrFile = fopen(attr_file_name, "w");
    FILE * convFile = fopen(conv_file_name, "w");

    uint8_t * attractors_entries = (uint8_t*) malloc(size*size*sizeof(uint8_t));
    uint8_t ** attractors = (uint8_t**) malloc(size*sizeof(uint8_t*));

    uint8_t * convergence_entries = (uint8_t*) malloc(size*size*sizeof(uint8_t));
    uint8_t ** convergences = (uint8_t**) malloc(size*sizeof(uint8_t*));

    for ( int ix = 0, jx = 0; ix < size; ++ix, jx+=size )
    {
        attractors[ix] = attractors_entries + jx;
        convergences[ix] = convergence_entries + jx;
    }


    const int nthrds = nthreads;
    thrd_t thrds[nthrds];
    thrd_info_t thrds_info[nthrds];

    thrd_t thrd_write;
    thrd_info_write_t thrd_info_write;
    
    mtx_t mtx;
    mtx_init(&mtx, mtx_plain);

    cnd_t cnd;
    cnd_init(&cnd);

    int_padded status[nthrds];

    for ( int tx = 0; tx < nthrds; ++tx ) {
        thrds_info[tx].attractors = attractors;
        thrds_info[tx].convergences = convergences;
        thrds_info[tx].idx_begin = tx;
        thrds_info[tx].step_size = nthrds;
        thrds_info[tx].size = size;
        thrds_info[tx].tx = tx;
        thrds_info[tx].mtx = &mtx;
        thrds_info[tx].cnd = &cnd;
        thrds_info[tx].status = status;
        status[tx].val = -1;

        int r = thrd_create(thrds+tx, main_thrd, (void*) (thrds_info+tx));
        if ( r != thrd_success ) {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
        thrd_detach(thrds[tx]);
    }
    {
        thrd_info_write.attractors = attractors;
        thrd_info_write.convergences = convergences;
        thrd_info_write.size = size;
        thrd_info_write.nthrds = nthrds;
        thrd_info_write.mtx = &mtx;
        thrd_info_write.cnd = &cnd;
        thrd_info_write.status = status;
        thrd_info_write.attrFile = attrFile;
        thrd_info_write.convFile = convFile;

        int r = thrd_create(&thrd_write, main_thrd_write, (void*) (&thrd_info_write));
        if ( r != thrd_success ) {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
    }

    {
        int r;
        thrd_join(thrd_write, &r);
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

        fflush(attrFile);
        fclose(attrFile);
        fflush(convFile);
        fclose(convFile);
    }
    return 0;
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