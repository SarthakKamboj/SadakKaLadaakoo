#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_VIDEO_BLOCKS 10
#define MAX_METADATA_BLOCKS 10
#define MAX_MOVIE_TRACK_BLOCKS 10
#define MAX_COMPATIBLE_BRANDS 3

struct mp4_ctx_t {
    const char* filename;
    FILE* fptr;

    // ftyp info
    uint32_t major_brand = 0;
    uint32_t minor_brand = 0;
    uint32_t compatible_brands[MAX_COMPATIBLE_BRANDS]{};

    // mdata info
    int video_data_block_idx = 0;
    uint8_t* video_data_blocks[MAX_VIDEO_BLOCKS]{};

    // moov info
    int video_metadata_block_idx = 0;
    uint8_t* video_metadata_blocks[MAX_METADATA_BLOCKS]{};

    // int trak info
    int video_movie_track_block_idx = 0;
    uint8_t* movie_track_blocks[MAX_MOVIE_TRACK_BLOCKS]{};
};

uint8_t* load_contigous_data(mp4_ctx_t& ctx, uint32_t num_bytes);
uint8_t load_byte(mp4_ctx_t& ctx);
uint16_t load_2byte(mp4_ctx_t& ctx);
uint32_t load_4byte(mp4_ctx_t& ctx);
uint32_t chunk_type_to_4byte(const char* type);
bool parse_chunk(mp4_ctx_t& ctx);
bool parse_mp4(mp4_ctx_t& ctx);
bool load_mp4(const char* filename);
