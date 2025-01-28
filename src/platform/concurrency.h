#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <immintrin.h>
#endif

// Acquire lock, may block
#define ACQUIRE(lock) while (_InterlockedCompareExchange8(&lock, 1, 0)) _mm_pause()

// Try to acquire lock and return if successful (without blocking)
#define TRY_ACQUIRE(lock) (_InterlockedCompareExchange8(&lock, 1, 0) == 0)

// Release lock
#define RELEASE(lock) lock = 0
