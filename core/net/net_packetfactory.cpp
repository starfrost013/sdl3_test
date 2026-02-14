/* 
    The Packet Factory: Creates packets for use
*/

#include <core/net/net.hpp>

namespace Capy
{
    NetMessage NetFactory_SendHelloPacket(NetCast castType)
    {
        // no data
        return NetMessage(castType, NetMessageType::NETMSG_HELLO, nullptr, 0);
    }
}