#include "mp4.h"

#include <cmath>

#include "defines.h"
#include "defaults.h"
#include "utils/skl_fixed_pt.h"

uint8_t load_byte(mp4_ctx_t& ctx) {
  if (feof(ctx.fptr)) return 0;
  return static_cast<uint8_t>(fgetc(ctx.fptr));
}

uint16_t load_2byte(mp4_ctx_t& ctx) {
  uint16_t res = 0;
  for (int i = 0; i < 2; i++) {
    if (feof(ctx.fptr)) return 0;
    uint8_t b = load_byte(ctx);
    res = (res << 8) | b;
  }
  return res;
}

uint32_t load_4byte(mp4_ctx_t& ctx) {
  uint32_t res = 0;
  for (int i = 0; i < 4; i++) {
    if (feof(ctx.fptr)) {
      if (defaults_t::VIDEO_LOG) {
        SKL_LOG("EOF so returning 0");
      }
      return 0;
    }
    uint8_t b = load_byte(ctx);
    res = (res << 8) | b;
  }
  return res;
}

uint32_t chunk_type_to_4byte(const char* type) {
  uint32_t res = 0;
  for (int i = 0; i < 4; i++) {
    const char c = type[i];
    res = (res << 8) | static_cast<uint8_t>(c);
  }
  return res;
}

uint8_t* load_contigous_data(mp4_ctx_t& ctx, uint32_t num_bytes) {
  uint8_t* ptr = (uint8_t*)malloc(sizeof(uint8_t) * num_bytes);
  for (int i = 0; i < num_bytes; i++) {
    ptr[i] = load_byte(ctx);
  }
  return ptr;
}

bool parse_chunk(mp4_ctx_t& ctx) {

  uint32_t chunk_size = load_4byte(ctx);
  uint32_t chunk_type = load_4byte(ctx);
  
  if (chunk_type == chunk_type_to_4byte("ftyp")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading ftyp");
    }
    uint32_t expected_chunk_size = 0x0000001c;
    if (chunk_size != expected_chunk_size) {
      SKL_LOG("chunk size for ftyp should be %i but is %i", expected_chunk_size, chunk_size)
      return false;
    }
    ctx.major_brand = load_4byte(ctx);
    ctx.minor_brand = load_4byte(ctx);
    int num_compatible_brands_listed = (chunk_size - 16) / 4;
    for (int i = 0; i < fmin(MAX_COMPATIBLE_BRANDS, num_compatible_brands_listed); i++) {
      ctx.compatible_brands[i] = load_4byte(ctx);
    }
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded ftyp");
    }
  } else if (false && chunk_type == chunk_type_to_4byte("free")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading free");
    }
    uint32_t free_space = 0;
    for (int i = 0; i < chunk_size - 8; i++) {
      free_space *= 16;
      free_space += load_byte(ctx);
    }
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded free");
    }
  } else if (chunk_type == chunk_type_to_4byte("mdat")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading mdat");
    }
    uint32_t video_data_size = chunk_size - 8;
    if (video_data_size != 0) {
      uint8_t* video_data = load_contigous_data(ctx, video_data_size);
      ctx.video_data_blocks[ctx.video_data_block_idx] = video_data;
      ctx.video_data_block_idx += 1;
    }
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded mdat");
    }
  } else if (chunk_type == chunk_type_to_4byte("moov")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading moov");
    }
    uint32_t metadata_size = chunk_size - 8;
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("metadata_size: %u", metadata_size);
    }
    if (metadata_size != 0) {
      uint8_t* metadata = load_contigous_data(ctx, metadata_size);
    }
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded moov");
    }
  } else if (false && chunk_type == chunk_type_to_4byte("mvhd")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading mvhd");
    }
    uint32_t metadata_size = chunk_size - 8;
    uint8_t movie_hd_version = load_byte(ctx);
    uint8_t* future_movie_hd_flags = load_contigous_data(ctx, 3);
    uint32_t creation_time = load_4byte(ctx);
    uint32_t modification_time = load_4byte(ctx);
    uint32_t time_units_hz = load_4byte(ctx);
    uint32_t duration_time_units = load_4byte(ctx);

    // FIXME: must be interpreted as 32-bit fixed point
    uint32_t preferred_playback_rate_raw = load_4byte(ctx);
    skl_ufixed_pt<uint32_t> preferred_playback_rate(preferred_playback_rate_raw, 16);
    SKL_LOG("preferred playback rate: %f", preferred_playback_rate.get_val());

    // FIXME: must be interpreted as 16-bit fixed point
    uint16_t preferred_volume_raw = load_2byte(ctx);
    skl_ufixed_pt<uint16_t> preferred_volume(preferred_volume_raw, 8);
    SKL_LOG("preferred volume: %f", preferred_volume.get_val());

    // TODO: look at Matrix, predefines, and next track ID of mvhd in https://www.cimarronsystems.com/wp-content/uploads/2017/04/Elements-of-the-H.264-VideoAAC-Audio-MP4-Movie-v2_0.pdf

    if (metadata_size != 0) {
      uint8_t* metadata = load_contigous_data(ctx, metadata_size);
      ctx.video_metadata_blocks[ctx.video_metadata_block_idx] = metadata;
      ctx.video_metadata_block_idx += 1;
    }
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded mvhd");
    }
  } else if (false && chunk_size == chunk_type_to_4byte("trak")) {
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loading trak");
    }
    int movie_track_size = chunk_size - 8;
    ctx.movie_track_blocks[ctx.video_movie_track_block_idx] = load_contigous_data(ctx, movie_track_size);
    ctx.video_movie_track_block_idx += 1;
    if (defaults_t::VIDEO_LOG) {
      SKL_LOG("loaded trak");
    }
  } // TODO: conitnue at tkhd
  else {
    if (chunk_type == 0) return false;

    // pass over the remaining chunk size
    SKL_LOG("unrecognized chunk: %#08x => %c%c%c%c with chunk size: %i", 
      chunk_type, 
      (chunk_type >> 24) & 0xff,
      (chunk_type >> 16) & 0xff, 
      (chunk_type >> 8) & 0xff,
      chunk_type & 0xff,
      chunk_size - 8
    );
    if (chunk_size - 8 > 0) {
      uint8_t* data = load_contigous_data(ctx, chunk_size - 8);
      free(data);
    }
  }

  return true;
}

bool parse_mp4(mp4_ctx_t& ctx) {
  while (feof(ctx.fptr) == 0) {
    if (!parse_chunk(ctx)) {
      return false;
    }
  }
  return true;
}

bool load_mp4(const char* filename) {
  mp4_ctx_t ctx;
  ctx.filename = filename;
  if (defaults_t::VIDEO_LOG) {
    SKL_LOG("filename to load is %s", filename);
  }
  ctx.fptr = fopen(filename, "rb");
  if (!ctx.fptr) {
    SKL_LOG("could not open file %s", filename);
    return false;
  }
  parse_mp4(ctx);
  fclose(ctx.fptr);
  return true;
}
