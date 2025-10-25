#include "munit.h"

#include <mux.h>

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

    MunitTest tests[] = {
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    };

    MunitSuite test_suite_main = {"/mux", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

    return munit_suite_main(&test_suite_main, NULL, argc, argv);
}
