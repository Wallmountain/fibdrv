#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MASK_R 0xF0
#define MASK_L 0xF

#define FIB_DEV "/dev/fibonacci"

void print_bn(char *a, int len);

int main()
{
    char buf[300];
    int offset = 500; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        long long sz;
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 3);
        printf("%3d ", i);
        print_bn(buf, sz);
        printf("\n");
    }
    close(fd);
    return 0;
}

void print_bn(char *a, int len)
{
    if (len < 1)
        return;
    if (len == 1 && a[0] == 0) {
        printf("0");
        return;
    }
    if (a[--len] & MASK_R)
        printf("%d", (a[len] & MASK_R) >> 4);
    printf("%d", a[len] & MASK_L);
    while (len > 0) {
        --len;
        printf("%d%d", (a[len] & MASK_R) >> 4, a[len] & MASK_L);
    }
}
