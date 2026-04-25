/* 
    Tracked memory allocator
    TODO: Tagging

    NOTE: Logging is called from here. So don't use new/delete in logging
*/

#include "util/logging.hpp"
#include <core/tier0/memory/memory.hpp>

#include <core/cmdline/cmdline.hpp>

std::size_t sysTotalRam;
std::size_t sysNumAllocs;

void* operator new(std::size_t size)
{
    sysNumAllocs++;
    sysTotalRam += size;

    void* ptr = malloc(size);

    if (!ptr)
        Capy::Logging_LogChannel("**** Internal Problem **** Failed to allocate %ld bytes.", Capy::LogChannel::SuperFatal, size);
    
#ifdef DEBUG
    if (Capy::Cmdline_Check("-memdebug"))
        Capy::Logging_LogChannel("operator new: Allocated %ld bytes (total is now %ld)", Capy::LogChannel::Debug, size, sysTotalRam);
#endif
    return ptr;
}

void* operator new[](std::size_t size)
{
    sysNumAllocs++;
    sysTotalRam += size;

    void* ptr = malloc(size);

    if (!ptr)
        Capy::Logging_LogChannel("**** Internal Problem **** operator new[]: Failed to allocate %ld bytes.", Capy::LogChannel::SuperFatal, size);
    
#ifdef DEBUG
    if (Capy::Cmdline_Check("-memdebug"))
        Capy::Logging_LogChannel("operator new[]: Allocated %ld bytes (total is now %ld)", Capy::LogChannel::Debug, size, sysTotalRam);
#endif
    return ptr;
}

void operator delete(void *p) _GLIBCXX_TXN_SAFE _GLIBCXX_USE_NOEXCEPT
{
    if (!p)
        Capy::Logging_LogChannel("**** Internal Problem **** operator delete: Failed to free memory at 0x%x.", Capy::LogChannel::SuperFatal, p);

    if (!sysNumAllocs)
        Capy::Logging_LogChannel("**** Internal Problem **** operator delete: Tried to free 0x%x with no allocations!", Capy::LogChannel::SuperFatal, p);

    sysNumAllocs--;
    sysTotalRam -= sizeof(p);
#ifdef DEBUG
    if (Capy::Cmdline_Check("-memdebug"))
        Capy::Logging_LogChannel("operator delete: Freed %ld bytes (total is now %ld)", Capy::LogChannel::Debug, sizeof(p), sysTotalRam);
#endif

    free(p);
}

void operator delete[](void *p) _GLIBCXX_TXN_SAFE _GLIBCXX_USE_NOEXCEPT
{
    if (!p)
        Capy::Logging_LogChannel("**** Internal Problem **** operator delete[]: Failed to free memory at 0x%x.", Capy::LogChannel::SuperFatal, p);

    if (!sysNumAllocs)
        Capy::Logging_LogChannel("**** Internal Problem **** operator delete[]: Tried to free 0x%x with no allocations!", Capy::LogChannel::SuperFatal, p);

    sysNumAllocs--;
    sysTotalRam -= sizeof(p);
#ifdef DEBUG

    if (Capy::Cmdline_Check("-memdebug"))
        Capy::Logging_LogChannel("operator delete[]: Freed %ld bytes (total is now %ld)", Capy::LogChannel::Debug, sizeof(p), sysTotalRam);
#endif

    free(p);
}

std::size_t Memory_GetTotalRam()
{
    return sysTotalRam;
}

std::size_t Memory_GetNumAllocs()
{
    return sysNumAllocs;
}
