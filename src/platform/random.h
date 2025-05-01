#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <immintrin.h>
#endif

inline static unsigned int random(const unsigned int range)
{
    unsigned int value;
    _rdrand32_step(&value);

    return value % range;
}

inline static void random64(unsigned long long* dst)
{
    ASSERT(dst != NULL);
    _rdrand64_step(dst);
}

inline static void random256(m256i* dst)
{
    ASSERT(dst != NULL);
    dst->setRandomValue();
}