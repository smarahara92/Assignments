#ifndef _LDR_H
#define _LDR_H

#define BASE_LENGTH sizeof(unsigned long)

#define ROUNDUP(length) (((length + 7) / 8) + (BASE_LENGTH - 1)) / BASE_LENGTH

#define E_INPUT_MISMATCH 0x50
#define E_INSUFFICIENT_SPACE 0x51

#define SUCCESS 0x10

struct LDR {
	unsigned long length;
	unsigned long *pdata;
};

typedef struct LDR ldr;

ldr * ldr_create(unsigned long length);

int ldr_make(ldr *ldrvar, char *numinhex);

void ldr_copy(ldr *src, ldr *dest);

void ldr_xadd(ldr *xaugend, ldr *xaddend);

ldr * ldr_add(ldr *augend, ldr *addend);

ldr * ldr_sub(ldr *minuend, ldr *subtrahend);

void ldr_shift(ldr *shiftnum, unsigned long shift, int direction);

ldr * ldr_mul(ldr *multiplicand, ldr *multiplier);

ldr * ldr_gcd(ldr *aa, ldr *bb);

char * ldr_get_str(ldr *data);

void ldr_void_str(ldr *data);

ldr * ldr_invert(ldr *a, ldr *n);

ldr * ldr_powm(ldr *base, ldr *raise, ldr *modn);

ldr * ldr_import(char *text, unsigned long length);

#endif
