#ifndef __ECM
#define __ECM
#include "ec.h"
int ecm(mpz_t *out, ec_t *curve, ec_point_t *P, mpz_t *B1, volatile int *stop);
#endif
