#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include <wavio.h>

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
}

void wavio_fill_block(wavio* self, const float* in) {
    memcpy(self->block, in, self->block_nbytes);
}

void wavio_copy_block(wavio* self, float* out) {
    memcpy(out, self->block, self->block_nbytes);
}

sf_count_t wavio_write_block(wavio* self) {
    return sf_writef_float(self->handle, self->block, self->n_frames);
}

sf_count_t wavio_read_block(wavio* self) {
    return sf_readf_float(self->handle, self->block, self->n_frames);
}

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
