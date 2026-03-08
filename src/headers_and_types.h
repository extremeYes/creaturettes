////////////////////////////////
/// headers.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <raylib.h>
#include <raymath.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../nob.h"

////////////////
/// Types

#define bool  _Bool
#define true  1
#define false 0

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef size_t    usize; // Unsigned, pointer-sized
typedef ptrdiff_t isize; // Signed, pointer-sized

typedef float  f32;   // IEEE 754 Single Precision
typedef double f64;   // IEEE 754 Double Precision
