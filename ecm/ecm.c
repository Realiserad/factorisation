/**
 * Implementation of Lentra's elliptic curve factorization
 * algorithm running in O(e^(1+o(1)*sqrt(ln p*ln ln p))).
 */
 
#include "mini-gmp.h"
#include "ecm.h"
#include "log.h"

int ecm(mpz_t *out, ec_t *curve, ec_point_t *P, mpz_t *B1, volatile int *stop) {
	mpz_t current;
	mpz_init_set_ui(current, 2);

	mpz_t next_factor;
	mpz_init_set_ui(next_factor, 2);

	mpz_t zero;
	mpz_init_set_ui(zero, 0);

	int toreturn = -1;
	while ( mpz_cmp(current, *B1) < 0 ) {
		if (*stop == 1) {
			break;
		}

		mpz_mul(current, current, next_factor);
		if (-1 == point_mul(P, P, &next_factor, curve)) {
			// Non-invertible element detected, this element has
			// been put in P->x. We can now calculate a factor
			// of N as gcd(P->x, N)
			mpz_gcd(*out, P->x, curve->N);

			toreturn = 0;
			*stop = 1;
			break;
		}

		if (mpz_cmp(zero, P->x) == 0 && mpz_cmp(zero, P->y) == 0) {
			break;
		}
		
		mpz_add_ui(next_factor, next_factor, 1);
	} 

	mpz_clear(current);
	mpz_clear(next_factor);
	mpz_clear(zero);
	return toreturn;
} 
