/* 
    Tracked memory allocator
    TODO: Tagging
*/

#include "util/logging.hpp"

#include <cmdline/cmdline.hpp>

std::size_t sysTotalRam;
std::size_t sysNumAllocs;

void* operator new(std::size_t size)
{
    sysNumAllocs++;
    sysTotalRam += size;

    void* ptr = malloc(size);

    if (!ptr)
        throw std::bad_alloc();
    
#ifdef DEBUG
    if (Brewery::Cmdline_Check("-memdebug"))
        Brewery::Logging_LogChannel("operator new: Allocated %d bytes (total is now %d)", Brewery::LogChannel::Debug, size, sysTotalRam);
#endif
    return ptr;
}

void* operator new[](std::size_t size)
{
    sysNumAllocs++;
    sysTotalRam += size;

    void* ptr = malloc(size);

    if (!ptr)
        throw std::bad_alloc();
    
#ifdef DEBUG
    if (Brewery::Cmdline_Check("-memdebug"))
        Brewery::Logging_LogChannel("operator new[]: Allocated %d bytes (total is now %d)", Brewery::LogChannel::Debug, size, sysTotalRam);
#endif
    return ptr;
}

void operator delete(void *p) _GLIBCXX_TXN_SAFE _GLIBCXX_USE_NOEXCEPT
{
    if (!p)
        throw std::bad_alloc();

    if (!sysNumAllocs)
        throw std::bad_alloc();

    sysNumAllocs--;
    sysTotalRam -= sizeof(p);
#ifdef DEBUG

    if (Brewery::Cmdline_Check("-memdebug"))
        Brewery::Logging_LogChannel("operator delete: Freed %d bytes (total is now %d)", Brewery::LogChannel::Debug, sizeof(p), sysTotalRam);
#endif

    free(p);
}

void operator delete[](void *p) _GLIBCXX_TXN_SAFE _GLIBCXX_USE_NOEXCEPT
{
    if (!p)
        throw std::bad_alloc();

    if (!sysNumAllocs)
        throw std::bad_alloc();

    sysNumAllocs--;
    sysTotalRam -= sizeof(p);
#ifdef DEBUG
    if (Brewery::Cmdline_Check("-memdebug"))
        Brewery::Logging_LogChannel("operator delete[]: Freed %d bytes (total is now %d)", Brewery::LogChannel::Debug, sizeof(p), sysTotalRam);
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
