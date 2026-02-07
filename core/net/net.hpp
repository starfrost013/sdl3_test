/* 
    Shared network headers 
*/

#pragma once
#include <Capy.hpp>
#include <SDL3_net/SDL_net.h>

/* 
    Network message 
    For now everything will be in one server using SDL_net (which is TCP)
    If we need to, we should be able to rapidly rewrite to use UDP
*/

namespace Capy
{
    enum NetMessageType
    {
        NETMSG_HELLO = 0x0,
        NETMSG_WORLD_DOWNLOAD = 0x1,
        NETMSG_PING = 0x2,
        NETMSG_PONG = 0x3,
    };

    //basic check that the message was received (tcp but still)
    #define NETWORK_MESSAGE_MAGIC       0x00FF55AA

    struct NetMessage
    {
        uint32_t magic;
        std::size_t seqNumber;
        uint8_t messageType; 
        uint8_t* messageData;

        template <typename T>
        T Read()
        {

        }
    };

    // network mode
    enum NetType
    {
        NETMODE_CLIENT = 0,             // Client only
        NETMODE_SERVER_LISTEN = 1,      // Server and client
        NETMODE_SERVER_DEDICATED = 2,   // Server only
    };

    /* Base for all network modes */
    class NetMode
    {
        public:
            virtual void Init() { };
            virtual void Shutdown() { };

        protected:
            // single source of truth for UDP packet sequencing (since it can be out of order, etc.)
            size_t seqNumber; 
    };

    /* 
        Net system init 
        Called "CapyNet" to prevent confusion with exising system
    */
    void CapyNet_Init();
    void CapyNet_Shutdown();
}