/* 
    The Packet Factory: Creates packets for use
*/

#include <core/net/net.hpp>

namespace Capy
{
    NetMessage NetFactory_CreateClientHelloPacket(NetCast castType)
    {
        // no data
        return NetMessage(castType, NetMessageType::NETMSG_HELLO, nullptr, 0);
    }

    NetMessage NetFactory_CreateServerHelloPacket()
    {
        return NetMessage(NetCast::NET_CAST_TO_CLIENT, NetMessageType::NETMSG_SERVER_HELLO, nullptr, sizeof(uint8_t));
    }

    NetMessage NetFactory_CreateDisconnectPacket(NetCast castType)
    {
        return NetMessage(castType, NetMessageType::NETMSG_DISCONNECT, nullptr, 0);
    }
}