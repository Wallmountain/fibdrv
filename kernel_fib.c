#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define sample_size 1000

int main(int argc, char **argv)
{
    if (argc < 2)
        return 0;
    char write_buf[] = "testing writing";
    int offset = 100;
    int func = atoi(argv[1]);
    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    /* for each F(i), measure sample_size times of data and
     * remove outliers based on the 95% confidence level
     */
    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        double t[sample_size] = {0};
        double mean = 0.0, sd = 0.0, result = 0.0;
        int count = 0;

        for (int n = 0; n < sample_size; n++) { /* sampling */
            /* get the runtime in kernel space here */
            read(fd, write_buf, func);
            t[n] = (double) write(fd, write_buf, func); /* recursion */
            mean += t[n];                               /* sum */
        }
        mean /= sample_size; /* mean */

        for (int n = 0; n < sample_size; n++) {
            sd += (t[n] - mean) * (t[n] - mean);
        }
        sd = sqrt(sd / (sample_size - 1)); /* standard deviation */

        for (int n = 0; n < sample_size; n++) { /* remove outliers */
            if (t[n] <= (mean + 2 * sd) && t[n] >= (mean - 2 * sd)) {
                result += t[n];
                count++;
            }
        }
        result /= count;

        printf("%d %.5lf samples: %d\n", i, result, count);
    }
    close(fd);
    return 0;
}
