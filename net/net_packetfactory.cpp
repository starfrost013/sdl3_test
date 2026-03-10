/* 
    The Packet Factory: Helper functions to create packets with a defined type/schema
*/

#include <net/net.hpp>

namespace Capy
{
    NetMsg NetFactory_CreateClientHelloPacket(const char* username)
    {
        // no data
        NetMsg msg = NetMsg(NetMsgType::NETMSG_HELLO, nullptr, sizeof(uint8_t));
        msg.Write<uint8_t>(NET_PROTOCOL_VERSION);
        msg.Write<const char*>(username);

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

    NetMsg NetFactory_CreateDownloadPacket(uint32_t size)
    {
        return NetMsg(NetMsgType::NETMSG_WORLD_DOWNLOAD_PACKET, nullptr, size);
    }

    // Create an RPC create packet
    NetMsg NetFactory_CreateRpcCreatePacket_Client(const char* classname)
    {
        NetMsg msg = NetMsg(NetMsgType::NETMSG_CLIENT_RPC, nullptr, sizeof(uint8_t) + strlen(classname));
     
        if (!Entity::HasNetIdentity(classname))
            return msg; 

        msg.Write<uint8_t>(NetMsgRpcType::NETMSG_RPC_REQUEST_ENTITY_CREATE);
        msg.Write<const char*>(classname);
        
        return msg;
    }

    // Create an RPC create packet
    NetMsg NetFactory_CreateRpcEntityCreate(uint32_t edictId)
    {
        NetMsg msg = NetMsg(NetMsgType::NETMSG_SERVER_RPC, nullptr, sizeof(uint8_t) + sizeof(uint32_t));
        msg.Write<uint8_t>(NetMsgRpcType::NETMSG_RPC_REQUEST_ENTITY_CREATE);
        msg.Write<uint32_t>(edictId);

        return msg;
    }

    NetMsg NetFactory_CreateRpcClientCreate(const char* username)
    {
        NetMsg msg = NetMsg(NetMsgType::NETMSG_SERVER_RPC, nullptr, sizeof(uint8_t) + strlen(username));
        msg.Write<uint8_t>(NetMsgRpcType::NETMSG_RPC_REQUEST_CLIENT_CREATE);
        msg.Write<const char*>(username);

        return msg; 
    }

    NetMsg NetFactory_CreateRpcClientDelete(const char* username)
    {
        NetMsg msg = NetMsg(NetMsgType::NETMSG_SERVER_RPC, nullptr, sizeof(uint8_t) + strlen(username));
        msg.Write<uint8_t>(NetMsgRpcType::NETMSG_RPC_REQUEST_CLIENT_DELETE);
        msg.Write<const char*>(username);

        return msg; 
    }
}