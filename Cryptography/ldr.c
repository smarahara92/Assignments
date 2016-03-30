#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldr.h"
#include "gmp.h"

ldr * ldr_create(unsigned long lengthinbits) {						//TESTED SUCCESSFULLY
	lengthinbits = ROUNDUP(lengthinbits);
	ldr *tmp = (ldr *) malloc (sizeof(ldr *));
	tmp -> length = lengthinbits;
	tmp -> pdata = (unsigned long *) malloc (sizeof(unsigned long *) * lengthinbits);
	bzero(tmp -> pdata, lengthinbits * 4);
	return tmp;
}

void ldr_clear(ldr *num) {
	free(num -> pdata);
	free(num);
}

int ldr_make(ldr *ldrvar, char *numinhex) {						//TESTED SUCCESSFULLY
	unsigned long x, y, bitstore, looplength, hexlen = strlen(numinhex);
	looplength = (hexlen + ((BASE_LENGTH * 2) - 1)) / (BASE_LENGTH * 2);
	unsigned long nbits = BASE_LENGTH * 8;
	if(ldrvar -> length >= looplength) {
		looplength = ldrvar -> length - looplength;
		for(x = (ldrvar -> length - 1), hexlen--; x < -1; x--) {
			ldrvar -> pdata[x] = 0x00;
			if(x >= looplength) {
				for(y = 0; (y < nbits) && (hexlen < -1) ; y += 4, hexlen--) {
					if((numinhex[hexlen] >= '0') && (numinhex[hexlen] <= '9')) {
						bitstore = (unsigned long) (numinhex[hexlen] - '0') << y;
					} else if((numinhex[hexlen] >= 'A') && (numinhex[hexlen] <= 'F')) {
						bitstore = (unsigned long) (numinhex[hexlen] - 55) << y;
					} else if((numinhex[hexlen] >= 'a') && (numinhex[hexlen] <= 'f')) {
						bitstore = (unsigned long) (numinhex[hexlen] - 87) << y;
					} else {
						return E_INPUT_MISMATCH;
					}
					ldrvar -> pdata[x] = ldrvar -> pdata[x] | bitstore;
				}
			}
		}
		return SUCCESS;
	} else {
		return E_INSUFFICIENT_SPACE;
	}
}

void ldr_copy(ldr *src, ldr *dest) {							//TESTED SUCCESSFULLY
	if(dest -> length < src -> length) {
		free(dest -> pdata);
		dest -> length = src -> length;
		dest -> pdata = (unsigned long *) malloc (src -> length * sizeof(unsigned long *));
	}
	unsigned long x, y;
	for(x = dest -> length - 1, y = src -> length - 1; x < -1; x--) {
		if(y < -1) {
			dest -> pdata[x] = src -> pdata[y--];
		} else {
			dest -> pdata[x] = 0x00;
		}
	}
}

void display(ldr *data) {								//TESTED SUCCESSFULLY
	unsigned long x, y, bitselector, bitstore, bit1store = 0x0F;
	bit1store = bit1store << ((BASE_LENGTH * 8) - 4);
	for(x = 0; x < data -> length; x++) {
		bitselector = bit1store;
		for(y = (BASE_LENGTH * 8) - 4; y < -4; y -= 4) {
			bitstore = (data -> pdata[x] & bitselector) >> y;
			if(bitstore >= 0 && bitstore <= 9) {
				printf("%c", (char)(bitstore + '0'));
			} else if(bitstore >= 10 && bitstore <= 16) {
				printf("%c", (char) (bitstore + 55));
			}
			bitselector = bitselector >> 0x04;
		}
	}
	printf(" ");
}

void ldr_xadd(ldr *xaugend, ldr *xaddend) {						//TESTED SUCCESSFULLY
	unsigned long x, looplength = xaugend -> length - 1, llbit, y = ((BASE_LENGTH * 8) - 1), lastbit, carry;
	int ok;
	do {
		ok = 0;
		lastbit = 0x00;
		for(x = looplength; x < -1; x--) {
			carry = xaugend -> pdata[x] & xaddend -> pdata[x];
			xaugend -> pdata[x] = xaugend -> pdata[x] ^ xaddend -> pdata[x];
			llbit = carry >> y;
			carry = carry << 0x01;
			carry = carry | lastbit;
			xaddend -> pdata[x] = carry;
			lastbit = llbit;
			if(xaddend -> pdata[x] != 0x00) ok = 1;
		}
	} while(ok);
}

ldr * ldr_add(ldr *augend, ldr *addend) {						//TESTED SUCCESSFULLY
	unsigned long length = augend -> length > addend -> length ? augend -> length : addend -> length;
	unsigned long nbits = length * BASE_LENGTH * 8;
	ldr *xaugend = ldr_create(nbits);
	ldr *xaddend = ldr_create(nbits);
	ldr_copy(augend, xaugend);
	ldr_copy(addend, xaddend);
	display(xaugend);
	display(xaddend);
	ldr_xadd(xaugend, xaddend);
	ldr_clear(xaddend);
	return xaugend;
}

ldr * ldr_sub(ldr *minuend, ldr *subtrahend) {						//TESTED SUCCESSFULLY
	unsigned long x, length = minuend -> length > subtrahend -> length ? minuend -> length : subtrahend -> length;
	unsigned long nbits = length * BASE_LENGTH * 8;
	ldr *xminuend = ldr_create(nbits);
	ldr *xsubtrahend = ldr_create(nbits);
	ldr *xadder = ldr_create(nbits);
	xadder -> pdata[length - 1] = 0x01;
	ldr_copy(minuend, xminuend);
	ldr_copy(subtrahend, xsubtrahend);
	for(x = 0; x < length; x++) {
		xsubtrahend -> pdata[x] = ~xsubtrahend -> pdata[x];
	}
	ldr_xadd(xsubtrahend, xadder);
	ldr_xadd(xminuend, xsubtrahend);
	ldr_clear(xsubtrahend);
	ldr_clear(xadder);
	return xminuend;
}

int ldr_compare(ldr *lhs, ldr *rhs) {						//TESTED
	unsigned long length, llength, x, y = 0;
	ldr *xlhs, *xrhs;
	int True = 1, False = -1, *xTrue, *xFalse;
	if(rhs -> length > lhs -> length) {
		length = rhs -> length;
		xlhs = rhs;
		xrhs = lhs;
		xTrue = &False;
		xFalse = &True;
	} else {
		length = lhs -> length;
		xlhs = lhs;
		xrhs = rhs;
		xTrue = &True;
		xFalse = &False;
	}
	llength = length;
	for(x = 0; x < length; x++) {
		if((llength -- > xrhs -> length)) {
			if((xlhs -> pdata[x] != 0)) {
				return *xTrue;
			}
			continue;
		} else {
			if(xlhs -> pdata[x] > xrhs -> pdata[y]) {
				return *xTrue;
			} else if(xlhs -> pdata[x] < xrhs -> pdata[y]){
				return *xFalse;
			}
			y += 1;
		}
	}
	return 0;
}

void ldr_leftshift(ldr *num, unsigned long shift) {
	unsigned long nbits = BASE_LENGTH * 8;
	if(shift <= 0) {
		return;
	} else if(shift < num -> length * nbits) {
		int set = 0xFF, ok = 1;
		unsigned long setter, x;
		memset(&setter, set, BASE_LENGTH);
		unsigned long index = shift / nbits;
		unsigned long bitindex = shift % nbits;
		unsigned long rp = setter >> bitindex;
		unsigned long lp = ~rp, rem = (nbits - bitindex);
		for(x = 0; x < num -> length;) {
			if(index < num -> length) {
				if(ok) {
					num -> pdata[x] = (num -> pdata[index] & rp) << bitindex;
					index += 1;
				} else {
					num -> pdata[x] = num -> pdata[x] | ((num -> pdata[index] & lp) >> rem);
					x++;
				}
				ok = !ok;
			} else {
				if(!ok) {
					ok = 1;
				} else {
					num -> pdata[x] = 0x00;
				}
				x += 1;
			}
		}
	} else {
		bzero(num -> pdata, num -> length * BASE_LENGTH);
	}
}

void ldr_rightshift(ldr *num, unsigned long shift) {
	unsigned long nbits = BASE_LENGTH * 8;
	if(shift <= 0) {
		return;
	} else if(shift < num -> length * nbits) {
		int set = 0xFF, ok = 1;
		unsigned long setter, x;
		memset(&setter, set, BASE_LENGTH);
		unsigned long index = (num -> length - 1) - (shift / nbits);
		unsigned long bitindex = shift % nbits;
		unsigned long rp = setter << bitindex;
		unsigned long lp = ~rp, rem = (nbits - bitindex);
		for(x = num -> length - 1; x < -1;) {
			if(index < -1) {
				if(ok) {
					num -> pdata[x] = (num -> pdata[index] & rp) >> bitindex;
					index -= 1;
				} else {
					num -> pdata[x] = num -> pdata[x] | ((num -> pdata[index] & lp) << rem);
					x--;
				}
				ok = !ok;
			} else {
				if(!ok) {
					ok = 1;
				} else {
					num -> pdata[x] = 0x00;
				}
				x -= 1;
			}
		}
	} else {
		bzero(num -> pdata, num -> length * BASE_LENGTH);
	}
}

ldr * ldr_mul(ldr *multiplicand, ldr *multiplier) {
	unsigned long length = multiplicand -> length > multiplier -> length ? multiplicand -> length : multiplier -> length;
	unsigned long nbits = length * BASE_LENGTH * 8, bitchooser, shift = 0, nnbits = BASE_LENGTH * 8, x, y;
	ldr *xmultiplicand = ldr_create(nbits);
	ldr *xmultiplier = ldr_create(nbits);
	ldr *xxmultiplier = ldr_create(nbits);
	ldr_copy(multiplicand, xmultiplier);
	for(x = length - 1; x < -1; x--) {
		bitchooser = 0x01;
		for(y = 0; y < nnbits; y++) {
			if(multiplier -> pdata[x] & bitchooser) {
				ldr_leftshift(xmultiplier, shift);
				ldr_copy(xmultiplier, xxmultiplier);
				ldr_xadd(xmultiplicand, xxmultiplier);
				shift = 0x00;
			}
			shift += 1;
			bitchooser = bitchooser << 0x01;
		}
	}
	ldr_clear(xmultiplier);
	ldr_clear(xxmultiplier);
	return xmultiplicand;
}

unsigned long ldr_hsbpos(ldr *num) {
	int y;
	unsigned long hsbpos = num -> length * BASE_LENGTH * 8, nbits = BASE_LENGTH * 8, setter = -1, x;
	for(x = 0; x < num -> length; x++) {
		if((num -> pdata[x] & setter) == 0x00) {
			hsbpos -= nbits;
		} else {
			break;
		}
	}
	setter = setter << (nbits - 1);
	for(y = 0; y < 64; y++) {
		if((num -> pdata[x] & setter)) {
			break;
		}
		setter = setter >> 0x01;
		hsbpos -= 0x01;
	}
	return hsbpos;
}

ldr * ldr_div(ldr *dividend, ldr *divisor) {
	unsigned long length = dividend -> length > divisor -> length ? dividend -> length : divisor -> length;
	unsigned long bitchanger, z, nbits = BASE_LENGTH * 8;
	unsigned long nnbits = nbits * length;
	ldr *xdividend = ldr_create(nnbits);
	ldr *xdivisor = ldr_create(nnbits);
	ldr *xxdivisor = ldr_create(nnbits);
	ldr *one = ldr_create(nnbits);
	ldr *xone = ldr_create(nnbits);
	ldr_make(one, "1");
	ldr_copy(dividend, xdividend);
	ldr_copy(divisor, xdivisor);
	unsigned long llen = nnbits - ldr_hsbpos(xdivisor);
	ldr_leftshift(xdivisor, llen);
	llen += 1;
	ldr *quotient = ldr_create(llen);
	unsigned long bitchange, x, y = (nnbits - llen);
	int compvalue;
	for(x = 0; x < quotient -> length; x++) {
		bitchanger = (unsigned long)0x01 << (nbits - y - 1);
		for(; y < nbits; y++) {
			compvalue = ldr_compare(xdividend, xdivisor);
			if(compvalue == 0 || compvalue == 1) {
				ldr_copy(xdivisor, xxdivisor);
				for(z = 0; z < xdivisor -> length; z++) {
					xxdivisor -> pdata[z] = ~xxdivisor -> pdata[z];
				}
				quotient -> pdata[x] = quotient -> pdata[x] | bitchanger;
				ldr_copy(one, xone);
				ldr_xadd(xxdivisor, xone);
				ldr_xadd(xdividend, xxdivisor);
			}
			ldr_rightshift(xdivisor, 1);
			bitchanger = bitchanger >> 0x01;
		}
		y = 0;
	}
	ldr_clear(xdivisor);
	ldr_clear(xdividend);
	ldr_clear(xxdivisor);
	ldr_clear(one);
	ldr_clear(xone);
	return quotient;
}

ldr * ldr_mod(ldr *dividend, ldr *divisor) {
	unsigned long length = dividend -> length > divisor -> length ? dividend -> length : divisor -> length;
	unsigned long nbits = BASE_LENGTH * 8;
	int compvalue;
	unsigned long nnbits = nbits * length;
	ldr *xdividend = ldr_create(nnbits);
	ldr *xdivisor = ldr_create(nnbits);
	ldr *xxdivisor = ldr_create(nnbits);
	ldr *one = ldr_create(nnbits);
	ldr *xone = ldr_create(nnbits);
	ldr_make(one, "1");
	ldr_copy(dividend, xdividend);
	ldr_copy(divisor, xdivisor);
	unsigned long x, y, llen = nnbits - ldr_hsbpos(xdivisor);
	ldr_leftshift(xdivisor, llen);
	llen += 1;
	for(x = 0; x < llen; x++) {
		compvalue = ldr_compare(xdividend, xdivisor);
		if(compvalue == 0 || compvalue == 1) {
			ldr_copy(xdivisor, xxdivisor);
			for(y = 0; y < xdivisor -> length; y++) {
				xxdivisor -> pdata[y] = ~xxdivisor -> pdata[y];
			}
			ldr_copy(one, xone);
			ldr_xadd(xxdivisor, xone);
			ldr_xadd(xdividend, xxdivisor);
		}
		ldr_rightshift(xdivisor, 1);
	}
	ldr_clear(xdivisor);
	ldr_clear(xxdivisor);
	ldr_clear(one);
	ldr_clear(xone);
	return xdividend;
}

void ldr_div_qr(ldr *xquotient, ldr *remainder, ldr *dividend, ldr *divisor) {
	unsigned long length = dividend -> length > divisor -> length ? dividend -> length : divisor -> length;
	unsigned long bitchanger, z, nbits = BASE_LENGTH * 8;
	unsigned long nnbits = nbits * length;
	ldr *xdividend = ldr_create(nnbits);
	ldr *xdivisor = ldr_create(nnbits);
	ldr *xxdivisor = ldr_create(nnbits);
	ldr *one = ldr_create(nnbits);
	ldr *xone = ldr_create(nnbits);
	ldr_make(one, "1");
	ldr_copy(dividend, xdividend);
	ldr_copy(divisor, xdivisor);
	unsigned long llen = nnbits - ldr_hsbpos(xdivisor);
	ldr_leftshift(xdivisor, llen);
	llen += 1;
	ldr *quotient = ldr_create(llen);
	unsigned long bitchange, x, y = (nnbits - llen);
	int compvalue;
	for(x = 0; x < quotient -> length; x++) {
		bitchanger = (unsigned long)0x01 << (nbits - y - 1);
		for(; y < nbits; y++) {
			compvalue = ldr_compare(xdividend, xdivisor);
			if(compvalue == 0 || compvalue == 1) {
				ldr_copy(xdivisor, xxdivisor);
				for(z = 0; z < xdivisor -> length; z++) {
					xxdivisor -> pdata[z] = ~xxdivisor -> pdata[z];
				}
				quotient -> pdata[x] = quotient -> pdata[x] | bitchanger;
				ldr_copy(one, xone);
				ldr_xadd(xxdivisor, xone);
				ldr_xadd(xdividend, xxdivisor);
			}
			ldr_rightshift(xdivisor, 1);
			bitchanger = bitchanger >> 0x01;
		}
		y = 0;
	}
	ldr_clear(xquotient);
	ldr_clear(remainder);
	ldr_clear(xdivisor);
	ldr_clear(xxdivisor);
	ldr_clear(one);
	ldr_clear(xone);
	xquotient = quotient;
	remainder = xdividend;
}

void ldr_random(ldr *ran, ldr *max) {
	srand(time(0));
	unsigned long randd = rand();
	if(randd % 2 == 0) {
		randd += 1;
	}
	unsigned long x, y;
	for(x = ran -> length - 1; x < -1; x--) {
		for(y = 0; y < 2; y++) {
			ran -> pdata[x] = ran -> pdata[x] | (randd << (y * 32));
			randd = rand();
		}
	}
	ldr *tmp = ldr_mod(ran, max);
	ldr_copy(tmp, ran);
	ldr_clear(tmp);
}

ldr * ldr_gcd(ldr *aa, ldr *bb) {
	ldr *a = ldr_create(aa -> length * BASE_LENGTH * 8);
	ldr *b = ldr_create(bb -> length * BASE_LENGTH * 8);
	ldr *gcd = b, *mod;
	ldr *zero = ldr_create(bb -> length * BASE_LENGTH * 8);
	while(ldr_compare(gcd, zero) != 0) {
		mod = ldr_mod(a, gcd);
		ldr_clear(a);
		a = gcd;
		gcd = mod;
	}
	ldr_clear(zero);
	ldr_clear(b);
	return a;
}

char * ldr_get_str(ldr *data) {								//TESTED SUCCESSFULLY
	unsigned long hsbpos = (ldr_hsbpos(data) + 3) / 4;
	char *num = (char *) malloc (hsbpos);
	hsbpos = (hsbpos * 4) - 1;
	unsigned long nbits = BASE_LENGTH * 8;
	unsigned long index = 0, x = (data -> length - 1) - (hsbpos / nbits), y = (hsbpos % nbits ) - 4 + 1, nnbits = (nbits - 4);
	unsigned long bitstore, bitselector = (unsigned long) 0x0F << y;
	nbits -= 0x01;
	for(; x < data -> length; x++) {
		for(; y < -4; y -= 4) {
			bitstore = (data -> pdata[x] & bitselector) >> y;
			if(bitstore >= 0 && bitstore <= 9) {
				num[index] = (char)(bitstore + '0');
			} else if(bitstore >= 10 && bitstore <= 16) {
				num[index] = (char) (bitstore + 55);
			}
			bitselector = bitselector >> 0x04;
			index += 0x01;
		}
		num[index] = '\0';
		y = nnbits;
		bitselector = (unsigned long)0x0F << nnbits;
	}
	return num;
}

void ldr_void_str(ldr *data) {								//TESTED SUCCESSFULLY
	unsigned long hsbpos = (ldr_hsbpos(data) + 3) / 4;
	hsbpos = (hsbpos * 4) - 1;
	unsigned long nbits = BASE_LENGTH * 8;
	unsigned long index = 0, x = (data -> length - 1) - (hsbpos / nbits), y = (hsbpos % nbits ) - 4 + 1, nnbits = (nbits - 4);
	unsigned long bitstore, bitselector = (unsigned long) 0x0F << y;
	nbits -= 0x01;
	for(; x < data -> length; x++) {
		for(; y < -4; y -= 4) {
			bitstore = (data -> pdata[x] & bitselector) >> y;
			if(bitstore >= 0 && bitstore <= 9) {
				printf("%c", (char)(bitstore + '0'));
			} else if(bitstore >= 10 && bitstore <= 16) {
				printf("%c", (char) (bitstore + 55));
			}
			bitselector = bitselector >> 0x04;
			index += 0x01;
		}
		y = nnbits;
		bitselector = (unsigned long)0x0F << nnbits;
	}
	printf(" ");
}

ldr * ldr_invert(ldr *a, ldr *n) {
	char *xa = ldr_get_str(a);
	char *xb = ldr_get_str(n);
	mpz_t xxa, xxb, invert;
	mpz_inits(xxa, xxb, invert, NULL);
	mpz_set_str(xxa, xa, 16);
	mpz_set_str(xxb, xb, 16);
	mpz_invert(invert, xxa, xxb);
	mpz_get_str(xa, 16, invert);
	mpz_clears(xxa, xxb, invert, NULL);
	ldr *invertt = ldr_create(n -> length);
	ldr_make(invertt, xa);
	free(xa);
	free(xb);
	return invertt;
}

ldr * ldr_powm(ldr *base, ldr *raise, ldr *modn) {
	ldr *tmp, *ans = ldr_create(raise -> length * BASE_LENGTH * 8);
	unsigned long selector, x, y, nbits = BASE_LENGTH * 8;
	ldr_make(ans, "1");
	for(x = 0; x < modn -> length; x++) {
		selector = (unsigned long) 0x01 << (nbits - 1);
		for(y = 0; y < nbits; y++) {
			tmp = ldr_mul(ans, ans);
			ldr_clear(ans);
			ans = ldr_mod(tmp, modn);
			ldr_clear(tmp);
			if(raise -> pdata[x] & selector) {
				tmp = ldr_mul(ans, base);
				ldr_clear(ans);
				ans = ldr_mod(tmp, modn);
				ldr_clear(tmp);
			}
			selector = selector >> 0x01;
		}
	}
	return ans;
}

ldr * ldr_import(char *text, unsigned long length) {
	ldr *newvar = ldr_create(length * 8);
	length -= 1;
	unsigned long x, y;
	for(x = newvar -> length - 1; x < -1; x--) {
		for(y = 0; (y < BASE_LENGTH) && (length < -1); y++) {
			newvar -> pdata[x] = newvar -> pdata[x] | ((unsigned long) text[length--] << (y * 8));
		}
	}
	return newvar;
}


//int main(int argc, char *argv[]) {
	/*ldr *a = ldr_create(128);
	ldr *b = ldr_create(128);
	ldr *n = ldr_create(128);;
	//display(n);
	ldr_make(a, "3");
	ldr_make(b, "A");
	ldr_make(n, "2F");
	display(ldr_powmt(a, n));*/
	//ldr_leftshift(a, 64);
	//display(b);
	//printf("\n");
	//ldr_invert(a);
	//ldr_make(b, "A");
	//ldr_make(n, "4");
	/*display(ldr_powm(a, b, n));*/
	//printf("%d\n", gcd(10, 2));
	/*ldr *a = ldr_create(120);
	ldr *b = ldr_create(120);
	ldr_make(a, "FC");
	ldr_make(b, "C");
	display(ldr_gcd(a, b));*/
	//ldr_random(a);
	//display(a);
	/*printf("%ld\n", sizeof(unsigned long));
	ldr *a = ldr_create(120);
	ldr *b = ldr_create(120);
	ldr_make(a, "234AB7FD3");
	ldr_make(b, "5AD676");
	printf("hsb = %lu\n", ldr_hsbpos(b));
	display(ldr_div(a, b));*/
	/*ldr *a = ldr_create("234AB7FD3");
	ldr *b = ldr_create("5AD676");
	ldr_make(a, "8FA279AA5D6E8394F1125675F4A8789ADEB3425A7675F4AB");
	ldr_make(b, "78");
	ldr_rightshift(a, 111);*/
	//ldr_mul(a, b);
	//ldr_leftshift(a, 1);
	//ldr_leftshift(a, 111);
	/*ldr_leftshift(a, 63);
	ldr_leftshift(a, 64);
	ldr_leftshift(a, 65);
	ldr_leftshift(a, 92);
	ldr_leftshift(a, 127);
	ldr_leftshift(a, 128);*/
	/*printf("%p %p\n", a, b);
	printf("%d\n", ldr_compare(a, b));*/
	//char a = (int) 1;
	//printf("%u %u\n", (unsigned int) a, (unsigned int) a << 33);
	/*int a = 10, b = 7;
	while(b > 0) {s
		a = a ^ b;
		b = b << 1;
		b = a & b;
	}
	printf("a = %d\n", a);*/
	/*ldr_shift();
	ldr *augend = ldr_create(156);
	ldr *addend = ldr_create(156);
	ldr_make(addend, "ED9A659C8FBA21504951AEFD5A96184DEFBAED9");
	ldr_make(augend, "AD9EAD984501DC2736A9904EDA9FBEA695238");
	ldr *sum = ldr_add(augend, addend);
	display(augend);
	display(addend);
	display(sum);*/
	/*ldr *tmp = ldr_create(64);
	ldr_make(tmp, "9EADC89D56EFABDC");
	display(tmp);
	ldr *tmp1 = ldr_create(64);
	ldr_make(tmp1, "56EFABDC");
	display(tmp1);
	ldr_copy(tmp, tmp1);
	display(tmp);
	ldr *tmp2 = ldr_create(128);
	ldr_make(tmp2, "9EADC89D56EFABDCAED956EFB7A659C8");
	display(tmp2);
	ldr_copy(tmp2, tmp);
	display(tmp);
	ldr_copy(tmp, tmp1);
	display(tmp1);*/
	//printf("%0x\n", ldr_make(tmp, "9EADC89D56EFABDCAED956EFB7A659C8"));
	/*tmp = ldr_create(128);
	ldr_make(tmp, "9EADC89D56EFABDCAED956EFB7A659C8");
	display(tmp);
	//perror("Error");
	/*"56EFABDC"
	"9EADC89D56EFABDC"
	"AED956EFB7A659C8"*/
//	return 0;
//}
