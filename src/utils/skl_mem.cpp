#include "skl_mem.h"

void skl_memset(void* _data, uint8_t per_byte_val, uint32_t num_bytes) {
    uint8_t* data = static_cast<uint8_t*>(_data);
    for (uint32_t i = 0; i < num_bytes; i++) {
        data[i] = per_byte_val;
    }
}