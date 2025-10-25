#include "munit.h"

#include <stdio.h>
#include <wavio.h>

MunitResult test_wavio_mux(const MunitParameter params[], void* data) {
    (void) data;
    (void) params;

    float interleaved[12] = {0};
    float l[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    float r[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
    const float* channels[] = {l, r};

    wavio_mux(interleaved, channels, 2, 6);

    float expected[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0};

    munit_assert_memory_equal(sizeof(interleaved), interleaved, expected);
    return MUNIT_OK;
}

MunitResult test_wavio_demux(const MunitParameter params[], void* data) {
    (void) data;
    (void) params;

    const float interleaved[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0,
                                 3.0, 3.0, 4.0, 4.0, 5.0, 5.0};
    float l[6] = {0.0};
    float r[6] = {0.0};
    float* channels[] = {l, r};

    wavio_demux(channels, interleaved, 2, 6);

    float expected[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    munit_assert_memory_equal(sizeof(l), expected, channels[0]);
    munit_assert_memory_equal(sizeof(r), expected, channels[1]);

    return MUNIT_OK;
}

MunitResult test_wavio_mux1d(const MunitParameter params[], void* data) {
    (void) data;
    (void) params;

    float interleaved[12] = {0};
    float channels[] = {
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0,  // L
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0   // R
    };

    wavio_mux1d(interleaved, channels, 2, 6);

    float expected[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0};

    munit_assert_memory_equal(sizeof(expected), interleaved, expected);
    return MUNIT_OK;
}

MunitResult test_wavio_demux1d(const MunitParameter params[], void* data) {
    (void) data;
    (void) params;

    const float interleaved[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0,
                                 3.0, 3.0, 4.0, 4.0, 5.0, 5.0};
    float channels[12] = {0.0};

    wavio_demux1d(channels, interleaved, 2, 6);

    float expected[] = {
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0,  // L
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0   // R
    };

    munit_assert_memory_equal(sizeof(expected), channels, expected);
    return MUNIT_OK;
}

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

    MunitTest tests[] = {
        {"/wavio_mux", test_wavio_mux, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/wavio_demux", test_wavio_demux, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/wavio_mux1d", test_wavio_mux1d, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/wavio_demux1d", test_wavio_demux1d, NULL, NULL, MUNIT_TEST_OPTION_NONE,
         NULL},

        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    };

    MunitSuite test_suite_main = {"/mux", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

    return munit_suite_main(&test_suite_main, NULL, argc, argv);
}
