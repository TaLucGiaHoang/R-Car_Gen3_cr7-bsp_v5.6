
#include "target/trampoline/r_osal_common.h"
#include "r_os_cache.h"

int32_t R_OSAL_CacheFlush(size_t Start, size_t Size, size_t Cache)
{
    if (Cache == R_OSAL_CACHE_I) {
        /* Flushing an instruction cache is nonsensical */
    } else if (Cache == R_OSAL_CACHE_D) {
        R_OS_Cache_FlushData(Start, Size);
    } else if (Cache == R_OSAL_CACHE_ALL) {
        R_OS_Cache_FlushData(Start, Size);
    } else {
        return -1;
    }
    return 0;
}

int32_t R_OSAL_CacheInvalidate(size_t Start, size_t Size, size_t Cache)
{
    if (Cache == R_OSAL_CACHE_I) {
        R_OS_Cache_InvalidateInstruction(Start, Size);
    } else if (Cache == R_OSAL_CACHE_D) {
        R_OS_Cache_InvalidateData(Start, Size);
    } else if (Cache == R_OSAL_CACHE_ALL) {
        R_OS_Cache_InvalidateInstruction(Start, Size);
        R_OS_Cache_InvalidateData(Start, Size);
    } else {
        return -1;
    }
    return 0;
}
