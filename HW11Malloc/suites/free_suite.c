// Homework 11 Tests, Fall 2018
// Tests based off of ones written by Austin Adams. Formatting and tests and randomness by Maddie Brickell
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

/*Special setup for free*/
void setup_malloc_free(void) {
    setup_malloc_malloc();
    init_malloc_test(1, false);
}

void teardown_malloc_free(void) {
    teardown_malloc_malloc();
}

//
// free() tests
//

/*Free Null*/
void setup_free_null(void) {
    my_malloc_errno = OUT_OF_MEMORY;
    my_free(NULL);
}

//tests to make sure you checked error code correctly
START_TEST(test_free_null_gen) {
    call_freelist("actual/Free_Null.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

/*Free Bad Canary from Metadata*/
void setup_free_bad_meta_canary(void) {
    my_malloc_errno = OUT_OF_MEMORY;
    metadata_t *meta = (metadata_t *)(my_sbrk_fake_heap + 128);
    meta->size = 64;
    meta->canary = 0xBEEF;
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    *trailing_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    my_free(meta + 1);
}

//Correct Error Code
START_TEST(test_free_bad_meta_canary_gen) {
    call_freelist("actual/Free_Bad_Meta_Canary.txt");
    ck_assert_int_eq(my_malloc_errno, CANARY_CORRUPTED);
    ck_assert(!size_list && !address_list);
}
END_TEST

/*Free End Canary*/
void setup_free_bad_end_canary(void) {
    my_malloc_errno = OUT_OF_MEMORY;
    metadata_t *meta = (metadata_t *)(my_sbrk_fake_heap + 128);
    meta->size = 64;
    meta -> canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    *trailing_canary = 0xBEEF;
    my_free(meta + 1);
}

//Correct Error Code
START_TEST(test_free_bad_end_canary_gen) {
    call_freelist("actual/Free_Bad_End_Canary.txt");
    ck_assert_int_eq(my_malloc_errno, CANARY_CORRUPTED);
    ck_assert(!size_list && !address_list);
}
END_TEST

/*Free with an Empty Freelist*/
metadata_t* meta;
void setup_free_empty_freelist(void) {
    my_malloc_errno = OUT_OF_MEMORY;
    meta = (metadata_t *)(my_sbrk_fake_heap + 64);
    meta->size = 128 + TOTAL_METADATA_SIZE;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    my_free(meta + 1);
}

//check the lists 
START_TEST(test_free_empty_freelist_lists) {
    call_freelist("actual/Free_Empty_Freelist.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //check address_list
    ck_assert(address_list);
    ck_ptr(address_list, meta);
    ck_assert(!address_list -> next_addr);
    ck_assert(!address_list -> prev_addr);

    //check size_list
    ck_assert(size_list);
    ck_ptr(size_list, meta);
    ck_assert(!size_list -> next_size);
    ck_assert(!size_list -> prev_size);
}
END_TEST

//check if remainder in free list is correct correct
START_TEST(test_free_empty_freelist_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_ptr(meta, my_sbrk_fake_heap + 64);
    ck_assert_int_eq(meta -> size, 128 + TOTAL_METADATA_SIZE);
}
END_TEST

/*No Merge 1*/
void setup_free_no_merge1(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    
    create_situation_1(&A, &B, &C);

    meta = (metadata_t *)((uint8_t *)B + B->size + 8);
    meta->size = 32 + TOTAL_METADATA_SIZE;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check the freelists
START_TEST(test_free_no_merge1_lists) {
    call_freelist("actual/Free_NoMerge1.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, B);
    ck_ptr(B -> next_addr, meta);
    ck_ptr(meta -> next_addr, C);
    ck_assert(!C -> next_addr);
    //check address list back to front
    ck_ptr(C -> prev_addr, meta);
    ck_ptr(meta -> prev_addr, B);
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(meta, size_list);
    ck_ptr(meta -> next_size, B);
    ck_ptr(B -> next_size, A);
    ck_ptr(A -> next_size, C);
    ck_assert(!C -> next_size);
    //check size list back to front
    ck_ptr(C -> prev_size, A);
    ck_ptr(A -> prev_size, B);
    ck_ptr(B -> prev_size, meta);
    ck_assert(!meta -> prev_size);
}
END_TEST

//check the block in the freelist for correct metadata
START_TEST(test_free_no_merge1_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(meta -> size, 32 + TOTAL_METADATA_SIZE);
    ck_ptr(meta, (uint8_t*)B + B -> size + 8);
}
END_TEST

/*No Merge 2*/
void setup_free_no_merge2(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    
    create_situation_1(&A, &B, &C);

    meta = (metadata_t *)my_sbrk_fake_heap;
    meta->size = 500;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}


//check for correctness in the freelist
START_TEST(test_free_no_merge2_lists) {
    call_freelist("actual/Free_NoMerge2.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, meta);
    ck_ptr(meta -> next_addr, A);
    ck_ptr(A -> next_addr, B);
    ck_ptr(B -> next_addr, C);
    ck_assert(!C -> next_addr);
    //check address list back to front
    ck_ptr(C -> prev_addr, B);
    ck_ptr(B -> prev_addr, A);
    ck_ptr(A -> prev_addr, meta);
    ck_assert(!meta -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(B, size_list);
    ck_ptr(B -> next_size, A);
    ck_ptr(A -> next_size, meta);
    ck_ptr(meta -> next_size, C);
    ck_assert(!C -> next_size);
    //check size list back to front
    ck_ptr(C -> prev_size, meta);
    ck_ptr(meta -> prev_size, A);
    ck_ptr(A -> prev_size, B);
    ck_assert(!B -> prev_size);
}
END_TEST

//check if the metadata of the inserted block is correct
START_TEST(test_free_no_merge2_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(meta -> size, 500);
    ck_ptr(meta, my_sbrk_fake_heap);
}
END_TEST


/*Left Merge 1*/
unsigned long a_size;
metadata_t* prev_a;
void setup_free_left_merge1(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    
    create_situation_1(&A, &B, &C);
    prev_a = A;
    a_size = A -> size;
    meta = (metadata_t *)((uint8_t*)A + A -> size);
    meta->size = 64;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//Check to make sure the freelists are correct
START_TEST(test_free_left_merge1_lists) {
	call_freelist("actual/Free_LeftMerge1.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);

    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, B);
    ck_ptr(B -> next_addr, C);
    ck_assert(!C -> next_addr);
    //check address list back to front
    ck_ptr(C -> prev_addr, B);
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(B, size_list);
    ck_ptr(B -> next_size, A);
    ck_ptr(A -> next_size, C);
    ck_assert(!C -> next_size);
    //check size list back to front
    ck_ptr(C -> prev_size, A);
    ck_ptr(A -> prev_size, B);
    ck_assert(!B -> prev_size);
}
END_TEST

//check to make sure the inserted block is correct
START_TEST(test_free_left_merge1_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(A -> size, a_size + 64);
    ck_ptr(A, prev_a);
}
END_TEST

/*Left Merge 2 *The* GIANT*/
unsigned long b_size;
metadata_t* prev_b;
void setup_free_left_merge2(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    prev_b = B;
    b_size = B -> size;
    meta = (metadata_t *)((uint8_t*)B + B -> size);
    //to make the ...GIANT BLOCK... we gotta get rid of C. Adios Amigos!
    A -> next_size = NULL;
    B -> next_addr = NULL;
    C -> prev_addr = NULL;
    C -> prev_size = NULL;

    meta->size = SBRK_SIZE - 1472 - 2*TOTAL_METADATA_SIZE;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check for correct freelists
START_TEST(test_free_left_merge2_lists) {
    call_freelist("actual/Free_LeftMerge2.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);

    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, B);
    ck_assert(!B -> next_addr);
    //check address list back to front
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(A, size_list);
    ck_ptr(A -> next_size, B);
    ck_assert(!B -> next_size);
    //check size list back to front
    ck_ptr(B -> prev_size, A);
    ck_assert(!A -> prev_size);
}
END_TEST

//check to make sure the inserted block is correct
START_TEST(test_free_left_merge2_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(B -> size, b_size + SBRK_SIZE - 1472 - 2*TOTAL_METADATA_SIZE);
    ck_ptr(B, prev_b);
}
END_TEST

/*Right Merge 1 the *right* GIANT*/
unsigned long a_size;
void setup_free_right_merge1(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    a_size = A -> size;
    meta = (metadata_t *)my_sbrk_fake_heap;

    meta->size = 512;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check to see if the freelist is correct
START_TEST(test_free_right_merge1_lists) {
    call_freelist("actual/Free_RightMerge1.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, meta);
    ck_ptr(meta -> next_addr, B);
    ck_ptr(B -> next_addr, C);
    ck_assert(!C -> next_addr);
    //check address list back to front
    ck_ptr(C -> prev_addr, B);
    ck_ptr(B -> prev_addr, meta);
    ck_assert(!meta -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(B, size_list);
    ck_ptr(B -> next_size, C);
    ck_ptr(C -> next_size, meta);
    ck_assert(!meta -> next_size);
    //check size list back to front
    ck_ptr(meta -> prev_size, C);
    ck_ptr(C -> prev_size, B);
    ck_assert(!B -> prev_size);
}
END_TEST

//Check to see if the inserted block is correct
START_TEST(test_free_right_merge1_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(meta -> size, a_size + 512);
    ck_ptr(meta, my_sbrk_fake_heap);
}
END_TEST

/*Right Merge 2 hehehe*/
unsigned long c_size;
void setup_free_right_merge2(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    c_size = C -> size;
    meta = (metadata_t *)((uint8_t*)C - 255);

    meta->size = 255;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check to make sure the freelist is correct
START_TEST(test_free_right_merge2_lists) {
    call_freelist("actual/Free_RightMerge2.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, B);
    ck_ptr(B -> next_addr, meta);
    ck_assert(!meta -> next_addr);
    //check address list back to front
    ck_ptr(meta -> prev_addr, B);
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(B, size_list);
    ck_ptr(B -> next_size, A);
    ck_ptr(A -> next_size, meta);
    ck_assert(!meta -> next_size);
    //check size list back to front
    ck_ptr(meta -> prev_size, A);
    ck_ptr(A -> prev_size, B);
    ck_assert(!B -> prev_size);
}
END_TEST

//check to make sure the block is inserted correctly
START_TEST(test_free_right_merge2_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(meta -> size, c_size + 255);
    ck_ptr(meta, (uint8_t*)C - 255);
}
END_TEST

/*Double Merge 1*/
unsigned long b_size;
unsigned long a_size;
void setup_free_double_merge1(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    b_size = B -> size;
    a_size = A -> size;
    prev_a = A;
    meta = (metadata_t *)((uint8_t*)A + A -> size);

    meta->size = 128;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check to make sure the freelist is correct
START_TEST(test_free_double_merge1_lists) {
    call_freelist("actual/Free_DoubleMerge1.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, C);
    ck_assert(!C -> next_addr);
    //check address list back to front
    ck_ptr(C -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(A, size_list);
    ck_ptr(A -> next_size, C);
    ck_assert(!C -> next_size);
    //check size list back to front
    ck_ptr(C -> prev_size, A);
    ck_assert(!A -> prev_size);
}
END_TEST

//check to make sure the block is inserted correctly
START_TEST(test_free_double_merge1_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(A -> size, b_size + 128 + a_size);
    ck_ptr(A, prev_a);
}
END_TEST

/*Double Merge 2*/
unsigned long b_size;
unsigned long c_size;
void setup_free_double_merge2(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    prev_b = B;
    b_size = B -> size;
    c_size = C -> size;
    meta = (metadata_t *)((uint8_t*)B + B -> size);

    meta->size = 256;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check to make sure the lists are completely correct
START_TEST(test_free_double_merge2_lists) {
    call_freelist("actual/Free_DoubleMerge2.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list front to back
    ck_ptr(address_list, A);
    ck_ptr(A -> next_addr, B);
    ck_assert(!B -> next_addr);
    //check address list back to front
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    ck_assert(size_list);
    //check size list front to back
    ck_ptr(A, size_list);
    ck_ptr(A -> next_size, B);
    ck_assert(!B -> next_size);
    //check size list back to front
    ck_ptr(B -> prev_size, A);
    ck_assert(!A -> prev_size);
}
END_TEST

//check to make sure the student inserted the block correctly
START_TEST(test_free_double_merge2_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(B -> size, b_size + 256 + c_size);
    ck_ptr(B, prev_b);
}
END_TEST

/*DOUBLE DECKER FOR DANIEL BECKER, AKA THE BIGGEST BLOCK YOU'VE EVA SEEEEEEN*/
unsigned long b_size;
unsigned long c_size;
unsigned long a_size;
void setup_free_double_decker(void) {
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;

    // Setup the freelist and fake heap to represent situation 1
    create_situation_1(&A, &B, &C);
    prev_a = A;
    b_size = B -> size;
    c_size = C -> size;
    a_size = A -> size;
    meta = (metadata_t *)((uint8_t*)B + B -> size);

    meta->size = 256;
    unsigned long *tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);

    meta = (metadata_t *)((uint8_t*)A + A -> size);

    meta->size = 128;
    tail_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    meta->canary = *tail_canary = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;

    my_free(meta + 1);
}

//check the lists for correctness
START_TEST(test_free_double_decker_lists) {
    call_freelist("actual/Free_DoubleDecker.txt");
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert(address_list);
    //check address list 
    ck_ptr(A, address_list);
    ck_assert(!A -> next_addr);
    ck_assert(!A -> prev_addr);

    //check size list
    ck_ptr(A, size_list);
    ck_assert(!A -> next_size);
    ck_assert(!A -> prev_size);
    
}
END_TEST

//check that the inserted block is correct
START_TEST(test_free_double_decker_insert) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    ck_assert_int_eq(A -> size, a_size + 128 + b_size + 256 + c_size);
    ck_ptr(A, prev_a);

    //make sure you don't have pointers flying everywhere
    ck_assert(!B -> next_addr);
    ck_assert(!B -> prev_addr);
    ck_assert(!B -> next_size);
    ck_assert(!B -> prev_size);

    ck_assert(!C -> next_addr);
    ck_assert(!C -> prev_addr);
    ck_assert(!C -> next_size);
    ck_assert(!C -> prev_size);

}
END_TEST

Suite *free_suite(void) {
    Suite *s = suite_create("free");

    
    // free() tests
    
    TCase *free1 = tcase_create("Free_Null");
    tcase_add_checked_fixture(free1, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free1, setup_free_null, NULL);
    tcase_add_test(free1, test_free_null_gen);
    suite_add_tcase(s, free1);

    TCase *free2 = tcase_create("Free_Bad_Meta_Canary");
    tcase_add_checked_fixture(free2, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free2, setup_free_bad_meta_canary, NULL);
    tcase_add_test(free2, test_free_bad_meta_canary_gen);
    suite_add_tcase(s, free2);

    TCase *free3 = tcase_create("Free_Bad_End_Canary");
    tcase_add_checked_fixture(free3, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free3, setup_free_bad_end_canary, NULL);
    tcase_add_test(free3, test_free_bad_end_canary_gen);
    suite_add_tcase(s, free3);

    TCase *free4 = tcase_create("Free_Empty_Freelist");
    tcase_add_checked_fixture(free4, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free4, setup_free_empty_freelist, NULL);
    tcase_add_test(free4, test_free_empty_freelist_insert);
    tcase_add_test(free4, test_free_empty_freelist_lists);
    suite_add_tcase(s, free4);

    TCase *free5 = tcase_create("Free_NoMerge1");
    tcase_add_checked_fixture(free5, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free5, setup_free_no_merge1, NULL);
    tcase_add_test(free5, test_free_no_merge1_insert);
    tcase_add_test(free5, test_free_no_merge1_lists);
    suite_add_tcase(s, free5);

    TCase *free6 = tcase_create("Free_NoMerge2");
    tcase_add_checked_fixture(free6, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free6, setup_free_no_merge2, NULL);
    tcase_add_test(free6, test_free_no_merge2_insert);
    tcase_add_test(free6, test_free_no_merge2_lists);
    suite_add_tcase(s, free6);

    TCase *free7 = tcase_create("Free_LeftMerge1");
    tcase_add_checked_fixture(free7, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free7, setup_free_left_merge1, NULL);
    tcase_add_test(free7, test_free_left_merge1_insert);
    tcase_add_test(free7, test_free_left_merge1_lists);
    suite_add_tcase(s, free7);

    TCase *free8 = tcase_create("Free_LeftMerge2");
    tcase_add_checked_fixture(free8, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free8, setup_free_left_merge2, NULL);
    tcase_add_test(free8, test_free_left_merge2_insert);
    tcase_add_test(free8, test_free_left_merge2_lists);
    suite_add_tcase(s, free8);

    TCase *free9 = tcase_create("Free_RightMerge1");
    tcase_add_checked_fixture(free9, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free9, setup_free_right_merge1, NULL);
    tcase_add_test(free9, test_free_right_merge1_insert);
    tcase_add_test(free9, test_free_right_merge1_lists);
    suite_add_tcase(s, free9);

    TCase *free10 = tcase_create("Free_RightMerge2");
    tcase_add_checked_fixture(free10, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free10, setup_free_right_merge2, NULL);
    tcase_add_test(free10, test_free_right_merge2_insert);
    tcase_add_test(free10, test_free_right_merge2_lists);
    suite_add_tcase(s, free10);

    TCase *free11 = tcase_create("Free_DoubleMerge1");
    tcase_add_checked_fixture(free11, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free11, setup_free_double_merge1, NULL);
    tcase_add_test(free11, test_free_double_merge1_insert);
    tcase_add_test(free11, test_free_double_merge1_lists);
    suite_add_tcase(s, free11);

    TCase *free12 = tcase_create("Free_DoubleMerge2");
    tcase_add_checked_fixture(free12, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free12, setup_free_double_merge2, NULL);
    tcase_add_test(free12, test_free_double_merge2_insert);
    tcase_add_test(free12, test_free_double_merge2_lists);
    suite_add_tcase(s, free12);

    TCase *free13 = tcase_create("Free_DoubleDecker");
    tcase_add_checked_fixture(free13, setup_malloc_free, teardown_malloc_free);
    tcase_add_checked_fixture(free13, setup_free_double_decker, NULL);
    tcase_add_test(free13, test_free_double_decker_insert);
    tcase_add_test(free13, test_free_double_decker_lists);
    suite_add_tcase(s, free13);

    return s;

}
