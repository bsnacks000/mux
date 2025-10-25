/**
 * @version 0.1.0
 * @brief wavio - libsndfile wrapper for block reading and writing.
 *
 *
 *
 *
 *
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
#include <stddef.h>

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
                     int chans,
                     int format,
                     uint32_t n_frames);

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
                    uint32_t n_frames);

/**
 * @brief fill wavio with one block of interleaved audio.
 * NOTE: caller must guarentee in_sz == self->block_sz;
 */
void wavio_fill_block(wavio* self, const float* in);

/**
 * @brief copy one block (nframes) of interleaved audio into out.
 * NOTE: caller must guarentee out_sz == self->block_sz;
 */
void wavio_copy_block(wavio* self, float* out);

/**
 * @brief write the current block.
 */
sf_count_t wavio_write_block(wavio* self);

/**
 * @brief read the next block. If sf_count_t returns 0 we have reached EOF. The
 * second to last read may return a partial block. Depending on the application may
 * have to pad zeros (or should have already!) on a partial.
 */
sf_count_t wavio_read_block(wavio* self);

/**
 * @brief cleanup the wavio object using dealloc. Fails fast on sf_close error.
 */
int wavio_close(wavio* self, void (*dealloc)(void*));

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
