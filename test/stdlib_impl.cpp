// Implements NO_UEFI versions of several functions that require cstdlib functions.
// Having them in a separate cpp file avoids the name clash between cstdlib's system and Qubic's system.

#include <cstring>
#include <cstdlib>
#include <ctime>
#include "../lib/platform_common/compiler_warnings.h"

#define NO_UEFI

#include "platform/time.h"

EFI_TIME utcTime;

void setMem(void* buffer, unsigned long long size, unsigned char value)
{
    memset(buffer, value, size);
}

void copyMem(void* destination, const void* source, unsigned long long length)
{
    memcpy(destination, source, length);
}

bool allocatePool(unsigned long long size, void** buffer)
{
    void* ptr = malloc(size);
    if (ptr)
    {
        *buffer = ptr;
        return true;
    }
    return false;
}

void freePool(void* buffer)
{
    free(buffer);
}

SUPPRESS_WARNINGS_BEGIN
IGNORE_DEPRECATED_UNSAFE_FUNCTIONS_WARNING
void updateTime()
{
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::gmtime(&t);
    utcTime.Year = static_cast<unsigned short>(tm->tm_year + 1900);
    utcTime.Month = static_cast<unsigned char>(tm->tm_mon + 1);
    utcTime.Day = static_cast<unsigned char>(tm->tm_mday);
    utcTime.Hour = static_cast<unsigned char>(tm->tm_hour);
    utcTime.Minute = static_cast<unsigned char>(tm->tm_min);
    utcTime.Second = static_cast<unsigned char>(tm->tm_sec);
    utcTime.Nanosecond = 0;
    utcTime.TimeZone = 0;
    utcTime.Daylight = 0;
}

unsigned long long now_ms()
{
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::gmtime(&t);
    return ms(
        static_cast<unsigned char>(tm->tm_year % 100),
        static_cast<unsigned char>(tm->tm_mon + 1),
        static_cast<unsigned char>(tm->tm_mday),
        static_cast<unsigned char>(tm->tm_hour),
        static_cast<unsigned char>(tm->tm_min),
        static_cast<unsigned char>(tm->tm_sec),
        0
    );
}
SUPPRESS_WARNINGS_END
