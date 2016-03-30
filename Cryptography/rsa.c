#include <stdio.h>
#include <string.h>
#include "ldr.h"

int main() {
	char p_in[257], q_in[257];
	ldr *p = ldr_create(1024);
	printf("Enter \"P\" :: ");
	scanf("%s", p_in);
	ldr_make(p, p_in);
	ldr *q = ldr_create(1024);
	printf("Enter \"Q\" :: ");
	scanf("%s", q_in);
	ldr_make(q, q_in);
	ldr *e = ldr_create(1024);
	ldr *tmp1, *one = ldr_create(1024);
	ldr *two = ldr_create(1024);
	ldr_make(two, "2");
	ldr_make(one, "1");
	ldr *n = ldr_mul(p, q);
				printf("n = ");
				ldr_void_str(n);
				printf("\n");
	ldr *pm1 = ldr_sub(p, one);
				printf("pm1 = ");
				ldr_void_str(pm1);
				printf("\n");
	ldr *qm1 = ldr_sub(q, one);
				printf("qm1 = ");
				ldr_void_str(qm1);
				printf("\n");
	ldr *piofn = ldr_mul(pm1, qm1);
				printf("piofn = ");
				ldr_void_str(piofn);
				printf("\n");
	ldr_clear(pm1);
	ldr_clear(qm1);
	ldr_random(e, piofn);
	ldr *d = ldr_invert(e, piofn);
			printf("d = ");
				ldr_void_str(d);
				printf("\n");
	printf("Enter Plian Text :: ");
	scanf("%s", p_in);
	ldr *pt = ldr_import(p_in, strlen(p_in));
	printf("Plian Text :: ");
	ldr_void_str(pt);
	printf("\n");
	printf("Encrypted Text :: ");
	ldr *ct = ldr_powm(pt, e, n);
	ldr_void_str(ct);
	printf("\n");
	printf("Decrypted Text :: ");
	pt = ldr_powm(ct, d, n);
	ldr_void_str(pt);
	printf("\n");
}
