#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PI 3.14159265358979323846

typedef uint8_t byte_t;


#define mprintf(...) {fprintf(stdout, __VA_ARGS__); fflush(stdout);}


static inline void fastmemcpy(void* dst, const void* src, size_t size) {
    uint64_t* _dst8 = (uint64_t*) dst;
    uint64_t* _src8 = (uint64_t*) src;
    size_t i = size >> 3;
    while (i--) _dst8[i] = _src8[i];
    uint8_t* _dst1 = (uint8_t*) dst;
    uint8_t* _src1 = (uint8_t*) src;
    for (i = size & 0xFFFFFFFFFFFFFFF8; i < size; i++) _dst1[i] = _src1[i];
}



// Min and max functions

static inline int8_t  min_i8 (int8_t  a, int8_t  b) {return a < b ? a : b;}
static inline int8_t  max_i8 (int8_t  a, int8_t  b) {return a > b ? a : b;}

static inline int16_t min_i16(int16_t a, int16_t b) {return a < b ? a : b;}
static inline int16_t max_i16(int16_t a, int16_t b) {return a > b ? a : b;}

static inline int32_t min_i32(int32_t a, int32_t b) {return a < b ? a : b;}
static inline int32_t max_i32(int32_t a, int32_t b) {return a > b ? a : b;}

static inline int64_t min_i64(int64_t a, int64_t b) {return a < b ? a : b;}
static inline int64_t max_i64(int64_t a, int64_t b) {return a > b ? a : b;}


static inline uint8_t  min_u8 (uint8_t  a, uint8_t  b) {return a < b ? a : b;}
static inline uint8_t  max_u8 (uint8_t  a, uint8_t  b) {return a > b ? a : b;}

static inline uint16_t min_u6(uint16_t a, uint16_t b) {return a < b ? a : b;}
static inline uint16_t max_u6(uint16_t a, uint16_t b) {return a > b ? a : b;}

static inline uint32_t min_u32(uint32_t a, uint32_t b) {return a < b ? a : b;}
static inline uint32_t max_u32(uint32_t a, uint32_t b) {return a > b ? a : b;}

static inline uint64_t min_u64(uint64_t a, uint64_t b) {return a < b ? a : b;}
static inline uint64_t max_u64(uint64_t a, uint64_t b) {return a > b ? a : b;}

static inline float min_f(float a, float b) {return a < b ? a : b;}
static inline float max_f(float a, float b) {return a > b ? a : b;}



// Abs functions

static inline int8_t  abs_i8 (int8_t  a) {return a < 0 ? -a : a;}
static inline int16_t abs_i16(int16_t a) {return a < 0 ? -a : a;}
static inline int32_t abs_i32(int32_t a) {return a < 0 ? -a : a;}
static inline int64_t abs_i64(int64_t a) {return a < 0 ? -a : a;}
static inline float   abs_f  (float   a) {return a < 0 ? -a : a;}
static inline double  abs_d  (double  a) {return a < 0 ? -a : a;}

