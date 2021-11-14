#ifndef __QUEUE
#define __QUEUE

#include <stdlib.h>
#include "mini-gmp.h"

struct node {
	mpz_t* data;
	struct node *next;
};

struct queue {
	struct node *head;
	size_t size;
};

struct group {
	size_t size;
	mpz_t *value;
};

typedef struct node node_t;
typedef struct queue queue_t;
typedef struct group group_t;

queue_t* queue_create();
void enqueue(queue_t*, mpz_t*);
mpz_t* dequeue(queue_t*);
void queue_clear(queue_t*);
mpz_t** queue_to_array(queue_t *q);
group_t* queue_group(queue_t *, size_t);
void __test_queue();
int __cmp_mpz(const void*, const void*);
void queue_product(queue_t*, mpz_t*);

#endif
