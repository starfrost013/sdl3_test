#include <net/net.hpp>

namespace Capy
{
    Cvar* netMode;

    //temporary
    Cvar* netServerAddress;
    Cvar* netPort;
    Cvar* netMaxPlayers;

    void CapyNet_Init()
    {   
        Logging_LogChannel("CapyNet_Init: Initialising network...", LogChannel::Message);

        netMode = Cvar_Get("netMode", "0", false);
        netServerAddress = Cvar_Get("netServerAddress", "127.0.0.1", false);
        netPort = Cvar_Get("netPort", "6769", false);
        netMaxPlayers = Cvar_Get("netMaxPlayers", "32", false);

        if (netMaxPlayers->value == 0)
            Cvar_Set("netMaxPlayers", "32", false);
        else if (netMaxPlayers->value > MAX_CLIENTS)
            Cvar_Set("netMaxPlayers", "32", false);

        NET_Init();
    }

    // TODO: Refactor this to ues the constructor
    void NetMode::GetAllIncomingMessages()
    {
        // we need to manage the lifetime of these objects
        NET_Datagram* dgram; 

        bool success = NET_ReceiveDatagram(socket, &dgram);

        NetMsg msg = {};
        memset(&msg, 0x00, sizeof(NetMsg)); // clear all pointers, etc to 0
        
        if (success && !dgram)
        {
            // no message to receive
            return; 
        }

        /* cast start of message to header */

        if (dgram->buflen < sizeof(NetMsg::NetHeader))
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - size must be at least %d", LogChannel::Error, sizeof(NetMsg::NetHeader));
            return;
        }

        memcpy(&msg.header, dgram->buf, sizeof(NetMsg::NetHeader));

        if (msg.header.magic != NETMSG_MAGIC)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid magic", LogChannel::Error);
            return;
        }
   
        //todo; store a buffer of messages and reorder them etc

        if (msg.header.size > MAX_PACKET_SIZE)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid size %d", LogChannel::Error, msg.header.size);
            return;
        }

        if (msg.header.castType > NET_CAST_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid cast type %d", LogChannel::Error, msg.header.msgType);
            return;
        }

        // check for valid message type
        if (msg.header.msgType > NETMSG_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid msg type %d", LogChannel::Error, msg.header.msgType);
            return;
        }

        //todo: packet type

        msg.valid = true;
        seqNumber++;

        // some are header only
        // only obtain the amount that actually exists
        if (msg.header.size > 0)
        {
            msg.msgData = new uint8_t[dgram->buflen];
            memcpy(msg.msgData, &dgram->buf[sizeof(NetMsg::NetHeader)], dgram->buflen - sizeof(NetMsg::NetHeader));
        }

        msg.addr = dgram->addr;
        
        //temp, slightly bad design
        NET_RefAddress(dgram->addr);

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
    
    NetMsg* NetMode::GetMessage()
    {
        if (!netBufferPtr)
            return nullptr;

        netBufferPtr -= 1;
        return &netBuffer[netBufferPtr + 1];
    }

    void NetMode::SendMessage(NetMsg msg, NET_Address* address)
    {
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
