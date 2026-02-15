/* 
    The Packet Factory: Creates packets for use
*/

#include <core/net/net.hpp>

namespace Capy
{
    NetMsg NetFactory_CreateClientHelloPacket(NetCastType castType)
    {
        // no data
        return NetMsg(castType, NetMsgType::NETMSG_HELLO, nullptr, 0);
    }

    NetMsg NetFactory_CreateServerHelloPacket()
    {
        return NetMsg(NetCastType::NET_CAST_TO_CLIENT, NetMsgType::NETMSG_SERVER_HELLO, nullptr, sizeof(uint8_t));
    }

    NetMsg NetFactory_CreateDisconnectPacket(NetCastType castType)
    {
        return NetMsg(castType, NetMsgType::NETMSG_DISCONNECT, nullptr, 0);
    }
}