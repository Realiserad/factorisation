#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mini-gmp.h"

/**
 * Create an empty queue.
 */ 
queue_t* queue_create() {
	queue_t *q = calloc(1, sizeof(queue_t));
	return q;
}

/**
 * Add an element to a queue.
 */
void enqueue(queue_t *q, mpz_t* data) {
	node_t *n = malloc(sizeof(node_t));
	n->data = data;
	n->next = q->head;
	q->head = n;
	q->size++;
}

/**
 * Retrieve the next element from a queue or NULL
 * if the queue is empty.
 */ 
mpz_t* dequeue(queue_t *q) {
	if (0 == q->size) {
		return NULL;
	}
	node_t *n = q->head;
	mpz_t *data = n->data;
	q->head = n->next;
	q->size--;
	free(n);
	return data;
}

/**
 * Clear a queue.
 */ 
void queue_clear(queue_t *q) {
	mpz_t *data = dequeue(q);
	while (data != NULL) {
		data = dequeue(q);
	}
}

/**
 * Convert a queue to an array.
 */ 
mpz_t** queue_to_array(queue_t *q) {
	mpz_t **a = malloc(q->size*sizeof(mpz_t*));
	node_t *current = q->head;
	int i = 0;
	while (current != NULL) {
		a[i] = current->data;
		current = current->next;
		++i;
	}
	return a;
}

/**
 * Multiply all numbers in a queue and outputs the
 * product. The product is zero, if the queue is empty.
 */ 
void queue_product(queue_t *q, mpz_t *result) {
	if (q->size == 0) {
		mpz_set_ui(*result, 0);
		return;
	}
	mpz_t **a = queue_to_array(q);
	mpz_set_ui(*result, 1);
	for (size_t i = 0; i < q->size; ++i) {
		mpz_mul(*result, *result, *a[i]);
	}
	free(a);
}

int __cmp_mpz(const void *arg0, const void *arg1) {
	mpz_t *a, *b;
	a = *((mpz_t**) arg0);
	b = *((mpz_t**) arg1);
	return mpz_cmp(*a, *b);
}

/**
 * Get a group of identical elements from 
 * a queue or NULL if there is no such group.
 * 
 * Example: (88212184) contains four groups
 * Group 0: (11)
 * Group 1: (22)
 * Group 2: (4)
 * Group 3: (888)
 */
group_t* queue_group(queue_t *q, size_t index) {
	if (0 == q->size)
		return NULL;
		
	group_t *g = NULL;
	
	mpz_t **a = queue_to_array(q);
	qsort(a, q->size, sizeof(mpz_t*), __cmp_mpz);
	
	mpz_t *value = a[0];
	size_t grp_num = 0;
	
	for (size_t i = 0; i < q->size; ++i) {
		if (mpz_cmp(*a[i], *value) != 0) {
			++grp_num;
			value = a[i];
		}
		if (grp_num == index) {
			size_t size = 0;
			for (size_t j = i; j < q->size; ++j) {
				if (mpz_cmp(*a[j], *value) != 0) {
					break;
				}
				++size;
			}
			g = malloc(sizeof(group_t));
			g->value = value;
			g->size = size;
			break;
		}
	}
	free(a);
	return g;
}

/**
 * Unit test.
 */
void __test_queue() {
	mpz_t n1, n2, n3, product;
	mpz_init_set_ui(n1, 1);
	mpz_init_set_ui(n2, 2);
	mpz_init_set_ui(n3, 3);
	mpz_init(product);
	mpz_t *n = NULL;
	mpz_t **a = NULL;
	
	queue_t *q = queue_create();
	assert(q->size == 0);
	assert(q->head == NULL);
	
	enqueue(q, &n1);
	assert(q->size == 1);
	n = dequeue(q);
	assert(q->size == 0);
	assert(mpz_cmp_ui(*n, 1) == 0);
	
	n = dequeue(q);
	assert(n == NULL);
	n = dequeue(q);
	assert(n == NULL);
	assert(q->size == 0);
	
	enqueue(q, &n1);
	enqueue(q, &n2);
	enqueue(q, &n3);
	assert(q->size == 3);
	
	queue_product(q, &product);
	assert(mpz_cmp_ui(product, 6) == 0);
	
	a = queue_to_array(q);
	assert(mpz_cmp_ui(*a[0], 3) == 0);
	assert(mpz_cmp_ui(*a[1], 2) == 0);
	assert(mpz_cmp_ui(*a[2], 1) == 0);
	free(a); 
	
	n = dequeue(q);
	assert(mpz_cmp_ui(*n, 3) == 0);
	n = dequeue(q);
	assert(mpz_cmp_ui(*n, 2) == 0);
	n = dequeue(q);
	assert(mpz_cmp_ui(*n, 1) == 0);
	n = dequeue(q);
	assert(n == NULL);
	queue_clear(q);
	
	enqueue(q, &n1);
	dequeue(q);
	enqueue(q, &n1);
	enqueue(q, &n1);
	dequeue(q);
	dequeue(q);
	enqueue(q, &n1);
	enqueue(q, &n1);
	enqueue(q, &n1);
	enqueue(q, &n1);
	dequeue(q);
	dequeue(q);
	dequeue(q);
	dequeue(q);
	enqueue(q, &n1);
	enqueue(q, &n1);
	enqueue(q, &n1);
	dequeue(q);
	dequeue(q);
	enqueue(q, &n2);
	enqueue(q, &n2);
	enqueue(q, &n2);
	
	a = queue_to_array(q);
	assert(mpz_cmp_ui(*a[0], 2) == 0);
	assert(mpz_cmp_ui(*a[1], 2) == 0);
	assert(mpz_cmp_ui(*a[2], 2) == 0);
	assert(mpz_cmp_ui(*a[3], 1) == 0);
	free(a);
	
	queue_clear(q);
	assert(q->size == 0);
	
	group_t *g = queue_group(q, 0);
	assert(g == NULL);
	enqueue(q, &n1);
	g = queue_group(q, 0);
	assert(g->size == 1);
	assert(mpz_cmp_ui(*(g->value), 1) == 0);
	free(g);
	
	enqueue(q, &n2);
	g = queue_group(q, 0);
	assert(g->size == 1);
	assert(mpz_cmp_ui(*(g->value), 1) == 0);
	free(g);
	g = queue_group(q, 1);
	assert(g->size == 1);
	assert(mpz_cmp_ui(*(g->value), 2) == 0);
	free(g);
	g = queue_group(q, 2);
	assert(g == NULL);
	
	enqueue(q, &n2);
	enqueue(q, &n3);
	enqueue(q, &n3);
	enqueue(q, &n3);
	g = queue_group(q, 2);
	assert(g->size == 3);
	assert(mpz_cmp_ui(*(g->value), 3) == 0);
	free(g);
	
	queue_clear(q);
	enqueue(q, &n2);
	enqueue(q, &n1);
	enqueue(q, &n3);
	enqueue(q, &n2);
	enqueue(q, &n2);
	enqueue(q, &n3);
	enqueue(q, &n3);
	enqueue(q, &n1);
	enqueue(q, &n3);
	g = queue_group(q, 0);
	assert(g->size == 2);
	assert(mpz_cmp_ui(*(g->value), 1) == 0);
	free(g);
	g = queue_group(q, 1);
	assert(g->size == 3);
	assert(mpz_cmp_ui(*(g->value), 2) == 0);
	free(g);
	g = queue_group(q, 2);
	assert(g->size == 4);
	assert(mpz_cmp_ui(*(g->value), 3) == 0);
	free(g);
	g = queue_group(q, 3);
	assert(g == NULL);
	
	queue_clear(q);
	free(q);
	mpz_clear(n1);
	mpz_clear(n2);
	mpz_clear(n3);
	mpz_clear(product);
}
