// Homework 11 Tests, Spring 2018
// Tests by Austin Adams, Formatting by Maddie Brickell
// 
//
// Warning: much the structure of this file is shamelessly copypasted from
// https://libcheck.github.io/check/doc/check_html/check_3.html

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "suites.h"

//
// calloc() tests
//

#define SUBPTR(a) ((uint8_t*)((((uint8_t*)(a)) - ((uint8_t*)(my_sbrk_fake_heap))) + 1))

#define ck_ptr(A, B) do { \
	uint8_t* _a = SUBPTR((A)); \
	uint8_t* _b = SUBPTR((B)); \
	ck_assert_msg(_a == _b, "Assertion '%s' failed: %s == %#x, %s == %#x", #A" == "#B, #A, _a, #B, _b); \
} while(0)

/*Initial Calloc*/
void setup_calloc_initial(void) {
    init_malloc_test(0, true);

    // Put a bunch of [feces]
    memset(my_sbrk_fake_heap, 0x69, SBRK_SIZE);
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_calloc(8, 64);
}

//check return value correct
START_TEST(test_calloc_initial_retvalue) {
    call_freelist("actual/Calloc_Init.txt");
    ck_assert(ret);
    uint8_t *split_right_choice =  my_sbrk_fake_heap + SBRK_SIZE - sizeof (unsigned long) - 512;
    ck_ptr(ret, split_right_choice);

    // Check that memory is zeroed out
    for (unsigned int i = 0; i < 512; i++) {
        ck_assert(!ret[i]);
    }
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//check that the metadata is correct as well
START_TEST(test_calloc_initial_metadata) {
    metadata_t *meta = (metadata_t *)ret - 1;
    ck_assert_int_eq(meta->size, 512 + TOTAL_METADATA_SIZE);
    unsigned long canary_expected = ((uintptr_t)meta ^ CANARY_MAGIC_NUMBER) + 1;
    ck_assert_int_eq(meta->canary, canary_expected);
    unsigned long *trailing_canary = (unsigned long *)((uint8_t *)meta + meta->size - sizeof (unsigned long));
    ck_assert_int_eq(*trailing_canary, canary_expected);
}
END_TEST

//check that the student called sbrkcall
START_TEST(test_calloc_initial_sbrkcall) {
    ck_assert(my_sbrk_called);
}
END_TEST

//check the freelists
START_TEST(test_calloc_initial_lists) {
    ck_assert(size_list);
    ck_assert(!size_list -> next_size);
    ck_assert(!size_list -> prev_size);
    ck_assert_int_eq(size_list->size, SBRK_SIZE - 512 - TOTAL_METADATA_SIZE);
    ck_ptr((uint8_t *)size_list, my_sbrk_fake_heap);

    ck_assert(address_list);
    ck_assert(!address_list -> next_addr);
    ck_assert(!address_list -> prev_addr);
    ck_assert_int_eq(address_list->size, SBRK_SIZE - 512 - TOTAL_METADATA_SIZE);
    ck_ptr((uint8_t *)address_list, my_sbrk_fake_heap);
}
END_TEST

/*Calloc Zero*/
void* reti;
void setup_calloc_zero(void) {
    init_malloc_test(0, false);
    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    reti = my_calloc(0, 69);
}

//test return value
START_TEST(test_calloc_zero_retvalue) {
    call_freelist("actual/Calloc_Zero.txt");
    ck_assert(!reti);
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

/*Calloc Clobbered*/
void setup_calloc_clobber_errno(void) {
    init_malloc_test(0, false);

    // Set errno to something else to check if student is setting it
    my_malloc_errno = OUT_OF_MEMORY;
    reti = my_calloc(SBRK_SIZE, 1);
}

//checking return value
START_TEST(test_calloc_clobber_errno_retvalue) {
    call_freelist("actual/Calloc_Clobber.txt");
    ck_assert(!reti);
    ck_assert_int_eq(my_malloc_errno, SINGLE_REQUEST_TOO_LARGE);
}
END_TEST

/*Calloc Zeroed Out*/
void calloc_zeroed_setup(void) {
    init_malloc_test(1, false);

    // Put a bunch of [feces]
    memset(my_sbrk_fake_heap, 0x69, SBRK_SIZE);

    create_situation_1(&A, &B, &C);

    my_malloc_errno = OUT_OF_MEMORY;
    ret = my_calloc(1,607);
}

//check retvalue correct
START_TEST(test_calloc_zeroed_retvalue) {
    call_freelist("actual/Calloc_IsItZero.txt");
    ck_assert(ret);
    //metadata_t* meta = (metadata)ret;
    uint8_t *split_right = my_sbrk_fake_heap + SBRK_SIZE - 607 - TOTAL_METADATA_SIZE + sizeof(metadata_t);
    ck_ptr(ret, split_right);

    // Check that memory is zeroed out
    for (unsigned int i = 0; i < 607; i++) {
        ck_assert(!ret[i]);
    }

    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
}
END_TEST

//check remainder of block in freelist is correct
START_TEST(test_calloc_zeroed_splitblock) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //make sure the address is correct
    uint8_t *split_left = (uint8_t*)my_sbrk_fake_heap + 1216 + 2*TOTAL_METADATA_SIZE;
    ck_ptr(C, split_left);
    //check to see if the remainder is correct in the list
    unsigned long c_size = SBRK_SIZE - (1216 + 2*TOTAL_METADATA_SIZE) - (607 + TOTAL_METADATA_SIZE);
    ck_assert_int_eq(C -> size, c_size);
    //the pointers of this block will be checked in the freelist section so no need to duplicate that check here
}
END_TEST

//check metadata of block returned correct
START_TEST(test_calloc_zeroed_metadata) {
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
START_TEST(test_calloc_zeroed_lists) {
    ck_assert_int_eq(my_malloc_errno, NO_ERROR);
    //checks address_list front to back
    ck_ptr(A, address_list);
    ck_ptr(A -> next_addr, B);
    ck_ptr(B -> next_addr, C);
    ck_assert(!C -> next_addr);
    //checks address_list back to front
    ck_ptr(C -> prev_addr, B);
    ck_ptr(B -> prev_addr, A);
    ck_assert(!A -> prev_addr);

    //checks size list front to back
    ck_ptr(C, size_list);
    ck_ptr(C -> next_size, B);
    ck_ptr(B -> next_size, A);
    ck_assert(!A -> next_size);
    //checks size list back to front
    ck_ptr(A -> prev_size, B);
    ck_ptr(B -> prev_size, C);
    ck_assert(!C -> prev_size);
}
END_TEST


Suite *calloc_suite(void) {
    Suite *s = suite_create("Calloc");

    
    // calloc() tests
    
    TCase *calloc1 = tcase_create("Calloc_Init");
    tcase_add_checked_fixture(calloc1, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(calloc1, setup_calloc_initial, NULL);
    tcase_add_test(calloc1, test_calloc_initial_lists);
    tcase_add_test(calloc1, test_calloc_initial_sbrkcall);
    tcase_add_test(calloc1, test_calloc_initial_retvalue);
    tcase_add_test(calloc1, test_calloc_initial_metadata);
    suite_add_tcase(s, calloc1);

    TCase *calloc2 = tcase_create("Calloc_Zero");
    tcase_add_checked_fixture(calloc2, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(calloc2, setup_calloc_zero, NULL);
    tcase_add_test(calloc2, test_calloc_zero_retvalue);
    suite_add_tcase(s, calloc2);

    TCase *calloc3 = tcase_create("Calloc_Clobber");
    tcase_add_checked_fixture(calloc3, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(calloc3, setup_calloc_clobber_errno, NULL);
    tcase_add_test(calloc3, test_calloc_clobber_errno_retvalue);
    suite_add_tcase(s, calloc3);

    // copy-paste of Malloc_Split_Block3
    TCase *calloc4 = tcase_create("Calloc_IsItZero");
    tcase_add_checked_fixture(calloc4, setup_malloc_malloc, teardown_malloc_malloc);
    tcase_add_checked_fixture(calloc4, calloc_zeroed_setup, NULL);
    tcase_add_test(calloc4, test_calloc_zeroed_lists);
    tcase_add_test(calloc4, test_calloc_zeroed_metadata);
    tcase_add_test(calloc4, test_calloc_zeroed_retvalue);
    tcase_add_test(calloc4, test_calloc_zeroed_splitblock);
    suite_add_tcase(s, calloc4);

    return s;

}
