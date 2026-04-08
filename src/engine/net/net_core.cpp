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

        if (netMaxPlayers->value == 0
        || netMaxPlayers->value > MAX_CLIENTS)
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
        
        if (!success || !dgram)
        {
            // return value of this function apparently can be different on Windows compared to Linux.
            // on windows, failure doesn't actually mean a fatal error, but on linux, it just returns immediately with a nullptr for a dgram!
            // therefore we have to do this
            #ifdef __linux__
                if (!success)
                    Logging_LogChannel("NetMode::GetAllIncomingMessages - NET_ReceiveDatagram failed!", LogChannel::Fatal);
            #endif
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

        // check for valid message type
        if (msg.header.type > NETMSG_LAST_VALID)
        {
            Logging_LogChannel("NetMode::GetAllIncomingMessages - invalid msg type %d", LogChannel::Error, msg.header.type);
            return;
        }

        //todo: packet type

        msg.valid = true;
        seqNumber++;

        // some are header only
        // only obtain the amount that actually exists
        if (msg.header.size > 0)
        {
            msg.msgData = new uint8_t[msg.header.size];
            memcpy(msg.msgData, &dgram->buf[sizeof(NetMsg::NetHeader)], msg.header.size);
        }

        msg.addr = dgram->addr;
        msg.port = dgram->port;
        
        //temp, slightly bad design
        NET_RefAddress(dgram->addr);

        // add to buffer

        netBufferPtr++;

        if (netBufferPtr >= NET_BUFFER_SIZE)
        {
            Logging_LogChannel("NetMode::GetMessage - NetBuffer overflow, probably some messages will be dropped", LogChannel::Warning);
            netBufferPtr = 0;
            return; 
        }

        netBuffer[netBufferPtr] = msg;
        
        if (Cmdline_Check("-netdebug"))
            Logging_LogChannel("netBufferPtr is now %d", LogChannel::Message, netBufferPtr);
        
        NET_DestroyDatagram(dgram); 
        return; 
    }
    
    NetMsg* NetMode::GetMessage()
    {
        if (netBufferPtr < 0)
            return nullptr;

        NetMsg* msg = &netBuffer[netBufferPtr];
        netBufferPtr--;
        return msg;
    }

    void NetMode::DoneMessage(NetMsg* msg)
    {
        if (msg->header.size)
            delete msg->msgData;
    }

    void NetMode::SendMessageToPort(NetMsg msg, NET_Address* address, uint16_t port)
    {
        // optimised case: don't need to copy in the data 
        // could just use a static array, but then every packet would be 512 bytes when it doesn't need to be
        if (!msg.header.size)
            NET_SendDatagram(socket, address, port, (void*)&msg, sizeof(msg));
        else
        {
            // we can't just copy over the buf since it's a pointer
            // so create this temporary buffer. if it is too slow optimise it later
            int32_t bufSize = sizeof(NetMsg::NetHeader) + msg.header.size;
            uint8_t buf[bufSize] = {0};

            // make sure the data gets over properly
            memcpy(buf, &msg.header, sizeof(NetMsg::NetHeader));
            memcpy(buf + sizeof(NetMsg::NetHeader), msg.msgData, msg.header.size);

            NET_SendDatagram(socket, address, port, (void*)buf, bufSize);
        }

        //delete the message
        DoneMessage(&msg);

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
        // This randomly freezes (bug in SDL_Net ?)
        //NET_Quit();
    }
}
