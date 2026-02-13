#include <core/net/net.hpp>

namespace Capy
{
    Cvar* netMode;

    //temporary
    Cvar* netServerAddress;
    Cvar* netPort;

    void CapyNet_Init()
    {   
        Logging_LogChannel("CapyNet_Init: Initialising network...", LogChannel::Message);

        netMode = Cvar_Get("netMode", "0", false);
        netServerAddress = Cvar_Get("netServerAddress", "127.0.0.1", false);
        netPort = Cvar_Get("netPort", "6769", false);

        NET_Init();
    }

    void NetMode::GetAllIncomingMessages()
    {
        // we need to manage the lifetime of these objects
        NET_Datagram* dgram; 

        bool success = NET_ReceiveDatagram(socket, &dgram);

        NetMessage msg = {};

        if (success && !dgram)
        {
            // no message to receive
            return; 
        }

        /* cast start of message to header */

        if (dgram->buflen < sizeof(NetMessage::NetHeader))
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - size must be at least %d", LogChannel::Error, sizeof(NetMessage::NetHeader));
            return;
        }

        memcpy(&msg.header, dgram->buf, sizeof(NetMessage::NetHeader));

        if (msg.header.magic != NETMSG_MAGIC)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid magic", LogChannel::Error);
            return;
        }
   
        //todo; store a buffer of messages and reorder them etc

        if (msg.header.size == 0
        || msg.header.size > MAX_PACKET_SIZE)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid size %d", LogChannel::Error, msg.header.size);
            return;
        }

        if (msg.header.castType > NET_CAST_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid cast type %d", LogChannel::Error, msg.header.messageType);
            return;
        }

        // check for valid message type
        if (msg.header.messageType > NETMSG_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid msg type %d", LogChannel::Error, msg.header.messageType);
            return;
        }

        //todo: packet type

        msg.valid = true;
        seqNumber++;

        msg.messageData = new uint8_t[dgram->buflen];
        memcpy(msg.messageData, &dgram->buf[sizeof(NetMessage::NetHeader)], dgram->buflen - sizeof(NetMessage::NetHeader));

        /* Known Address Identification here */

        // add to buffer

        netBufferPtr++;

        netBuffer[netBufferPtr] = msg;

        if (netBufferPtr >= NET_BUFFER_SIZE)
        {
            Logging_LogChannel("NetMode::GetMessage - NetBuffer overflow, reset", LogChannel::Warning);
            netBufferPtr = 0; 
        }

        NET_DestroyDatagram(dgram);
        return; 
    }
    
    NetMessage* NetMode::GetMessage()
    {
        if (!netBufferPtr)
            return nullptr;

        netBufferPtr -= 1;
        return &netBuffer[netBufferPtr + 1];
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
        msg.header.size = size - sizeof(NetMessage::NetHeader);
        msg.valid = true;
        msg.messageData = new uint8_t[size];
        
        memcpy(msg.messageData, data, size);

        NET_SendDatagram(socket, address, port, (void*)&msg, sizeof(msg));

        seqNumber++;
    }

    void NetMode::Frame()
    {
        // try and get messages AFAP
        GetAllIncomingMessages();
    }

    void CapyNet_Shutdown()
    {
        Logging_LogChannel("CapyNet_Shutdown: Shutting down network...", LogChannel::Message);
        NET_Quit();
    }
}

