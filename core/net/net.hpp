/* 
    Shared network headers 
*/

#pragma once
#include <SDL3_net/SDL_net.h>
#include <core/net/netmodebase.hpp>

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
        uint8_t messageType; 
        uint8_t* messageData;
    };

    // network mode
    enum NetMode
    {
        NETMODE_CLIENT = 0,
        NETMODE_SERVER_LISTEN = 1,
        NETMODE_SERVER_DEDICATED = 2,
    };

    int8_t Net_ReadS8(NetMessage* msg);
    int16_t Net_ReadS16(NetMessage* msg);
    int32_t Net_ReadS32(NetMessage* msg);
    int64_t Net_ReadS64(NetMessage* msg);
    uint8_t Net_ReadU8(NetMessage* msg);
    uint16_t Net_ReadU16(NetMessage* msg);
    uint32_t Net_ReadU32(NetMessage* msg);
    uint64_t Net_ReadU64(NetMessage* msg);
}