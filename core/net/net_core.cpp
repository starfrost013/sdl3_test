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

        msg.header = (NetMessage::NetHeader*)dgram->buf;

        if (msg.header->magic != NETMSG_MAGIC)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid magic", LogChannel::Error);
            return msg;
        }
   
        //todo; store a buffer of messages and reorder them etc

        if (msg.header->size == 0
        || msg.header->size > MAX_PACKET_SIZE)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid size %d", LogChannel::Error, msg.header->size);
            return msg;
        }

        if (msg.header->castType > NET_CAST_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid cast type %d", LogChannel::Error, msg.header->messageType);
            return msg;
        }

        // check for valid message type
        if (msg.header->messageType > NETMSG_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetMessage - invalid msg type %d", LogChannel::Error, msg.header->messageType);
            return msg;
        }

        //todo: packet type

        msg.valid = true;
        seqNumber++;

        msg.messageData = new uint8_t[dgram->buflen];
        memcpy(msg.messageData, &dgram->buf[sizeof(NetMessage::NetHeader)], dgram->buflen - sizeof(NetMessage::NetHeader));

        return msg; 
    }

    void NetMode::SendMessage(NetMessage msg)
    {
        
    }

    void CapyNet_Shutdown()
    {
        Logging_LogChannel("CapyNet_Shutdown: Shutting down network...", LogChannel::Message);
        NET_Quit();
    }
}

