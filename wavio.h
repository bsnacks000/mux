/**
 * @version 0.1.0
 * @brief wavio - libsndfile wrapper for block reading and writing.
 *  * mux/demux
 *
 *  * Supports converting between the following f32 memory layouts:
 *      - interleaved (frame major)
 *      - planar (channel major)
 *  * Conversions to/from 1D contiguous array or float** multi channels.
 *
 *  * Makes the following assumptions:
 *       1. Each channel has the same number of frames (samples).
 *       2. The interleaved buffer length equals n_chans * n_frames.
 *       3. The size of the interleaved buffer is evenly divisible by n_chans.
 *       4. Input and output buffers do not overlap or alias each other.
 */

#ifndef WAVIO_H
#define WAVIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sndfile.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// type mask - determine WAV/AIFF
// subtype - determine the byte format
#define SF_FORMAT_TYPEMASK 0x0FFF0000
#define SF_FORMAT_SUBMASK 0x0000FFFF

static inline bool is_supported_write_format(int format) {
    int major = format & SF_FORMAT_TYPEMASK;
    int subtype = format & SF_FORMAT_SUBMASK;

    if (major != SF_FORMAT_WAV && major != SF_FORMAT_AIFF)
        return false;

    if (subtype != SF_FORMAT_PCM_16 && subtype != SF_FORMAT_PCM_24 &&
        subtype != SF_FORMAT_FLOAT)
        return false;

    return true;
}
/**
 * @brief hold sndfile handle and info.
 */
typedef struct {
    SNDFILE* handle;
    SF_INFO info;
    uint32_t n_frames;      // this is the kblock size
    float* block;           // interleaved block
    uint32_t block_sz;      // interleaved block sz (kvec_sz * nchans)
    uint32_t block_nbytes;  // sizeof (float) * blk_sz
} wavio;

/**
 * @brief open sf for write.
 * NOTE: allocates memory for wavio struct via allocator.
 *  - return 0 on success.
 *  - return SF_ERR_UNRECOGNISED_FORMAT if AIFF/WAV i16, i24 f32 are not specified.
 *  - return SF_ERR_SYSTEM if alloc fails.
 */

int wavio_open_write(wavio* self,
                     void* (*alloc)(size_t nbytes),
                     const char* path,
                     int sr,
                     int nchns,
                     int format,
                     uint32_t n_frames) {

    memset(self, 0, sizeof(*self));
    memset(&self->info, 0, sizeof(SF_INFO));

    // check supported chan count
    if (nchns < 1 || nchns > 2) {
        fprintf(stderr, "unsupported channel count: %d\n", nchns);
        return SF_ERR_UNRECOGNISED_FORMAT;
    }

    // check supported formats
    if (!is_supported_write_format(format)) {
        fprintf(stderr,
                "wavio_open_write: unsupported format (must be WAV or AIFF / i16, "
                "i24,f32)\n");
        return SF_ERR_UNRECOGNISED_FORMAT;
    }

    // set the struct
    self->info.samplerate = sr;
    self->info.channels = nchns;
    self->info.format = format;
    self->n_frames = n_frames;
    self->block_sz = n_frames * nchns;
    self->block_nbytes = sizeof(float) * self->block_sz;

    // alloc or fail
    self->block = (float*) alloc(self->block_nbytes);
    if (!self->block) {
        fprintf(stderr, "wavio_open_write: alloc failed\n");
        return SF_ERR_SYSTEM;
    }

    // open the file for write or err
    self->handle = sf_open(path, SFM_WRITE, &self->info);
    if (!self->handle) {
        fprintf(stderr, "wavio_open_write: %s\n", sf_strerror(NULL));
        return SF_ERR_SYSTEM;
    }
    return SF_ERR_NO_ERROR;  // SF_ERR_NO_ERROR	0
}

/**
 * @brief open sf for read. Supports wav and aiff PCM/float formats. Only works with
 * mono or stereo files.
 * NOTE: allocates memory for wavio struct via allocator.
 *  - return 0 on success.
 *  - return SF_SYSTEM_ERR if open_read fails.
 *  - Fails fast if alloc error.
 */
int wavio_open_read(wavio* self,
                    void* (*alloc)(size_t nbytes),
                    const char* path,
                    uint32_t nframes) {

    memset(self, 0, sizeof(*self));
    memset(&self->info, 0, sizeof(SF_INFO));

    // open read or err
    if ((self->handle = sf_open(path, SFM_READ, &self->info)) == NULL) {
        fprintf(stderr, "wavio_open_read: %s\n", sf_strerror(NULL));
        return SF_ERR_SYSTEM;
    }

    self->n_frames = nframes;
    self->block_sz = nframes * self->info.channels;
    self->block_nbytes = sizeof(float) * self->block_sz;

    self->block = (float*) alloc(self->block_nbytes);
    // alloc or err
    if (!self->block) {
        fprintf(stderr, "wavio_open_read: alloc failed\n");
        sf_close(self->handle);
        self->handle = NULL;
        return SF_ERR_SYSTEM;
    }

    return SF_ERR_NO_ERROR;  // SF_ERR_NO_ERROR	0
} /**
   * @brief fill wavio with one block of interleaved audio.
   * NOTE: caller must guarentee in_sz == self->block_sz;
   */
void wavio_fill_block(wavio* self, const float* in) {
    memcpy(self->block, in, self->block_nbytes);
}

/**
 * @brief copy one block (nframes) of interleaved audio into out.
 * NOTE: caller must guarentee out_sz == self->block_sz;
 */
void wavio_copy_block(wavio* self, float* out) {
    memcpy(out, self->block, self->block_nbytes);
}

/**
 * @brief write the current block.
 */
sf_count_t wavio_write_block(wavio* self) {
    return sf_writef_float(self->handle, self->block, self->n_frames);
}

/**
 * @brief read the next block. If sf_count_t returns 0 we have reached EOF.
 */
sf_count_t wavio_read_block(wavio* self) {
    memset(self->block, 0, self->block_nbytes);
    return sf_readf_float(self->handle, self->block, self->n_frames);
}

/**
 * @brief cleanup the wavio object using dealloc. Fails fast on sf_close error.
 */
int wavio_close(wavio* self, void (*dealloc)(void*)) {
    if (self->block) {
        dealloc(self->block);
        self->block = NULL;
    }
    int err = 0;
    if (self->handle) {
        err = sf_close(self->handle);
        self->handle = NULL;
        if (err != 0)
            fprintf(stderr, "sf_close failed: error code %d\n", err);
    }

    return err;
}

/**
 * @brief mux N chans into 1 interleaved signal.
 */
static inline void wavio_mux(float* interleaved,
                             const float** channels,
                             size_t n_chans,
                             size_t n_frames) {
    for (size_t i = 0; i < n_frames; i++) {
        for (size_t j = 0; j < n_chans; j++) {
            interleaved[i * n_chans + j] = channels[j][i];
        }
    }
}

/**
 * @brief demux an interleaved signal into multiple channels.
 */
static inline void wavio_demux(float** channels,
                               const float* interleaved,
                               size_t n_chans,
                               size_t n_frames) {
    for (size_t i = 0; i < n_frames; i++) {
        for (size_t j = 0; j < n_chans; j++) {
            channels[j][i] = interleaved[i * n_chans + j];
        }
    }
}

/**
 * @brief mux n_chans in a 1d array in row major order into an interleaved buffer
 */
static inline void wavio_mux1d(float* interleaved,
                               const float* channels,
                               size_t n_chans,
                               size_t len_chans) {
    for (size_t i = 0; i < len_chans; i++) {
        for (size_t j = 0; j < n_chans; j++) {
            interleaved[i * n_chans + j] = channels[j * len_chans + i];
        }
    }
}

/**
 * @brief demux n_chans from an interleaved buffer into a 1d array in row major order.
 */
static inline void wavio_demux1d(float* channels,
                                 const float* interleaved,
                                 size_t n_chans,
                                 size_t len_chans) {
    for (size_t i = 0; i < len_chans; i++) {
        for (size_t j = 0; j < n_chans; j++) {
            channels[j * len_chans + i] = interleaved[i * n_chans + j];
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
