/**
 * @version 0.1.0
 * @brief mux.h - muxing and demuxing utils tuned for audio I/O.
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

#ifndef AUD_MUX_H
#define AUD_MUX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief mux N chans into 1 interleaved signal.
 */
static inline void aud_mux(float* interleaved,
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
static inline void aud_demux(float** channels,
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
static inline void aud_mux1d(float* interleaved,
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
static inline void aud_demux1d(float* channels,
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
