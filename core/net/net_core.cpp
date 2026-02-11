#include <core/net/net.hpp>

namespace Capy
{
    Cvar* netMode;

    void CapyNet_Init()
    {   
        Logging_LogChannel("CapyNet_Init: Initialising network...", LogChannel::Message);

        netMode = Cvar_Get("netMode", "0", false);

        NET_Init();
    }

    NetMessage NetMode::GetMessage(NET_Datagram* dgram)
    {
        NetMessage msg = {};

        if (!dgram)
        {
            Logging_LogChannel("NetMode::GetMessage - NULL dgram", LogChannel::Error);
            return msg; 
        }

        /* cast start of message to header */

        if (dgram->buflen < sizeof(NetMessage::NetHeader))
        {
            Logging_LogChannel("NetMode::GetMessage - size must be at least %d", LogChannel::Error, sizeof(NetMessage::NetHeader));
            return msg;
        }

        memcpy(&msg.header, dgram->buf, sizeof(NetMessage::NetHeader));

        if (msg.header.magic != NETMSG_MAGIC)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid magic", LogChannel::Error);
            return msg;
        }
   
        //todo; store a buffer of messages and reorder them etc

        if (msg.header.size == 0
        || msg.header.size > MAX_PACKET_SIZE)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid size %d", LogChannel::Error, msg.header.size);
            return msg;
        }

        if (msg.header.castType > NET_CAST_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid cast type %d", LogChannel::Error, msg.header.messageType);
            return msg;
        }

        // check for valid message type
        if (msg.header.messageType > NETMSG_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid msg type %d", LogChannel::Error, msg.header.messageType);
            return msg;
        }

        //todo: packet type

        msg.valid = true;
        seqNumber++;

        msg.messageData = new uint8_t[dgram->buflen];
        memcpy(msg.messageData, &dgram->buf[sizeof(NetMessage::NetHeader)], dgram->buflen - sizeof(NetMessage::NetHeader));

        return msg; 
    }

    void NetMode::SendMessage(NetMessageType msgType, NET_Address* address, uint8_t* data, uint32_t size, NetCast castType = NET_CAST_TO_ALL_CLIENTS)
    {
        if (!size)
        {
            Logging_LogChannel("NetMode::SendMessage - Size is 0, returning", LogChannel::Warning, size, MAX_PACKET_SIZE);
            return;
        }

        if (size > MAX_PACKET_SIZE)
        {
            Logging_LogChannel("NetMode::SendMessage - %d is larger than max packet size %d, ignoring", LogChannel::Warning, size, MAX_PACKET_SIZE);
            return;
        }

        NetMessage msg;
        msg.header.magic = NETMSG_MAGIC;
        msg.header.seqNumber = seqNumber;
        msg.header.messageType = msgType;
        msg.header.castType = castType;
        msg.header.size = sizeof(msg.messageData) - sizeof(NetMessage::NetHeader);
        msg.valid = true;
        msg.messageData = new uint8_t[size];
        
        memcpy(msg.messageData, data, size);

        NET_SendDatagram(socket, address, port, (void*)&msg, sizeof(msg));

        seqNumber++;
    }

    void CapyNet_Shutdown()
    {
        Logging_LogChannel("CapyNet_Shutdown: Shutting down network...", LogChannel::Message);
        NET_Quit();
    }
}

