#include "ecm_params.h"

/**
 * Get ECM parameters for the n:th round taken from 
 * A practical analysis of the elliptic curve factoring algorithm,
 * Silverman et al.
 * 
 * B1 - The maximum coefficient for repeated addition in stage 1
 * B2 - The limit for stage 2
 * L - The maximum amount of curves
 * 
 * Input: The round > 0 for which parameters should be found
 * Returns: The number of base 10-digits in the prime factor
 * which is expected to be found in this round.
 */
 int ecm_params(int round, mpz_t *B1, mpz_t *B2, mpz_t *L) {
	 if (round == 1) {
		 mpz_set_ui(*B1, 53);
		 mpz_set_ui(*B2, 2650);
		 mpz_set_ui(*L, 4);
		 return 6;
	 }
	 if (round == 2) {
		 mpz_set_ui(*B1, 156);
		 mpz_set_ui(*B2, 7176);
		 mpz_set_ui(*L, 8);
		 return 8;
	 }
	 if (round == 3) {
		 mpz_set_ui(*B1, 405);
		 mpz_set_ui(*B2, 19440);
		 mpz_set_ui(*L, 14);
		 return 10;
	 }
	 if (round == 4) {
		 mpz_set_ui(*B1, 962);
		 mpz_set_ui(*B2, 42328);
		 mpz_set_ui(*L, 25);
		 return 12;
	 }
	 if (round == 5) {
		 mpz_set_ui(*B1, 4777);
		 mpz_set_ui(*B2, 215010);
		 mpz_set_ui(*L, 62);
		 return 16;
	 }
	 if (round == 6) {
		 mpz_set_ui(*B1, 9004);
		 mpz_set_ui(*B2, 405180);
		 mpz_set_ui(*L, 106);
		 return 18;
	 }
	 if (round == 7) {
		 mpz_set_ui(*B1, 18436);
		 mpz_set_ui(*B2, 792791);
		 mpz_set_ui(*L, 161);
		 return 20;
	 }
	 if (round == 8) {
		 mpz_set_ui(*B1, 34155);
		 mpz_set_ui(*B2, 1400000);
		 mpz_set_ui(*L, 259);
		 return 8;
	 }
	 if (round == 9) {
		 mpz_set_ui(*B1, 66596);
		 mpz_set_ui(*B2, 2660000);
		 mpz_set_ui(*L, 376);
		 return 24;
	 }
	 if (round == 10) {
		 mpz_set_ui(*B1, 133297);
		 mpz_set_ui(*B2, 5330000);
		 mpz_set_ui(*L, 512);
		 return 26;
	 }
	 // Any further rounds
	 mpz_set_ui(*B1, 280000);
     mpz_set_ui(*B2, 10000000);
     mpz_set_ui(*L, 1024);
     return 30;
 }
