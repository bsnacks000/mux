#include "munit.h"

#include <sndfile.h>
#include <wavio.h>

#include <stdlib.h>

#define NFRAMES 1024

MunitResult test_wavio_sndfile_interop(const MunitParameter params[], void* data) {
    (void) data;
    (void) params;

    // happy path .. open a wav file .. write some data .. read it back out ..
    float in[NFRAMES] = {0};
    float out[NFRAMES] = {0};

    for (int i = 0; i < NFRAMES; i++)
        in[i] = 1.0;

    wavio w;
    int err = wavio_open_write(&w, malloc, "test.wav", 48000, 1,
                               SF_FORMAT_WAV | SF_FORMAT_PCM_16, NFRAMES);
    munit_assert_int(err, ==, 0);

    wavio_fill_block(&w, in);
    sf_count_t count = wavio_write_block(&w);
    munit_assert_int(count, ==, NFRAMES);

    err = wavio_close(&w, free);
    munit_assert_int(err, ==, 0);

    memset(&w, 0, sizeof(wavio));
    err = wavio_open_read(&w, malloc, "test.wav", NFRAMES);
    munit_assert_int(err, ==, 0);

    count = wavio_read_block(&w);
    munit_assert_int(count, ==, NFRAMES);

    wavio_copy_block(&w, out);

    for (int i = 0; i < NFRAMES; i++)
        munit_assert_double_equal(out[i], 1.0, 1);

    return MUNIT_OK;
}

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
        {"/sndfile_interop", test_wavio_sndfile_interop, NULL, NULL,
         MUNIT_TEST_OPTION_NONE, NULL},
        {"/mux", test_wavio_mux, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/demux", test_wavio_demux, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/mux1d", test_wavio_mux1d, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/demux1d", test_wavio_demux1d, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    };

    MunitSuite test_suite_main = {"/wavio", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

    return munit_suite_main(&test_suite_main, NULL, argc, argv);
}
