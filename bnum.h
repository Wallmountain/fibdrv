#ifndef BNUM_H
#define BNUM_H

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#define bnum_size unsigned int
#define bnum_digit unsigned char

typedef struct bnum {
    bnum_digit *ptr;
    bnum_size size;
    bnum_size capacity;
} bnum;

#define BNUM_INIT                                  \
    {                                              \
        {                                          \
            .ptr = NULL, .size = 0, .capacity = 0, \
        }                                          \
    }

#define MASK_R 0xF0
#define MASK_L 0xF
#define digit_min(a, b) (a ^ ((a ^ b) && (a > b)))
#define digit_max(a, b) (a ^ ((a ^ b) && (a < b)))

#define bnum_swap(x, y) \
    do {                \
        bnum temp = x;  \
        x = y;          \
        y = temp;       \
    } while (0)

#define bnum_free(x) kfree((x).ptr)

#define bnum_ptr_resize(x, n)                             \
    do {                                                  \
        if (n > x->capacity) {                            \
            x->size = 0;                                  \
            x->capacity = n;                              \
            x->ptr = krealloc(x->ptr, n + 1, GFP_KERNEL); \
        }                                                 \
        memset(x->ptr, '\0', x->capacity);                \
    } while (0)

#define bnum_ptr_new(x, n)                   \
    do {                                     \
        x->capacity = n;                     \
        x->ptr = kmalloc(n + 1, GFP_KERNEL); \
    } while (0)

#define BNUM_ALLOC(x, n)               \
    do {                               \
        bnum *__x = (x);               \
        bnum_size __n = (n);           \
        __n = (__n >> 1) + (__n & 1);  \
        if (__x->ptr)                  \
            bnum_ptr_resize(__x, __n); \
        else                           \
            bnum_ptr_new(__x, __n);    \
    } while (0)

void bnum_init(bnum *a);
// compare two bnum (ignore sign)
int bnum_cmp(const bnum *a, const bnum *b);
void bnum_add(bnum *total, const bnum *a, const bnum *b);
void bnum_sub(bnum *total, const bnum *a, const bnum *b);
void bnum_cal_size(bnum *a, bnum_size len);
int digit_add_c(bnum_digit *dest,
                const bnum_digit *a,
                const bnum_digit *b,
                bnum_size size);
bnum_size digit_addi(bnum_digit *dest, const bnum_digit *a);
void digit_cpy(bnum_digit *dest, const bnum_digit *a, bnum_size size);
void digit_add(bnum *total,
               const bnum_digit *a,
               bnum_size asize,
               const bnum_digit *b,
               bnum_size bsize);
void digit_sub(bnum_digit *total,
               const bnum_digit *a,
               bnum_size asize,
               const bnum_digit *b,
               bnum_size bsize);
void bnum_mul(bnum *total, const bnum *a, const bnum *b);
void bnum_lshift(bnum *dest, bnum *a);
void digit_mul_add(bnum_digit *dest,
                   const bnum_digit *a,
                   bnum_size asize,
                   int b);
int digit_ctoi(bnum_digit a);
#endif
