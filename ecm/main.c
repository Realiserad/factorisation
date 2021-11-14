#include <stdio.h>
#include <assert.h>
#include "mini-gmp.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "ec.h"
#include "prime.h"
#include "ecm.h"
#include "log.h"
#include "queue.h"
#include "main.h"
#include "ecm_params.h"
#include "pollard.h"

#define N_THREADS 4

/**
 * Tries to find a factor of a composite number.
 * The argument should contain an ecm_thread_context
 * containing the number N to factor.
 * 
 * This function is not quaranteed to find a factor
 * of N, since finite number of curves are checked.
 */ 
void* do_factorization(void *arg)
{
	ecm_thread_context_t *ctx = (ecm_thread_context_t*) arg;
	
	random_curve(ctx);
	ctx->success = 0;
	
	while (*(ctx->stop) == 0 && mpz_cmp(*(ctx->curves_tested), ctx->L) < 0) {
		if (0 == ecm(&(ctx->out), ctx->curve, ctx->point, &(ctx->B1), ctx->stop)) {
			ctx->success = 1;
			*(ctx->stop) = 1;
		} else {
			pthread_mutex_lock(ctx -> rand_mutex);
			mpz_add_ui(*(ctx->curves_tested), *(ctx->curves_tested), 1);
			pthread_mutex_unlock(ctx -> rand_mutex);
			
			curve_clear(ctx->curve);
			point_clear(ctx->point);
			random_curve(ctx);
		}
	}

	return ctx;
}

void random_curve(ecm_thread_context_t *ctx)
{
	ec_t *curve = curve_create_empty();
	ec_point_t *point = point_create_empty();
	ctx->point = point;
	ctx->curve = curve;

	pthread_mutex_lock(ctx -> rand_mutex);
	curve_create_random(ctx->curve, ctx->point, ctx->number);
	pthread_mutex_unlock(ctx -> rand_mutex);
}

void clear_result(ecm_thread_context_t *result)
{
	if (result == NULL) 
		return;
		
	mpz_clear(result->number);
	mpz_clear(result->B1);
	mpz_clear(result->B2);
	mpz_clear(result->L);
	mpz_clear(result->out);
	curve_clear(result->curve);
	point_clear(result->point);
	free(result);
}

/**
 * Performs a complete factorization of
 * an integer.
 */ 
queue_t* complete_factorization(mpz_t *number)
{
	queue_t* primes = queue_create();
	queue_t* queue = queue_create();
	mpz_t *first = malloc(sizeof(mpz_t)); // copy number to heap
	mpz_init_set(*first, *number); 
	enqueue(queue, first);
	while (queue->size > 0) {
		mpz_t *next = dequeue(queue);
		if (mpz_probab_prime_p(*next, 35) != 0) {
			enqueue(primes, next);
			continue;
		}
		mpz_t *factor = malloc(sizeof(mpz_t));
		mpz_init(*factor); 
		//ecm_threaded_factorization(factor, next);
		if (pollard(factor, next) != 0) {
			LOG("pollard fail");
			break;
		}

		enqueue(queue, factor);
		mpz_divexact(*next, *next, *factor);
		enqueue(queue, next);
	}
	free(queue);
	return primes;
}

/**
 * Threaded factorization using Elliptic Curve Method.
 * 
 * This function is responsible for creating threads
 * and invoking do_factorization with correct parameters
 * until a factor has been found.
 * 
 * The number to factor must be composite.
 */ 
void ecm_threaded_factorization(mpz_t *out, mpz_t *number) {
	ecm_thread_context_t *result;
	pthread_t threads[N_THREADS];
	pthread_mutex_t rand_mutex;
	pthread_mutex_init(&rand_mutex, NULL);
	bool factor_found = false;
	char buffer[1024];
	int id = 0;
	
	// ecm_params.c
	int round = 1;
	mpz_t B1, B2, L;
	mpz_init(B1);
	mpz_init(B2);
	mpz_init(L);

	
	while (!factor_found) {
		int stop = 0;
		mpz_t curves_tested;
		mpz_init_set_ui(curves_tested, 0);
		ecm_params(round++, &B1, &B2, &L);
		char B1_str[1024];
		char B2_str[1024];
		char L_str[1024];
		mpz_get_str(B1_str, 10, B1);
		mpz_get_str(B2_str, 10, B2);
		mpz_get_str(L_str, 10, L); 
		//NOTIFY("Starting round %d with B1=%s, B2=%s and L=%s", (round-1), B1_str, B2_str, L_str); 
		for (int i = 0; i < N_THREADS; i++) {
			ecm_thread_context_t *ctx = malloc(sizeof(ecm_thread_context_t));
			mpz_init_set(ctx->number, *number);
			mpz_init_set(ctx->B1, B1);
			mpz_init_set(ctx->B2, B2);
			mpz_init_set(ctx->L, L);
			mpz_init(ctx->out);
			ctx->curves_tested = &curves_tested;
			ctx->rand_mutex = &rand_mutex;
			ctx->id = id++;
			ctx->stop = &stop;
			mpz_init_set_ui(*(ctx->curves_tested), 0);
			pthread_create(&(threads[i]), NULL, do_factorization, ctx);
		}
		
		for (int i = 0; i < N_THREADS; i++) {
			pthread_join((threads[i]), (void **) &result);
			if (result->success && !factor_found) {
				factor_found = true;
				mpz_add_ui(*out, result->out, 0); // copy
				mpz_get_str(buffer, 10, *out); 
				NOTIFY("Found factor %s", buffer);
			}
			clear_result(result);
		}
		
		mpz_clear(curves_tested);
	}
	
	pthread_mutex_destroy(&rand_mutex);
	mpz_clear(B1);
	mpz_clear(B2);
	mpz_clear(L);
}

int main(int argc, char **argv)
{
	__test_queue();
	__test_ec();

	if (argc < 2) {
		printf("Usage: %s number\n", argv[0]);
		return 1;
	}
	
	srand(time(NULL));
	mpz_t number;
	mpz_init_set_str(number, argv[1], 10);
	char buffer[1024];
	
	mpz_get_str(buffer, 10, number);
	printf("Factoring %s\n", buffer);
	
	time_t start = time(NULL);
	queue_t* primes = complete_factorization(&number);
	time_t stop = time(NULL);
	
	printf("--------------------\n");
	printf("Execution time: %lu minutes\n", ((stop - start) / 60));
	group_t *prime_group = queue_group(primes, 0);
	int group_id = 1;
	while (prime_group != NULL) {
		mpz_get_str(buffer, 10, *(prime_group->value));
		printf("%s^%lu ", buffer, prime_group->size);
		free(prime_group);
		prime_group = queue_group(primes, group_id++);
	}
	printf("\n");
	
	mpz_t product;
	mpz_init(product);
	queue_product(primes, &product);
	if (mpz_cmp(product, number) == 0) {
		printf("Factorization complete.\n");
	} else {
		printf("\x1b[31mFactorization not complete.\x1b[0m\n");
	}
	mpz_clear(product);
	
	mpz_t *tmp = dequeue(primes);
	while(tmp != NULL) {
		free(tmp);
		tmp = dequeue(primes);
	}
	queue_clear(primes);
	free(primes);
	mpz_clear(number);
}

/**
 * EC arithmetic unit test.
 */  
void __test_ec()
{
	return;
	// P192: y^2 = x^3 + ax + b mod N 
	char *a_str   = "fffffffffffffffffffffffffffffffefffffffffffffffc";
	char *b_str   = "64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1";
	char *N_str   = "fffffffffffffffffffffffffffffffeffffffffffffffff";
	// t = (t_x, t_y) and s = (s_x, s_y) are two points on this curve
	char *t_x_str = "f22c4395213e9ebe67ddecdd87fdbd01be16fb059b9753a4";
	char *t_y_str = "264424096af2b3597796db48f8dfb41fa9cecc97691a9c79";
	char *s_x_str = "d458e7d127ae671b0c330266d246769353a012073e97acf8";
	char *s_y_str = "325930500d851f336bddc050cf7fb11b5673a1645086df3b";
	// A scalar
	char *d_str   = "a78a236d60baec0c5dd41b33a542463a8255391af64c74ee";
	// Generator
	char *g_x_str = "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012";
	char *g_y_str = "07192b95ffc8da78631011ed6b24cdd573f977a11e794811";
	// Group order
	char *q_str   = "ffffffffffffffffffffffff99def836146bc9b1b4d22831";
	
	char* result = malloc(1024 * sizeof(char));
	mpz_t a, b, N, t_x, t_y, s_x, s_y, d, g_x, g_y, q;
	mpz_init_set_str(a, a_str, 16);
	mpz_init_set_str(b, b_str, 16);
	mpz_init_set_str(N, N_str, 16);
	mpz_init_set_str(t_x, t_x_str, 16);
	mpz_init_set_str(t_y, t_y_str, 16);
	mpz_init_set_str(s_x, s_x_str, 16);
	mpz_init_set_str(s_y, s_y_str, 16);
	mpz_init_set_str(g_x, g_x_str, 16);
	mpz_init_set_str(g_y, g_y_str, 16);
	mpz_init_set_str(d, d_str, 16);
	mpz_init_set_str(q, q_str, 16);

	ec_t *curve = curve_create(&a, &b, &N);
	ec_point_t *t = point_create(&t_x, &t_y);
	ec_point_t *s = point_create(&s_x, &s_y);
	ec_point_t *g = point_create(&g_x, &g_y);
	ec_point_t *r = point_create_empty();

	// Full add t + s
	point_add(r, t, s, curve);
	assert(strcmp(mpz_get_str(result, 16, r->x), 
		"48e1e4096b9b8e5ca9d0f1f077b8abf58e843894de4d0290") == 0);
	assert(strcmp(mpz_get_str(result, 16, r->y), 
		"408fa77c797cd7dbfb16aa48a3648d3d63c94117d7b6aa4b") == 0);
	// Point doubling 2s
	point_add(r, s, s, curve);
	assert(strcmp(mpz_get_str(result, 16, r->x), 
		"30c5bc6b8c7da25354b373dc14dd8a0eba42d25a3f6e6962") == 0);
	assert(strcmp(mpz_get_str(result, 16, r->y), 
		"dde14bc4249a721c407aedbf011e2ddbbcb2968c9d889cf") == 0);
	// d*s
	point_mul(r, s, &d, curve);
	assert(strcmp(mpz_get_str(result, 16, r->x), 
		"1faee4205a4f669d2d0a8f25e3bcec9a62a6952965bf6d31") == 0);
	assert(strcmp(mpz_get_str(result, 16, r->y), 
		"5ff2cdfa508a2581892367087c696f179e7a4d7e8260fb06") == 0);
	// qg = O
	point_mul(r, g, &q, curve);
	assert(strcmp(mpz_get_str(result, 16, r->x), "0") == 0);
	assert(strcmp(mpz_get_str(result, 16, r->y), "0") == 0);
	
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(N);
	mpz_clear(t_x);
	mpz_clear(t_y); 
	mpz_clear(s_x); 
	mpz_clear(s_y);
	mpz_clear(d);
	mpz_clear(g_x);
	mpz_clear(g_y);
	mpz_clear(q);
	free(result);
	free(curve);
	free(t);
	free(s);
	free(g);
	free(r);
}
