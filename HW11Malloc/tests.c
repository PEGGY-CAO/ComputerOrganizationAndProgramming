#include <check.h>
#include <stdio.h>
#include "suites/suites.h"

void print_usage(char *progname) {
    fprintf(stderr, "usage: %s [testcase]\n", progname);
}

int main(int argc, char **argv) {
    char *testcase = NULL;

    if (argc-1 > 1) {
        print_usage(argv[0]);
        return 1;
    } else {
        if (argc-1 >= 1) {
            testcase = argv[1];
        }
    }

    Suite *malloc_s = malloc_suite();
    Suite *free_s = free_suite();
    Suite *calloc_s = calloc_suite();
    Suite *realloc_s = realloc_suite();

    if (testcase &&
        !suite_tcase(malloc_s, testcase) &&
        !suite_tcase(free_s, testcase) &&
		!suite_tcase(calloc_s, testcase) &&
        !suite_tcase(realloc_s, testcase)) {
        print_usage(argv[0]);
        fprintf(stderr, "\n%s: error: `%s' is not a test case\n", argv[0], testcase);
        return 2;
    }

    SRunner *sr = srunner_create(malloc_s);
    srunner_add_suite(sr, free_s);
    srunner_add_suite(sr, calloc_s);
    srunner_add_suite(sr, realloc_s);
    srunner_run(sr, NULL, testcase, CK_NORMAL);
    srunner_free(sr);
    return 0;
}
