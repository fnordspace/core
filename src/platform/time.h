#pragma once

#include "global_var.h"
#include <lib/platform_efi/uefi.h>
#include "memory.h"
#include <stddef.h>


GLOBAL_VAR_DECL EFI_TIME utcTime;


#ifdef NO_UEFI

// Defined in test/stdlib_impl.cpp
void updateTime();

#else

#include <lib/platform_efi/uefi_globals.h>
#include <lib/platform_common/processor.h>

static void updateTime()
{
    ASSERT(isMainProcessor());
    EFI_TIME newTime;
    if (!rs->GetTime(&newTime, NULL))
    {
        copyMem(&utcTime, &newTime, sizeof(utcTime));
    }
}

#endif

[[maybe_unused]]
static void initTime()
{
    setMem(&utcTime, sizeof(utcTime), 0);
    utcTime.Year = 2022;
    utcTime.Month = 4;
    utcTime.Day = 13;
    utcTime.Hour = 12;

    updateTime();
}

// Calculates an index representing the day of the week for a given date with 0 = Wednesday
inline int dayIndex(unsigned int year, unsigned int month, unsigned int day) 
{
    month = (month + 9) % 12;

    year += (2000 - month / 10);

    int result = year * 365;
    result += year / 4;
    result -= year / 100;
    result += year / 400;
    result += (month * 306 + 5) / 10;
    result += day - 1;

    return result;
}

inline long long ms(unsigned char year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second, unsigned short millisecond)
{
    return (((((long long)dayIndex(year, month, day)) * 24 + hour) * 60 + minute) * 60 + second) * 1000 + millisecond;
}

#ifndef NO_UEFI
inline unsigned long long now_ms()
{
    // utcTime is updated on main thread - because only main thread can do it
    return ms(static_cast<unsigned char>(utcTime.Year % 100), utcTime.Month, utcTime.Day, utcTime.Hour, utcTime.Minute, utcTime.Second, utcTime.Nanosecond / 1000000);
}
#else
unsigned long long now_ms();
#endif
