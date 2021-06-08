/*
 *   _|                                      _|_|_|_|            _|
 *   _|          _|_|    _|      _|      _|  _|        _|_|_|  _|_|_|_|
 *   _|        _|    _|  _|      _|      _|  _|_|_|  _|    _|    _|
 *   _|        _|    _|    _|  _|  _|  _|    _|      _|    _|    _|
 *   _|_|_|_|    _|_|        _|      _|      _|        _|_|_|      _|_|
 * 
 * Gregory J. Duck.
 *
 * Copyright (c) 2017 The National University of Singapore.
 * All rights reserved.
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See the LICENSE file for details.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __LOWFAT_PTR_INFO_HH
#define __LOWFAT_PTR_INFO_HH

#define _LOWFAT_SIZES                       ((size_t *)0x200000)
#define _LOWFAT_MAGICS                      ((uint64_t *)0x300000)
#define _LOWFAT_REGION_SIZE                 68719476736ull
#define LOWFAT_REGION_SIZE                  _LOWFAT_REGION_SIZE
#define EFFECTIVE_LOWFAT_NUM_REGIONS_LIMIT  127
#define LOWFAT_NUM_REGIONS                  62
#define EFFECTIVE_LIKELY(x)         __builtin_expect((x), 1)
#define EFFECTIVE_UNLIKELY(x)       __builtin_expect((x), 0)

#define EFFECTIVE_CONST     __attribute__((__const__))
#define EFFECTIVE_PURE              __attribute__((__pure__))

//#define LOWFAT_SIZES_PAGES 1
//#define LOWFAT_REGION_SIZE_SHIFT 36
#define LOWFAT_STACK_MEMORY_SIZE 17179869184
#define LOWFAT_GLOBAL_MEMORY_SIZE 17179869184
#define LOWFAT_HEAP_MEMORY_SIZE 34359672832
#define LOWFAT_STACK_MEMORY_OFFSET 51539607552
#define LOWFAT_GLOBAL_MEMORY_OFFSET 34359701504
#define LOWFAT_HEAP_MEMORY_OFFSET 0
//#define LOWFAT_STACK_SIZE 67108864
//#define LOWFAT_PAGE_SIZE 4096
//#define LOWFAT_HEAP_ASLR_MASK 0xFFFFFFFFull
//#define LOWFAT_MAX_HEAP_ALLOC_SIZE 17179869184
// #define LOWFAT_TID_OFFSET 0x2d0
// #define LOWFAT_JOINID_OFFSET 0x628
// #define LOWFAT_NO_MEMORY_ALIAS 1
// #define LOWFAT_NO_PROTECT 1
// #define LOWFAT_NO_REPLACE_STD_MALLOC 1
// #define LOWFAT_NO_REPLACE_STD_FREE 1
// #define LOWFAT_MAX_STACK_ALLOC_SIZE 33554432
// #define LOWFAT_MAX_GLOBAL_ALLOC_SIZE 67108864
// #define LOWFAT_MIN_ALLOC_SIZE 16
// #define LOWFAT_NUM_STACK_REGIONS 53
// #define LOWFAT_STACK_REGION 63


size_t lowfat_sizes[] =
{
	16, /* idx=0 */
	32, /* idx=1 */
	48, /* idx=2 */
	64, /* idx=3 */
	80, /* idx=4 */
	96, /* idx=5 */
	112, /* idx=6 */
	128, /* idx=7 */
	144, /* idx=8 */
	160, /* idx=9 */
	192, /* idx=10 */
	224, /* idx=11 */
	256, /* idx=12 */
	272, /* idx=13 */
	320, /* idx=14 */
	384, /* idx=15 */
	448, /* idx=16 */
	512, /* idx=17 */
	528, /* idx=18 */
	640, /* idx=19 */
	768, /* idx=20 */
	896, /* idx=21 */
	1024, /* idx=22 */
	1040, /* idx=23 */
	1280, /* idx=24 */
	1536, /* idx=25 */
	1792, /* idx=26 */
	2048, /* idx=27 */
	2064, /* idx=28 */
	2560, /* idx=29 */
	3072, /* idx=30 */
	3584, /* idx=31 */
	4096, /* idx=32 */
	4112, /* idx=33 */
	5120, /* idx=34 */
	6144, /* idx=35 */
	7168, /* idx=36 */
	8192, /* idx=37 */
	8208, /* idx=38 */
	10240, /* idx=39 */
	12288, /* idx=40 */
	16384, /* idx=41 */
	32768, /* idx=42 */
	65536, /* idx=43 */
	131072, /* idx=44 */
	262144, /* idx=45 */
	524288, /* idx=46 */
	1048576, /* idx=47 */
	2097152, /* idx=48 */
	4194304, /* idx=49 */
	8388608, /* idx=50 */
	16777216, /* idx=51 */
	33554432, /* idx=52 */
	67108864, /* idx=53 */
	134217728, /* idx=54 */
	268435456, /* idx=55 */
	536870912, /* idx=56 */
	1073741824, /* idx=57 */
	2147483648, /* idx=58 */
	4294967296, /* idx=59 */
	8589934592, /* idx=60 */
	17179869184, /* idx=61 */
};


/*
 * Slow definitions that do not depend on the LowFat runtime system:
 */

size_t lowfat_index(const void *ptr)
{
    return (uintptr_t)ptr / _LOWFAT_REGION_SIZE;
}

void *lowfat_region(const void *ptr)
{
    return (void *)(lowfat_index(ptr) * LOWFAT_REGION_SIZE);
}


bool lowfat_is_ptr(const void *ptr)
{
    size_t idx = lowfat_index(ptr);
    return (idx != 0 && idx <= LOWFAT_NUM_REGIONS+1);
}

bool lowfat_is_heap_ptr(const void *ptr)
{
    if (!lowfat_is_ptr(ptr))
        return false;
    void *heap_lo = lowfat_region(ptr) + LOWFAT_HEAP_MEMORY_OFFSET;
    void *heap_hi = heap_lo + LOWFAT_HEAP_MEMORY_SIZE;
    return (ptr >= heap_lo && ptr < heap_hi);
}

bool lowfat_is_stack_ptr(const void *ptr)
{
    if (!lowfat_is_ptr(ptr))
        return false;
    void *stack_lo = lowfat_region(ptr) + LOWFAT_STACK_MEMORY_OFFSET;
    void *stack_hi = stack_lo + LOWFAT_STACK_MEMORY_SIZE;
    return (ptr >= stack_lo && ptr < stack_hi);
}

bool lowfat_is_global_ptr(const void *ptr)
{
    if (!lowfat_is_ptr(ptr))
        return false;
    void *global_lo = lowfat_region(ptr) + LOWFAT_GLOBAL_MEMORY_OFFSET;
    void *global_hi = global_lo + LOWFAT_GLOBAL_MEMORY_SIZE;
    return (ptr >= global_lo && ptr < global_hi);
}

size_t lowfat_size(const void *ptr)
{
    if (!lowfat_is_ptr(ptr))
        return SIZE_MAX;
    else
        return lowfat_sizes[lowfat_index(ptr)-1];
}

// static size_t lowfat_magic(const void *ptr)
// {
//     size_t idx = lowfat_index(ptr);
//     if (!lowfat_is_ptr(ptr))
//         return 0;
//     else
//         return lowfat_magics[lowfat_index(ptr)-1];
// }

void *lowfat_base(const void *ptr)
{
    uintptr_t iptr = (uintptr_t)ptr;
    size_t size = lowfat_size(ptr);
    if (size != 0)
        iptr -= iptr % size;
    return (void *)iptr;
}

/*
 * Get the base address of a pointer.
 */
static EFFECTIVE_CONST const void *effective_baseof(const void *ptr)
{
    size_t idx = lowfat_index(ptr);
    if (idx > 1024 || _LOWFAT_MAGICS[idx] == 0)     // XXX: 1024 magic
        return NULL;
    return lowfat_base(ptr);
}

/*
 * Get the effective type of a pointer.
 */
static EFFECTIVE_PURE const EFFECTIVE_TYPE *effective_typeof(const void *ptr)
{
    const void *base = effective_baseof(ptr);
    if (base == NULL)
        return NULL;
    const EFFECTIVE_META *meta = (const EFFECTIVE_META *)base;
    return meta->type;
}

#endif	
