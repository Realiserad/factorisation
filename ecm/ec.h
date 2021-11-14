#ifndef __EC
#define __EC

#include "mini-gmp.h"
#include <stdbool.h>

typedef struct { 
	mpz_t a, b, N; 
} ec_t;

typedef struct {
	mpz_t x, y;
} ec_point_t;

void ec_init();
void point_clear(ec_point_t *p); // kör mpz_clear och free 
bool point_equals(ec_point_t  *  a, ec_point_t  *  b);
ec_t* curve_create(mpz_t  *  a, mpz_t  *  b, mpz_t  *  N);

ec_t* curve_create_empty();
void curve_create_random(ec_t *curve, ec_point_t *P, mpz_t modulus);

ec_point_t* point_create(mpz_t const * const x, mpz_t const * const y);
ec_point_t* point_create_empty();
int point_add(ec_point_t *res, ec_point_t  *  p, ec_point_t  *  q, ec_t  *  ec);

int point_mul(ec_point_t *res, ec_point_t const * const p, mpz_t const * const k, ec_t  *  curve);
void curve_clear(ec_t *c);
bool curve_is_smooth(mpz_t *a, mpz_t *b, mpz_t N);

#endif
