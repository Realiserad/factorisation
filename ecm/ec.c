#include "ec.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mini-gmp.h"
#include "log.h"

/**
 * Implementation of Weierstrass elliptic curves over
 * finite fields on the form y^2 = x^3 + ax + b mod N
 */

static mpz_t zero;

/*void point_print(ec_point_t *p, char *s)*/
/*{*/
	/*printf("%s:\n", s);*/
	/*mpz_out_str(stdout, 16, p->x);*/
	/*printf("\n");*/
	/*mpz_out_str(stdout, 16, p->y);*/
	/*printf("\n");*/
/*}*/

void ec_init() {
	mpz_init(zero);
	mpz_set_ui(zero, 0);
}

void point_clear(ec_point_t *p)
{
	mpz_clear(p->x);
	mpz_clear(p->y);
	free(p);
}

void curve_clear(ec_t *c)
{
	mpz_clear(c->a);
	mpz_clear(c->b);
	mpz_clear(c->N);
	free(c);
}

bool point_equals(ec_point_t  *  a, ec_point_t  *  b) {
	return mpz_cmp(a->x, b->x) == 0 && mpz_cmp(a->y, b->y) == 0;
}

ec_t* curve_create(mpz_t  *  a, mpz_t  *  b, mpz_t  *  N) {
	ec_t* ec = malloc(sizeof(ec_t));
	mpz_init_set(ec->a, *a);
	mpz_init_set(ec->b, *b);
	mpz_init_set(ec->N, *N);
	return ec;
}

ec_t* curve_create_empty()
{
	ec_t* ec = malloc(sizeof(ec_t));
	mpz_init(ec->a);
	mpz_init(ec->b);
	mpz_init(ec->N);
	return ec;
}

void curve_create_random(ec_t *curve, ec_point_t *P, mpz_t modulus) {
	// Generate random x0, y0, a
	mpz_t x0, y0, a;
	mpz_init(x0);
	mpz_init(y0);
	mpz_init(a);

	mpz_t b, tmp;
	mpz_init(tmp);
	mpz_init(b);

	
	do {
		mpz_cdiv_q_ui(tmp, modulus, RAND_MAX);
		mpz_set_si(x0, 1);
		mpz_set_si(y0, 1);
		mpz_set_si(a, 1);
		for (int i=0; mpz_cmp_si(tmp, i) >= 0; i++) {
			mpz_mul_ui(x0, x0, rand());
		}

		for (int i=0; mpz_cmp_si(tmp, i) >= 0; i++) {
			mpz_mul_ui(y0, y0, rand());
		}

		for (int i=0; mpz_cmp_si(tmp, i) >= 0; i++) {
			mpz_mul_ui(a, a, rand());
		}

		// Calculate b = y^2 - x^3 - ax
		// b = y^2
		mpz_mul(b, y0, y0);
		// tmp = x^3
		mpz_powm_ui(tmp, x0, 3, modulus);
		// b = y^2 - x^3
		mpz_sub(b, b, tmp);
		// tmp = a * x
		mpz_mul(tmp, a, x0);
		//  b = y^2 - x^3 - ax
		mpz_sub(b, b, tmp);
	} while (!curve_is_smooth(&a, &b, modulus));
	
	mpz_set(curve->a, a);
	mpz_set(curve->b, b);
	mpz_set(curve->N, modulus);
	mpz_set(P->x, x0);
	mpz_set(P->y, y0);

	mpz_clear(x0);
	mpz_clear(y0);
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(tmp);
}

bool curve_is_smooth(mpz_t *a, mpz_t *b, mpz_t N)
{
	// -4a^3 - 27b^2 != 0 mod N
	mpz_t a3, b2, zero;

	mpz_init(a3);
	mpz_init(b2);
	mpz_init_set_ui(zero, 0);

	// a3 = a^3 mod N
	mpz_powm_ui(a3, *a, 3, N);

	// a3 = -4a^3
	mpz_mul_si(a3, a3, -4);

	// b2 = b^2
	mpz_powm_ui(b2, *b, 2, N);

	// b2 = -27*b^2
	mpz_mul_si(b2, b2, -27);

	// a3 = -4a^3 + (-27b^2) mod N
	mpz_add(a3, a3, b2);
	mpz_mod(a3, a3, N);

	bool toreturn = false;
	if (mpz_cmp(a3, zero) != 0) toreturn = true;

	mpz_clear(a3);
	mpz_clear(b2);
	mpz_clear(zero);

	return toreturn;
}

ec_point_t* point_create(mpz_t const * const x, mpz_t const * const y) {
	ec_point_t *ec_point = malloc(sizeof(ec_point_t));
	mpz_init_set(ec_point->x, *x);
	mpz_init_set(ec_point->y, *y);
	return ec_point;
}


ec_point_t* point_create_empty() {
	ec_point_t *ec_point = malloc(sizeof(ec_point_t));
	mpz_init(ec_point->x);
	mpz_init(ec_point->y);
	return ec_point;
}

int point_add(ec_point_t *res, ec_point_t  *  p, ec_point_t  *  q, ec_t  *  ec) {
	mpz_t tmp, tmp2, k, resX, resY;

	// if p = INF
	if (mpz_cmp(p->x, zero) == 0 && mpz_cmp(p->y, zero) == 0) {
		mpz_set(res->x, q->x);
		mpz_set(res->y, q->y);
		return 0;
	}
	// if q = INF
	if (mpz_cmp(q->x, zero) == 0 && mpz_cmp(q->y, zero) == 0) {
		mpz_set(res->x, p->x);
		mpz_set(res->y, p->y);
		return 0;
	}
	// if p + q = INF

	// k = slope
	mpz_init(k);
	mpz_init(tmp);
	mpz_init(tmp2);
	int toreturn = 0;
	if (point_equals(p, q)) {
		if (mpz_cmp(p->y, zero) == 0) {
			mpz_set_ui(res->x, 0);
			mpz_set_ui(res->y, 0);
			toreturn = 0;
			goto cleanup;
		}
		// tmp = p.x^2
		mpz_mul(tmp, p->x, p->x);
		// k = 3*p.x^2
		mpz_mul_ui(k, tmp, 3);
		// k = 3p.x^2 + a
		mpz_add(k, k, ec->a);
		// tmp = 2*p.y
		mpz_mul_ui(tmp, p->y, 2);
		// tmp = modInv(2*p.y)
		if (0 == mpz_invert(tmp2, tmp, ec->N)) {
			// No inverse in Z_N, we have found a prime
			// factor of N as gcd(tmp, N)
			mpz_set(res->x, tmp);
			toreturn = -1;
			goto cleanup;
		}
		// k = k*modInv(2*p.y)
		mpz_mul(k, k, tmp2);
	} else {
		if (mpz_cmp(p->x, q->x) == 0) {
			mpz_set_ui(res->x, 0);
			mpz_set_ui(res->y, 0);
			toreturn = 0;
			goto cleanup;
		}

		// k = p.y-q.y
		mpz_sub(k, p->y, q->y);
		// tmp = p.x-q.x
		mpz_sub(tmp, p->x, q->x);
		// tmp = modInv(p.x-q.x)
		if (0 == mpz_invert(tmp2, tmp, ec->N)) {
			// No inverse in Z_N
			mpz_set(res->x, tmp);
			toreturn = -1;
			goto cleanup;
		}
		// k = (p.y-q.y) * modInv(p.x-q.x)
		mpz_mul(k, k, tmp2);
	}

	mpz_init(resX);
	mpz_init(resY);
	
	// r.x = k^2
	mpz_mul(resX, k, k);
	// r.x = k^2 - p.x
	mpz_sub(resX, resX, p->x);
	// r.x = k^2 - p.x - q.x
	mpz_sub(resX, resX, q->x);
	// r.x = k^2 - p.x - q.x mod N
	mpz_mod(resX, resX, ec->N);
	
	// r.y = p.x - r.x
	mpz_sub(resY, p->x, resX);
	// r.y = k(p.x - x)
	mpz_mul(resY, k, resY);
	// r.y = k(p.x - x) - p.y
	mpz_sub(resY, resY, p->y);
	// r.y = k(p.x - x) - p.y mod N
	mpz_mod(resY, resY, ec->N);

	mpz_set(res->x, resX);
	mpz_set(res->y, resY);

	mpz_clear(resX);
	mpz_clear(resY);
cleanup:
	mpz_clear(tmp);
	mpz_clear(k);
	mpz_clear(tmp2);

	return toreturn;
} 

int point_mul(ec_point_t *Q, ec_point_t const * const P, mpz_t  const * const d, ec_t *curve)
{
	char *binstr = mpz_get_str(NULL, 2, *d);
	int len = strlen(binstr);

	// kopiera P
	ec_point_t *res = point_create(&P->x, &P->y);
	/*P = point_create(&P->x, &P->y);*/

	// Q = inf
	mpz_set(Q->x, zero);
	mpz_set(Q->y, zero);


	int toreturn = 0;
	for (int i=len-1; i>=0; i--) {
		if (binstr[i] == '1') {
			if (-1 == point_add(Q, Q, res, curve)) {
				// Non-invertible element detected
				toreturn = -1;
				break;
			}
		}
		if (-1 == point_add(res, res, res, curve)) {
			// Non-invertible element detected
			mpz_set(Q->x, res->x);
			toreturn = -1;
			break;
		}
	}

	point_clear(res);
	free(binstr);

	return toreturn;
}
