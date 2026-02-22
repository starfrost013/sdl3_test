/* 
    The Packet Factory: Helper functions to create packets with a defined fot
*/

#include <net/net.hpp>

namespace Capy
{
    NetMsg NetFactory_CreateClientHelloPacket()
    {
        // no data
        NetMsg msg = NetMsg(NetMsgType::NETMSG_HELLO, nullptr, sizeof(uint8_t));
        msg.Write<uint8_t>(NET_PROTOCOL_VERSION);
        
        return msg; 
    }

    NetMsg NetFactory_CreateServerHelloPacket()
    {
        return NetMsg(NetMsgType::NETMSG_SERVER_HELLO, nullptr, sizeof(uint8_t));
    }

    NetMsg NetFactory_CreateDisconnectPacket()
    {
        return NetMsg(NetMsgType::NETMSG_DISCONNECT, nullptr, 0);
    }

    NetMsg NetFactory_CreateDownloadStartPacket_Client()
    {
        return NetMsg(NetMsgType::NETMSG_WORLD_DOWNLOAD_START, nullptr, 0);
    }

    NetMsg NetFactory_CreateDownloadStartPacket_Server(const char* mapName, Vector2<int32_t> size, uint32_t expectedBytes)
    {
        // worst case scenario it will just autoexpand anyway
        NetMsg msg = NetMsg(NetMsgType::NETMSG_WORLD_DOWNLOAD_START, nullptr, WORLD_NAME_LENGTH + sizeof(Vector2<int32_t>) + sizeof(uint32_t));
        msg.Write<const char*>(mapName);
        msg.Write<Vector2<int32_t>>(size);
        msg.Write<uint32_t>(expectedBytes);

        return msg; 
    }

    NetMsg NetFactory_CreateEntitySpawnPacket()
    {
        NetMsg msg = NetMsg(NetMsgType::NETMSG_WORLD_SPAWN_ENTITY, nullptr, MAX_PACKET_SIZE);
    }

    NetMsg NetFactory_CreateEntityUpdatePacket()
    {

    }

    NetMsg NetFactory_CreateDownloadPacket(uint32_t size)
    {
        return NetMsg(NetMsgType::NETMSG_WORLD_DOWNLOAD_PACKET, nullptr, size);
    }
}