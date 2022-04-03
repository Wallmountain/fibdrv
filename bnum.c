#include "bnum.h"

void bnum_init(bnum *a)
{
    a->ptr = NULL;
    a->capacity = 0;
    a->size = 0;
}

void bnum_lshift(bnum *dest, bnum *a)
{
    BNUM_ALLOC(dest, a->size + 1);
    bnum_digit *u = dest->ptr, *v = a->ptr;
    const unsigned size = (a->size + 1) >> 1;
    for (int i = 0; i < size; ++i) {
        *u += *v << 1;
        *u += ((*v & MASK_L) >= 5U) ? 6U : 0;
        if ((*v++ >> 4) >= 5U) {
            *u += 96U;
            *(++u) = 1;
        } else
            u++;
    }
    bnum_cal_size(dest, a->size);
}

int bnum_cmp(const bnum *a, const bnum *b)
{
    if (a->size > b->size)
        return 1;
    if (a->size < b->size)
        return -1;
    bnum_size i = (a->size >> 1);
    do {
        if (a->ptr[i] > b->ptr[i])
            return 1;
        if (a->ptr[i] < b->ptr[i])
            return -1;
    } while (i--);
    return 0;
}

void bnum_cal_size(bnum *a, bnum_size len)
{
    bnum_digit tmp = a->ptr[len >> 1];
    if (len & 1)
        tmp &= MASK_R;
    else
        tmp &= MASK_L;
    a->size = len + !!tmp;
}

void bnum_add(bnum *total, const bnum *a, const bnum *b)
{
    bnum_size n = digit_max(a->size, b->size);
    BNUM_ALLOC(total, n + 1);
    digit_add(total, a->ptr, (a->size + 1) >> 1, b->ptr, (b->size + 1) >> 1);
    bnum_cal_size(total, n);
}

void bnum_sub(bnum *total, const bnum *a, const bnum *b)
{
    int cmp = bnum_cmp(a, b);
    if (cmp > 0) {
        BNUM_ALLOC(total, a->size);
        digit_sub(total->ptr, a->ptr, (a->size + 1) >> 1, b->ptr,
                  (b->size + 1) >> 1);
        bnum_size size = ((a->size + 1) >> 1) - 1;
        do {
            if (total->ptr[size]) {
                size = (size << 1) + 1 + !!(total->ptr[size] & 0xF0);
                break;
            }
        } while (size--);
        total->size = size;
    } else {
        BNUM_ALLOC(total, 1);
        total->size = 1;
    }
}

int digit_add_c(bnum_digit *dest,
                const bnum_digit *a,
                const bnum_digit *b,
                bnum_size size)
{
    int carry = 0;
    for (bnum_size i = 0; i < size; i++) {
        unsigned int u = a[i] + b[i] + 0x66U + carry;
        dest[i] = a[i] + b[i] + carry;
        if ((u ^ a[i] ^ b[i]) & 0x10)
            dest[i] += 6U;
        if ((carry = !!(u & 0x100)))
            dest[i] += 0x60U;
    }
    return carry;
}

bnum_size digit_addi(bnum_digit *dest, const bnum_digit *a)
{
    int carry = 1;
    bnum_size i = 0;
    while (carry) {
        unsigned int u = a[i] + 0x66U + carry;
        dest[i] = a[i] + carry;
        if ((u ^ a[i]) & 0x10)
            dest[i] += 6U;
        if ((carry = !!(u & 0x100)))
            dest[i] += 0x60U;
        i++;
    }
    return i;
}

void digit_cpy(bnum_digit *dest, const bnum_digit *a, bnum_size size)
{
    while (size-- != 0) {
        dest[size] = a[size];
    }
}

void digit_add(bnum *total,
               const bnum_digit *a,
               bnum_size asize,
               const bnum_digit *b,
               bnum_size bsize)
{
    if (asize > bsize) {
        bnum_size i = bsize;
        int carry = digit_add_c(total->ptr, a, b, bsize);
        if (carry)
            i += digit_addi(&total->ptr[bsize], &a[bsize]);
        digit_cpy(&total->ptr[i], &a[i], asize - i + 1);
    } else {
        bnum_size i = asize;
        int carry = digit_add_c(total->ptr, a, b, asize);
        if (carry)
            i += digit_addi(&total->ptr[asize], &a[asize]);
        digit_cpy(&total->ptr[i], &a[i], bsize - i + 1);
    }
}

void digit_sub(bnum_digit *total,
               const bnum_digit *a,
               bnum_size asize,
               const bnum_digit *b,
               bnum_size bsize)
{
    bnum_size size = bsize;
    int carry = 0;
    for (bnum_size i = 0; i < size; i++) {
        if (a[i] < b[i] + carry) {
            total[i] = 0xA0U - b[i] + a[i] - carry;
            carry = 1;
        } else {
            total[i] = a[i] - b[i] - carry;
            carry = 0;
        }
        if ((total[i] & MASK_L) > (a[i] & MASK_L))
            total[i] -= 6U;
    }
    while (carry) {
        if (!a[size]) {
            total[size] = 0x99U;
        } else {
            total[size] = a[size] - carry;
            if ((total[size] ^ a[size]) & 0x10U)
                total[size] -= 6U;
            carry = 0;
        }
        ++size;
    }
    for (; size < asize; ++size)
        total[size] = a[size];
}

int digit_ctoi(bnum_digit a)
{
    return (a & MASK_L) + 10 * ((a & MASK_R) >> 4);
}

void bnum_mul(bnum *total, const bnum *a, const bnum *b)
{
    BNUM_ALLOC(total, a->size + b->size);
    bnum_size asize = ((a->size + 1) >> 1) + 1;
    for (bnum_size i = 0, size = (b->size + 1) >> 1; i < size; ++i) {
        if (b->ptr[i])
            digit_mul_add(&total->ptr[i], a->ptr, asize, digit_ctoi(b->ptr[i]));
    }
    bnum_cal_size(total, a->size + b->size - 1);
}

void digit_mul_add(bnum_digit *dest,
                   const bnum_digit *a,
                   bnum_size asize,
                   int b)
{
    int carry = 0;
    for (bnum_size i = 0; i < asize; ++i) {
        carry += digit_ctoi(a[i]) * b + digit_ctoi(dest[i]);
        dest[i] = carry % 10;
        carry /= 10;
        dest[i] += carry % 10 << 4;
        carry /= 10;
    }
}
