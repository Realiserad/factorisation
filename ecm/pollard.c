#include "pollard.h"
#include "mini-gmp.h"
#include "log.h"

int pollard(mpz_t *d, mpz_t *number)
{
	mpz_t x, y;
	mpz_init_set_ui(x, 2); // <-- Du ska välja ett slumpmässigt startvärde
	mpz_init_set_ui(y, 2); 
	mpz_set_ui(*d, 1);


	while (mpz_cmp_ui(*d, 1) == 0) {
		// x = g(x) = x^2 + 1
		mpz_mul(x, x, y); // <-- fel mpz_mul(x, x, x) ska det stå
		mpz_add_ui(x, x, 1);
		mpz_mod(x, x, *number);

		// y = g(y)
		mpz_mul(y, y, y);
		mpz_add_ui(y, y, 1);
		mpz_mod(y, y, *number);

		// y = g(g(y))
		mpz_mul(y, y, y);
		mpz_add_ui(y, y, 1);
		mpz_mod(y, y, *number);

		// d = x-y
		mpz_sub(*d, x, y);

		// d = |x-y|
		mpz_abs(*d, *d);

		// d = gcd(|x-y|, n)
		mpz_gcd(*d, *d, *number); 
	}

	int toreturn = 0;
	if (mpz_cmp(*d, *number) == 0) toreturn = -1;

	mpz_clear(x);
	mpz_clear(y);

	return toreturn;
}
