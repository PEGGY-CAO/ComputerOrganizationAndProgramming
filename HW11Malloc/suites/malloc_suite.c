// Homework 11 Tests, Fall 2018
// Written by Maddie Brickell, copied most of this from Austin Adams
//
// Warning: much the structure of this file is shamelessly copypasted from
// https://libcheck.github.io/check/doc/check_html/check_3.html

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "suites.h"

#define SUBPTR(a) ((uint8_t*)((((uint8_t*)(a)) - ((uint8_t*)(my_sbrk_fake_heap))) + 1))

#define ck_ptr(A, B) do { \
	uint8_t* _a = SUBPTR((A)); \
	uint8_t* _b = SUBPTR((B)); \
	ck_assert_msg(_a == _b, "Assertion '%s' failed: %s == %#x, %s == %#x", #A" == "#B, #A, _a, #B, _b); \
} while(0)

bool my_sbrk_called;
bool my_sbrk_call_expected;
uint8_t *my_sbrk_fake_heap;
// Determines the position of the break at the beginning of the tests
int my_sbrk_imaginary_prev_calls;
// Fake my_sbrk()
void *my_sbrk(int increment) {
    ck_assert_int_eq(increment, SBRK_SIZE);
    if (my_sbrk_call_expected) {
    	ck_assert_msg(my_sbrk_call_expected, "There was not a block available for this call to my_malloc so you were supposed to call my_sbrk, but did not");
    } else {
		ck_assert_msg(my_sbrk_call_expected, "There was a block available for this call to my_malloc, but you still made a call to my_sbrk when you did not have to");
    }
    ck_assert_msg(!my_sbrk_called, "You should be only calling my_sbrk once in an invocation of my_malloc. Check your my_malloc code, it looks like you are calling my_sbrk more than once.");
    my_sbrk_called = true;

    // Handle OOM
    if (my_sbrk_imaginary_prev_calls == HEAP_SIZE/SBRK_SIZE) {
        return NULL;
    } else {
		my_sbrk_calls = my_sbrk_calls + 1;
        return my_sbrk_fake_heap + my_sbrk_imaginary_prev_calls * SBRK_SIZE;
    }
}

void setup_malloc_malloc(void) {
    address_list = NULL;
    size_list = NULL;
    my_sbrk_called = false;
    my_sbrk_fake_heap = calloc(1, HEAP_SIZE);
    // Out of memory. Shouldn't happen but check anyway
    ck_assert(my_sbrk_fake_heap);
}

void teardown_malloc_malloc(void) {
    free(my_sbrk_fake_heap);
    my_sbrk_fake_heap = NULL;
}

// Called by a test to set up freelist and fake heap
void init_malloc_test(int prev_sbrk_calls, bool should_sbrk) {
    my_sbrk_call_expected = should_sbrk;
    my_sbrk_imaginary_prev_calls = prev_sbrk_calls;
}


void create_situation_1(metadata_t **Aout, metadata_t **Bout, metadata_t **Cout) {
    // Construct the following situation:
    //                  +768+TMS   +960+2*TMS
    //                 /          /
    //                 |         .
    //  +0      +512   |     +896+TMS +1216+2*TMS
    // /       /       |    /    .   /
    // |       |       |    |    |   |
    //  ---------------------------------   ---
    // |///////|       |////|    |///|   ...   |
    // |///////|     A |////|  B |///|   ... C |
    //  ---------------------------------   ---
    //  \_____/\______/\____/\__/\___/\________/
    //     /     |        |   |    \       |
    //   512   256+TMS  128  64+TMS 256  SBRK_SIZE-1216-2*TMS
    // (in use)       (in use)
    //
    // freelist->B->A->C

    // Setup C
    metadata_t *C = (metadata_t *)(my_sbrk_fake_heap + 1216 + 2*TOTAL_METADATA_SIZE);
    C->size = SBRK_SIZE - 1216 - 2*TOTAL_METADATA_SIZE;
    

    // Setup A
    metadata_t *A = (metadata_t *)(my_sbrk_fake_heap + 512);
    A->size = 256 + TOTAL_METADATA_SIZE;
    

    // Setup B
    metadata_t *B = (metadata_t *)(my_sbrk_fake_heap + 896 + TOTAL_METADATA_SIZE);
    B->size = 64 + TOTAL_METADATA_SIZE;
    
    // Setup linked_lists
    address_list = A;
    A -> prev_addr = NULL;
    A -> next_addr = B;
    B -> prev_addr = A;
    B -> next_addr = C;
    C -> prev_addr = B;
    C -> next_addr = NULL;

    size_list = B;
    B -> prev_size = NULL;
    B -> next_size = A;
    A -> prev_size = B;
    A -> next_size = C;
    C -> prev_size = A;
    C -> next_size = NULL;

    // Return all
    if (Aout) *Aout = A;
    if (Cout) *Cout = C;
    if (Bout) *Bout = B;
}

//
// malloc() tests
//

/* MALLOC_INIT*/
uint8_t *ret;

void malloc1_setup(void) {
    init_malloc_test(0, true);

    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_malloc(128);
}

START_TEST(test_malloc1_retvalue) {
    call_freelist("actual/Malloc_Init.txt");
    // Check return value
    ck_assert(ret);
    uint8_t *split_right_choice =  my_sbrk_fake_heap + SBRK_SIZE - sizeof (unsigned long) - 128;
//	ret = SUBPTR(ret);
//	split_right_choice = SUBPTR(split_right_choice);
//	ck_ptr(ret, split_right_choice);
	ck_ptr(ret,   split_right_choice);
    // They set errno 
}
END_TEST

START_TEST(test_malloc1_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    // Check canaries
    metadata_t *meta = (metadata_t *)ret - 1;
    ck_assert_int_eq(meta->size, 128 + TOTAL_METADATA_SIZE);
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta->canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_size);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> next_addr);
}
END_TEST

//checking sbrk call
START_TEST(test_malloc1_sbrkcall) {
    ck_assert(my_sbrk_called);
}   
END_TEST

//checking to see if address_list and size_list are perfectly perfect
START_TEST(test_malloc1_lists) { 
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(size_list);
    ck_assert(!size_list -> next_size);
    ck_assert_int_eq(size_list->size, SBRK_SIZE - 128 - TOTAL_METADATA_SIZE);

    ck_assert(address_list);
    ck_assert(!address_list -> next_addr);
    ck_assert_int_eq(address_list -> size, SBRK_SIZE - 128 - TOTAL_METADATA_SIZE);
    // Check address of this node
    ck_ptr(size_list,  my_sbrk_fake_heap);
    ck_ptr(size_list,  my_sbrk_fake_heap);
}
END_TEST

/*SBRK WORKS*/
void malloc_initial_sbrk_setup(void) {
    init_malloc_test(1, false);
    size_list = (metadata_t *)my_sbrk_fake_heap;
    size_list->size = SBRK_SIZE;
    size_list->next_size = NULL;
    size_list -> prev_size = NULL;

    address_list = (metadata_t *)my_sbrk_fake_heap;
    address_list->size = SBRK_SIZE;
    address_list->next_addr = NULL;
    address_list -> prev_addr = NULL;

    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_malloc(64);
}

//checks to see if the return value is correct and correct error code is set
START_TEST(test_malloc_initial_sbrk_retval) {
    call_freelist("actual/Malloc_Init_Sbrk.txt");
    ck_assert(ret);
    uint8_t *split_right_choice =  my_sbrk_fake_heap + SBRK_SIZE - sizeof (unsigned long) - 64;
    ck_ptr(ret,  split_right_choice);
}
END_TEST

//checks to see if the metadata of the block returned to the user is correct
START_TEST(test_malloc_initial_sbrk_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    metadata_t *meta = (metadata_t *)ret - 1;
    ck_assert_int_eq(meta->size, 64 + TOTAL_METADATA_SIZE);
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta->canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_size);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> next_addr);
}
END_TEST

//checks to see if address_list and size_list are correct
START_TEST(test_malloc_initial_sbrk_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(size_list);
    ck_assert(!size_list->next_size);
    ck_assert_int_eq(size_list->size, SBRK_SIZE - 64 - TOTAL_METADATA_SIZE);
    ck_ptr(size_list,  my_sbrk_fake_heap);

    ck_assert(address_list);
    ck_assert(!address_list->next_addr);
    ck_assert_int_eq(address_list->size, SBRK_SIZE - 64 - TOTAL_METADATA_SIZE);
    ck_ptr(address_list,  my_sbrk_fake_heap);
}
END_TEST

/*PERFECT BLOCK 1*/
uint8_t* ret1;
metadata_t *A, *B, *C;
void malloc_perf_block1_setup(void) {
    init_malloc_test(1, false);
    //change our errno to make sure it gets set
    my_malloc_errno = OUT_OF_MEMORY;
    create_situation_1(&A, &B, &C);
    ret1 = my_malloc(C->size - TOTAL_METADATA_SIZE);
}

//checking to see if the return value is correct and proper error code is set
START_TEST(test_malloc_perf_block1_retvalue) {
    call_freelist("actual/Malloc_Perf_Block1.txt");
    ck_assert(ret1);
    ck_ptr(ret1,  (C + 1));
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//checking to see if the metadata of the block returned to the user is correct
START_TEST(test_malloc_perf_block1_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    metadata_t *meta = (metadata_t*)ret1 - 1;
    ck_assert_int_eq(meta -> size, C -> size);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> prev_size);
    unsigned long expected_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, expected_canary);
    unsigned long* ending_canary = (unsigned long*)((uint8_t*)meta + meta->size - sizeof(unsigned long));
    ck_assert_int_eq(*ending_canary, expected_canary);
}
END_TEST

//checks the address_list and size_list
START_TEST(test_malloc_perf_block1_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //check address_list front to back
    ck_ptr(A,  address_list);
    ck_ptr(A -> next_addr,  B);
    ck_assert(!B -> next_addr);
    //check address_list back to front
    ck_ptr(B -> prev_addr,  A);
    ck_assert(!A -> prev_addr);

    //check size_list front to back
    ck_ptr(B,  size_list);
    ck_ptr(B -> next_size,  A);
    ck_assert(!A -> next_size);
    //check size_list back to front
    ck_ptr(A -> prev_size,  B);
    ck_assert(!B -> prev_size);
}
END_TEST

/*PERFECT BLOCK 2*/
void malloc_perf_block2_setup(void) {
    init_malloc_test(1, false);
    //change our errno to make sure it gets set
    my_malloc_errno = OUT_OF_MEMORY;
    create_situation_1(&A, &B, &C);
    ret1 = my_malloc(B->size - TOTAL_METADATA_SIZE);
}

//checks to see if returned right block and proper error code set
START_TEST(test_malloc_perf_block2_retvalue) {
    call_freelist("actual/Malloc_Perf_Block2.txt");
    ck_assert(ret1);
    ck_ptr(ret1,  (B + 1));
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//checks to see if metadata of the returned block is correct
START_TEST(test_malloc_perf_block2_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    metadata_t *meta = (metadata_t*)ret1 - 1;
    ck_assert_int_eq(meta -> size, B -> size);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> prev_size);
    unsigned long expected_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, expected_canary);
    unsigned long* ending_canary = (unsigned long*)((uint8_t*)meta + meta->size - sizeof(unsigned long));
    ck_assert_int_eq(*ending_canary, expected_canary);
}
END_TEST

//checks to see if address_list and size_list is correct
START_TEST(test_malloc_perf_block2_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //check address_list front to back
    ck_ptr(A,  address_list);
    ck_ptr(A -> next_addr,  C);
    ck_assert(!C -> next_addr);
    //check address_list back to front
    ck_ptr(C -> prev_addr,  A);
    ck_assert(!A -> prev_addr);

    //check size_list front to back
    ck_ptr(A,  size_list);
    ck_ptr(A -> next_size,  C);
    ck_assert(!C -> next_size);
    //check size_list back to front
    ck_ptr(C -> prev_size,  A);
    ck_assert(!A -> prev_size);
}
END_TEST

/*PERFECT BLOCK 3*/
void malloc_perf_block3_setup(void) {
    init_malloc_test(1, false);
    //change our errno to make sure it gets set
    my_malloc_errno = OUT_OF_MEMORY;
    create_situation_1(&A, &B, &C);
    ret1 = my_malloc(A->size - TOTAL_METADATA_SIZE);
}

//checks to see if returned right block
START_TEST(test_malloc_perf_block3_retvalue) {
    call_freelist("actual/Malloc_Perf_Block3.txt");
    ck_assert(ret1);
    ck_ptr(ret1,  (A + 1));
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//checks to make sure metadata of returned block is correct
START_TEST(test_malloc_perf_block3_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    metadata_t *meta = (metadata_t*)ret1 - 1;
    ck_assert_int_eq(meta -> size, A -> size);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> prev_size);
    unsigned long expected_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, expected_canary);
    unsigned long* ending_canary = (unsigned long*)((uint8_t*)meta + meta->size - sizeof(unsigned long));
    ck_assert_int_eq(*ending_canary, expected_canary);

}
END_TEST

//checks to make sure the address_list and size_list are correct
START_TEST(test_malloc_perf_block3_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //check address_list front to back
    ck_ptr(B,  address_list);
    ck_ptr(B -> next_addr,  C);
    ck_assert(!C -> next_addr);
    //check address_list back to front
    ck_ptr(C -> prev_addr,  B);
    ck_assert(!B -> prev_addr);

    //check size_list front to back
    ck_ptr(B,  size_list);
    ck_ptr(B -> next_size,  C);
    ck_assert(!C -> next_size);
    //check size_list back to front
    ck_ptr(C -> prev_size,  B);
    ck_assert(!B -> prev_size);
}
END_TEST

/*SPLIT BLOCK 1*/
void malloc_split_block1_setup(void) {
    init_malloc_test(1, false);

    create_situation_1(&A, &B, &C);

    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_malloc(32);
}

//checking to see if you returned the correct block
START_TEST(test_malloc_split_block1_retvalue) {
    call_freelist("actual/Malloc_Split_Block1.txt");
    ck_assert(ret);
    //metadata_t* meta = (metadata)ret;
    uint8_t *split_right = my_sbrk_fake_heap + 736 + sizeof(metadata_t);
    ck_ptr(ret,  split_right);
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//making sure the remainder of the split block is correct in the freelist
START_TEST(test_malloc_split_block1_splitblock) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //make sure the address is correct
    uint8_t *split_left = (uint8_t*)my_sbrk_fake_heap + 512;
    ck_ptr(A,  split_left);
    //check to see if the remainder is correct in the list
    ck_assert_int_eq(A -> size, 224);
    //check if the canaries of the remainder are correct
    //the pointers of this block will be checked in the freelist section so no need to duplicate that check here
}
END_TEST

//checking to see if the metadata of the returned block is correct
START_TEST(test_malloc_split_block1_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks to make sure all pointers are nulled out
    metadata_t *meta = (metadata_t*)ret - 1;
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> prev_size);
    ck_assert(!meta -> next_size);
    //checks to make sure size is correct
    ck_assert_int_eq(meta -> size, 32 + TOTAL_METADATA_SIZE);
    //checks to make sure canaries are correct
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta -> size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
}
END_TEST

//checking to see if the lists are correct after the operation
START_TEST(test_malloc_split_block1_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks address_list front to back
    ck_ptr(A,  address_list);
    ck_ptr(A -> next_addr,  B);
    ck_ptr(B -> next_addr,  C);
    ck_assert(!C -> next_addr);
    //checks address_list back to front
    ck_ptr(C -> prev_addr,  B);
    ck_ptr(B -> prev_addr,  A);
    ck_assert(!A -> prev_addr);

    //checks size list front to back
    ck_ptr(B,  size_list);
    ck_ptr(B -> next_size,  A);
    ck_ptr(A -> next_size,  C);
    ck_assert(!C -> next_size);
    //checks size list back to front
    ck_ptr(C -> prev_size,  A);
    ck_ptr(A -> prev_size,  B);
    ck_assert(!B -> prev_size);
}
END_TEST

/*SPLIT BLOCK 2*/
void malloc_split_block2_setup(void) {
    init_malloc_test(1, false);

    create_situation_1(&A, &B, &C);

    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_malloc(202);
}

//checking to see if the return value is correct
START_TEST(test_malloc_split_block2_retvalue) {
    call_freelist("actual/Malloc_Split_Block2.txt");
    ck_assert(ret);
    //metadata_t* meta = (metadata)ret;
    uint8_t *split_right = my_sbrk_fake_heap + SBRK_SIZE - 202 - TOTAL_METADATA_SIZE + sizeof(metadata_t);
    ck_ptr(ret,  split_right);
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//checking to see if the remainder is correct in the freelist
START_TEST(test_malloc_split_block2_splitblock) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //make sure the address is correct
    uint8_t *split_left = (uint8_t*)my_sbrk_fake_heap + 1216 + 2*TOTAL_METADATA_SIZE;
    ck_ptr(C,  split_left);
    //check to see if the remainder is correct in the list
    unsigned long c_size = SBRK_SIZE - (1216 + 2*TOTAL_METADATA_SIZE) - (202 + TOTAL_METADATA_SIZE);
    ck_assert_int_eq(C -> size, c_size);
    //the pointers of this block will be checked in the freelist section so no need to duplicate that check here
}
END_TEST

//checking to see if the metadata of the returned block is correct
START_TEST(test_malloc_split_block2_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks to make sure all pointers are nulled out
    metadata_t *meta = (metadata_t*)ret - 1;
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> prev_size);
    ck_assert(!meta -> next_size);
    //checks to make sure size is correct
    ck_assert_int_eq(meta -> size, 202 + TOTAL_METADATA_SIZE);
    //checks to make sure canaries are correct
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta -> size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
}
END_TEST

//check to see if freelists are correct
START_TEST(test_malloc_split_block2_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    ck_assert(size_list);
    //checks address_list front to back
    ck_ptr(A,  address_list);
    ck_ptr(A -> next_addr,  B);
    ck_ptr(B -> next_addr,  C);
    ck_assert(!C -> next_addr);
    //checks address_list back to front
    ck_ptr(C -> prev_addr,  B);
    ck_ptr(B -> prev_addr,  A);
    ck_assert(!A -> prev_addr);

    //checks size list front to back
    ck_ptr(B,  size_list);
    ck_ptr(B -> next_size,  A);
    ck_ptr(A -> next_size,  C);
    ck_assert(!C -> next_size);
    //checks size list back to front
    ck_ptr(C -> prev_size,  A);
    ck_ptr(A -> prev_size,  B);
    ck_assert(!B -> prev_size);
}
END_TEST

/*SPLIT BLOCK 3*/
void malloc_split_block3_setup(void) {
    init_malloc_test(1, false);

    create_situation_1(&A, &B, &C);

    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_malloc(607);
}

//check retvalue correct
START_TEST(test_malloc_split_block3_retvalue) {
    call_freelist("actual/Malloc_Split_Block3.txt");
    ck_assert(ret);
    //metadata_t* meta = (metadata)ret;
    uint8_t *split_right = my_sbrk_fake_heap + SBRK_SIZE - 607 - TOTAL_METADATA_SIZE + sizeof(metadata_t);
    ck_ptr(ret,  split_right);
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//check remainder of block in freelist is correct
START_TEST(test_malloc_split_block3_splitblock) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //make sure the address is correct
    uint8_t *split_left = (uint8_t*)my_sbrk_fake_heap + 1216 + 2*TOTAL_METADATA_SIZE;
    ck_ptr(C,  split_left);
    //check to see if the remainder is correct in the list
    unsigned long c_size = SBRK_SIZE - (1216 + 2*TOTAL_METADATA_SIZE) - (607 + TOTAL_METADATA_SIZE);
    ck_assert_int_eq(C -> size, c_size);
    //the pointers of this block will be checked in the freelist section so no need to duplicate that check here
}
END_TEST

//check metadata of block returned correct
START_TEST(test_malloc_split_block3_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks to make sure all pointers are nulled out
    metadata_t *meta = (metadata_t*)ret - 1;
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> prev_size);
    ck_assert(!meta -> next_size);
    //checks to make sure size is correct
    ck_assert_int_eq(meta -> size, 607 + TOTAL_METADATA_SIZE);
    //checks to make sure canaries are correct
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta -> canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta -> size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
}
END_TEST

//check for correct freelist
START_TEST(test_malloc_split_block3_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks address_list front to back
    ck_ptr(A,  address_list);
    ck_ptr(A -> next_addr,  B);
    ck_ptr(B -> next_addr,  C);
    ck_assert(!C -> next_addr);
    //checks address_list back to front
    ck_ptr(C -> prev_addr,  B);
    ck_ptr(B -> prev_addr,  A);
    ck_assert(!A -> prev_addr);

    //checks size list front to back
    ck_ptr(C,  size_list);
    ck_ptr(C -> next_size,  B);
    ck_ptr(B -> next_size,  A);
    ck_assert(!A -> next_size);
    //checks size list back to front
    ck_ptr(A -> prev_size,  B);
    ck_ptr(B -> prev_size,  C);
    ck_assert(!C -> prev_size);
}
END_TEST

/*Split Block with SBRK MERGE*/
metadata_t* D;
metadata_t* E;
void malloc_split_block_sbrk_merge(void) {
    init_malloc_test(1, true);
    D = (metadata_t*)(my_sbrk_fake_heap + 32);
    D -> size = 128 + TOTAL_METADATA_SIZE;

    E = (metadata_t *)(my_sbrk_fake_heap + SBRK_SIZE - 64 - TOTAL_METADATA_SIZE);
    E -> size = 64 + TOTAL_METADATA_SIZE;

    address_list = D;
    D -> prev_addr = NULL;
    D -> next_addr = E;
    E -> prev_addr = D;
    E -> next_addr = NULL;

    size_list = E;
    E -> prev_size = NULL;
    E -> next_size = D;
    D -> prev_size = E;
    D -> next_size = NULL;

    my_malloc_errno = OUT_OF_MEMORY;
    ret1 = my_malloc(256);
}

//check if sbrk got called correctly
START_TEST(test_malloc_split_block_sbrk_merge_sbrkcalled) {
    call_freelist("actual/Malloc_Split_Block_SBRKmerge.txt");
    ck_assert(my_sbrk_called);
}
END_TEST

//check to make sure you returned the correct block
START_TEST(test_malloc_split_block_sbrk_merge_retvalue) {
    //set errno correctly
     ck_assert_int_eq(my_malloc_errno, NO_ERROR);
     ck_assert(ret1);
     uint8_t* pointer = my_sbrk_fake_heap + 2* SBRK_SIZE - 256 - TOTAL_METADATA_SIZE + sizeof(metadata_t);
     ck_ptr(ret1,  pointer);
}
END_TEST

//check to make sure the metadata of the returned block is correct
START_TEST(test_malloc_split_block_sbrk_merge_metadata) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    metadata_t *meta = (metadata_t *)ret1 - 1;
    ck_assert_int_eq(meta->size, 256 + TOTAL_METADATA_SIZE);
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta->canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
    ck_assert(!meta -> next_addr);
    ck_assert(!meta -> prev_addr);
    ck_assert(!meta -> next_size);
    ck_assert(!meta -> prev_size);
}
END_TEST

//check for a correct remainder of the split block in the freelist
START_TEST(test_malloc_split_block_sbrk_merge_splitblock) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    uint8_t *split_left = (uint8_t*)my_sbrk_fake_heap + SBRK_SIZE - 64 - TOTAL_METADATA_SIZE;
    ck_ptr(E,  split_left);
    //check to see if the remainder is correct in the list
    unsigned long e_size = SBRK_SIZE - 192;
    ck_assert_int_eq(E -> size, e_size);
    //the pointers of this block will be checked in the freelist section so no need to duplicate that check here   
}
END_TEST

//check for correct freelists after operation
START_TEST(test_malloc_split_block_sbrk_merge_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //check address_list front to back
    ck_ptr(address_list,  D);
    ck_ptr(D -> next_addr,  E);
    ck_assert(!E -> next_addr);
    //check address list back to front
    ck_ptr(E -> prev_addr,  D);
    ck_assert(!D -> prev_addr);

    //check size list front to back
    ck_ptr(size_list,  D);
    ck_ptr(D -> next_size,  E);
    ck_assert(!E -> next_size);
    //check size list back to front
    ck_ptr(E -> prev_size,  D);
    ck_assert(!D -> prev_size);
}
END_TEST

/*ZERO*/
void malloc_zero_setup(void) {
    init_malloc_test(0, false);

    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    ret1 = my_malloc(0);
}

//check return value
START_TEST(test_malloc_zero_retvalue) {
    call_freelist("actual/Malloc_Zero.txt");
    ck_assert(!ret1);
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

/*TOOBIG*/
void malloc_toobig_setup(void) {
    init_malloc_test(0, false);

    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    ret1 = my_malloc(SBRK_SIZE);
}

//check return value
START_TEST(test_malloc_toobig_retvalue) {
    call_freelist("actual/Malloc_TooBig.txt");
    ck_assert(!ret1);
    ck_assert_int_eq(my_malloc_errno, SINGLE_REQUEST_TOO_LARGE);
}
END_TEST

/*OOM*/
void malloc_oom_setup(void) {
   init_malloc_test(HEAP_SIZE/SBRK_SIZE, true);

    // Set errno to something else to check if student is setting it
    my_malloc_errno = NO_ERROR;
    ret1 = my_malloc(8); 
}

//check return value
START_TEST(test_malloc_oom_retvalue) {
    call_freelist("actual/Malloc_OOM.txt");
    ck_assert(!ret1);
    ck_assert_int_eq(my_malloc_errno, OUT_OF_MEMORY);
}
END_TEST

Suite *malloc_suite(void) {
    Suite *s = suite_create("malloc");

    TCase *malloc1 = tcase_create("Malloc_Init");
    tcase_add_checked_fixture(malloc1, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc1, malloc1_setup, NULL);
    tcase_add_test(malloc1, test_malloc1_sbrkcall);
    tcase_add_test(malloc1, test_malloc1_lists);
    tcase_add_test(malloc1, test_malloc1_metadata);
    tcase_add_test(malloc1, test_malloc1_retvalue);
    suite_add_tcase(s, malloc1);

    TCase *malloc12 = tcase_create("Malloc_Init_SBRK");
    tcase_add_checked_fixture(malloc12, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc12, malloc_initial_sbrk_setup, NULL);
    tcase_add_test(malloc12, test_malloc_initial_sbrk_lists);
    tcase_add_test(malloc12, test_malloc_initial_sbrk_metadata);
    tcase_add_test(malloc12, test_malloc_initial_sbrk_retval);
    suite_add_tcase(s, malloc12);

    TCase *malloc2 = tcase_create("Malloc_Perf_Block1");
    tcase_add_checked_fixture(malloc2, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc2, malloc_perf_block1_setup, NULL);
    tcase_add_test(malloc2, test_malloc_perf_block1_lists);
    tcase_add_test(malloc2, test_malloc_perf_block1_metadata);
    tcase_add_test(malloc2, test_malloc_perf_block1_retvalue);
    suite_add_tcase(s, malloc2);

    TCase *malloc3 = tcase_create("Malloc_Perf_Block2");
    tcase_add_checked_fixture(malloc3, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc3, malloc_perf_block2_setup, NULL);
    tcase_add_test(malloc3, test_malloc_perf_block2_lists);
    tcase_add_test(malloc3, test_malloc_perf_block2_metadata);
    tcase_add_test(malloc3, test_malloc_perf_block2_retvalue);
    suite_add_tcase(s, malloc3);

    TCase *malloc4 = tcase_create("Malloc_Perf_Block3");
    tcase_add_checked_fixture(malloc4, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc4, malloc_perf_block3_setup, NULL);
    tcase_add_test(malloc4, test_malloc_perf_block3_lists);
    tcase_add_test(malloc4, test_malloc_perf_block3_metadata);
    tcase_add_test(malloc4, test_malloc_perf_block3_retvalue);
    suite_add_tcase(s, malloc4);

    TCase *malloc5 = tcase_create("Malloc_Split_Block1");
    tcase_add_checked_fixture(malloc5, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc5, malloc_split_block1_setup, NULL);
    tcase_add_test(malloc5, test_malloc_split_block1_lists);
    tcase_add_test(malloc5, test_malloc_split_block1_metadata);
    tcase_add_test(malloc5, test_malloc_split_block1_retvalue);
    tcase_add_test(malloc5, test_malloc_split_block1_splitblock);
    suite_add_tcase(s, malloc5);

    TCase *malloc6 = tcase_create("Malloc_Split_Block2");
    tcase_add_checked_fixture(malloc6, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc6, malloc_split_block2_setup, NULL);
    tcase_add_test(malloc6, test_malloc_split_block2_lists);
    tcase_add_test(malloc6, test_malloc_split_block2_metadata);
    tcase_add_test(malloc6, test_malloc_split_block2_retvalue);
    tcase_add_test(malloc6, test_malloc_split_block2_splitblock);
    suite_add_tcase(s, malloc6);

    TCase *malloc7 = tcase_create("Malloc_Split_Block3");
    tcase_add_checked_fixture(malloc7, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc7, malloc_split_block3_setup, NULL);
    tcase_add_test(malloc7, test_malloc_split_block3_lists);
    tcase_add_test(malloc7, test_malloc_split_block3_metadata);
    tcase_add_test(malloc7, test_malloc_split_block3_retvalue);
    tcase_add_test(malloc7, test_malloc_split_block3_splitblock);
    suite_add_tcase(s, malloc7);

    TCase *malloc11 = tcase_create("Malloc_Split_Block_SBRKmerge");
    tcase_add_checked_fixture(malloc11, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc11, malloc_split_block_sbrk_merge, NULL);
    tcase_add_test(malloc11, test_malloc_split_block_sbrk_merge_lists);
    tcase_add_test(malloc11, test_malloc_split_block_sbrk_merge_splitblock);
    tcase_add_test(malloc11, test_malloc_split_block_sbrk_merge_metadata);
    tcase_add_test(malloc11, test_malloc_split_block_sbrk_merge_retvalue);
    tcase_add_test(malloc11, test_malloc_split_block_sbrk_merge_sbrkcalled);
    suite_add_tcase(s, malloc11);

    TCase *malloc8 = tcase_create("Malloc_Zero");
    tcase_add_checked_fixture(malloc8, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc8, malloc_zero_setup, NULL);
    tcase_add_test(malloc8, test_malloc_zero_retvalue);
    suite_add_tcase(s, malloc8);

    TCase *malloc9 = tcase_create("Malloc_TooBig");
    tcase_add_checked_fixture(malloc9, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc9, malloc_toobig_setup, NULL);
    tcase_add_test(malloc9, test_malloc_toobig_retvalue);
    suite_add_tcase(s, malloc9);

    TCase *malloc10 = tcase_create("Malloc_OOM");
    tcase_add_checked_fixture(malloc10, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(malloc10, malloc_oom_setup, NULL);
    tcase_add_test(malloc10, test_malloc_oom_retvalue);
    suite_add_tcase(s, malloc10);
    
    return s;

}
