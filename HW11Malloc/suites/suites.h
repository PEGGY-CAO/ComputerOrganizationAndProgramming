#include <check.h>
#include <stdbool.h>
#include "../my_malloc.h"

#define HEAP_SIZE 0x2000

Suite *malloc_suite(void);
Suite *free_suite(void);
Suite *calloc_suite(void);
Suite *realloc_suite(void);

/* Used to access the address and size freelists */
extern metadata_t *address_list;
extern metadata_t *size_list;

/*These variables are explained in malloc_suite*/
extern uint8_t *ret;
extern metadata_t *A, *B, *C;
extern bool my_sbrk_called;
extern bool my_sbrk_call_expected;
extern uint8_t *my_sbrk_fake_heap;
extern int my_sbrk_imaginary_prev_calls;
int my_sbrk_calls;

/*These functions are defined/explained in malloc_suite*/
void setup_malloc_malloc(void);
void teardown_malloc_malloc(void);
void *my_sbrk(int increment);
void init_malloc_test(int prev_sbrk_calls, bool should_sbrk);
void create_situation_1(metadata_t **Aout, metadata_t **Bout, metadata_t **Cout);
void setup_malloc_malloc(void);
void teardown_malloc_malloc(void);

/*This is the function to print the freelist*/
void call_freelist();

