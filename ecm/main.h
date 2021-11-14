typedef struct {
	// number: composite number to factor
	// B1: max repeated additions for phase 1
	// B2: max divisions for phase 2
	// L: max amount of curves 
	// out: The factor found
	mpz_t number, B1, B2, out, L;
	// id: A number identifying this thread
	// success: 1/0 if factor is found on this thread
	int id, success;
	// curve: The elliptic curve
	ec_t *curve;
	// The number of curves tested so far
	mpz_t *curves_tested;
	// point: The start point on the elliptic curve
	ec_point_t *point;
	// 1/0 if threads should stop
	volatile int *stop;
	// Mutex used to lock operations shared by many threads
	pthread_mutex_t *rand_mutex;
} ecm_thread_context_t;

void __test_ec();
void random_curve(ecm_thread_context_t*);
queue_t* complete_factorization(mpz_t*);
void ecm_threaded_factorization(mpz_t*, mpz_t*);
