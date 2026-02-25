#include <net/net.hpp>

namespace Capy
{
    // 
    // NetMsg string explicit specialisations (cannot be in header scope)
    // 

    template<>
    char* NetMsg::Read<char*>()
    {
        //should be ok as a temporary
        char* ch = (char*)&msgData[msgPtrRead];
        
        auto size = strlen(ch);

        if (EnsureCapacityRead(size + 1))
            msgPtrRead += size + 1; // + 1 for null terminator

        return ch; 
    }

    template<>
    const char* NetMsg::Read<const char*>()
    {
        return (const char*)Read<char*>();
    }

    // Write a string to a network message
    template<>
    void NetMsg::Write<char*>(char* thing)
    {
        auto size = strlen(thing);

        if (!EnsureCapacityWrite(size + 1)) // + 1 for null terminator
            return;

        strncpy((char*)msgData, thing, size);

        // add null-terminator
        msgData[size] = '\0';

        msgPtrWrite += size + 1;
    }

    template<>
    void NetMsg::Write<const char*>(const char* thing)
    {
        Write((char*)thing);
    }
}