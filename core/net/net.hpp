/* 
    Shared network headers 
*/

#pragma once
#include <Capy.hpp>
#include <SDL3_net/SDL_net.h>

/* 
    Network message 
    For now everything will be in one server using SDL_net (which is TCP)
    If we need to, we should be able to rapidly rewrite to use UDP
*/

namespace Capy
{
    #define PORT_DEFAULT                6769    // 6-7 69

    // net cast types
    // these are encoded into the
    enum NetCast
    {  
        NET_CAST_TO_SERVER = 0,                             // Message from client to server
        NET_CAST_TO_CLIENT = 1,                             // Message from server to client
        NET_CAST_TO_ALL_CLIENTS = 2,                        // Message from server to all clients
        NET_CAST_LAST_VALID = NET_CAST_TO_ALL_CLIENTS,                    // Sentinel value
    };

    // maybe these should be classes...maybe not
    enum NetMessageType
    {
        // Client to server messages - 0x0-0x7f
        // Server to client messages - 0x80-0xff

        NETMSG_IS_SERVER_MESSAGE = 0x80,

        NETMSG_HELLO = 0x0,                                         // Client Hello & Identity
        NETMSG_WORLD_DOWNLOAD_START = 0x2,                          // Client Request World Download
        NETMSG_WORLD_DOWNLOAD_PACKET = 0x3,                         // Client Request World Packet
        NETMSG_WORLD_SPAWN_ENTITY = 0x4,                            // Client Request Entity Spawn (a.k.a. Client)
        NETMSG_WORLD_UPDATE_ENTITY = 0x5,                           // Client Request Entity Update
        NETMSG_DISCONNECT = 0x6,                                    // Client Disconnect

        NETMSG_SERVER_HELLO = NETMSG_IS_SERVER_MESSAGE | 0x0,
        NETMSG_SERVER_HEARTBEAT = NETMSG_IS_SERVER_MESSAGE | 0x1,   // Server heartbeat (if nothing for 5 seconds, disconnect)  
        NETMSG_UPDATE_ENTITY = NETMSG_IS_SERVER_MESSAGE | 0x2,      // Server update entity                            

        NETMSG_LAST_VALID = NETMSG_UPDATE_ENTITY,                   // Sentinel value
    };

    // enumerates network hello response statuses from the server to the client
    enum NetHelloStatus
    {
        HELLO_OK = 0,                                               // Go ahead and have fun
        HELLO_DUPLICATE_USERNAME = 1,                               // Duplicate username
        HELLO_GO_AWAY = 2,                                          // YOU HAVE BEEN IP BAAAAAAAAAAAAANED
        HELLO_TOO_MANY = 3,                                         // Maximum client reached
    };

    //basic check that the message was received (tcp but still)
    #define NETMSG_MAGIC            0x00FF55AA
    #define MAX_PACKET_SIZE         512      // max reliable, may increase later
    #define PACKET_RESIZE_FACTOR    1.5

    struct NetMessage
    {
        struct NetHeader
        {
            uint32_t magic;                 // magic
            std::size_t seqNumber;          // sequence number for message re-ordering etc
            // these are not enums because they could theoretically be out of range
            uint32_t size;                  // size does not need to be 8 bytes (nobody will send 2 GB packet!)
            uint8_t castType;               // see netcast above
            uint8_t msgType;                // type of message
        };

        NetHeader header;                   // header

        uint8_t* msgData;                   // data of message
        bool valid;                         // true if message parsed successfulyl        uint8_t msgPtrRead;                 // current location within the message while reading
        uint32_t msgPtrRead;                // current location within the message while writing    
        uint32_t msgPtrWrite;               // current location within the message while writing    

        NET_Address* addr;                  // sender's address

        // Create a new NetMessage. Used for GetMessage
        NetMessage()
        {

        }

        // Create a new NetMessage with a given message type, cast type, size and optional data
        NetMessage(NetCast castType, NetMessageType msgType, uint8_t data[], uint32_t size)
        {
            if (size > MAX_PACKET_SIZE)
            {
                Logging_LogChannel("NetMode::SendMessage - %d is larger than max packet size %d, ignoring", LogChannel::Warning, size, MAX_PACKET_SIZE);
                return;
            }

            header.magic = NETMSG_MAGIC;
            header.size = size;
            header.castType = castType;
            header.msgType = msgType;
            
            valid = true;
            msgPtrRead = msgPtrWrite = 0;

            // packets don't have to have data
            if (header.size > 0)
            {
                msgData = new uint8_t[header.size];
                
                // data being nullptr means don't fill with anything
                if (data != nullptr)
                    memcpy(&msgData, data, size);
            }

        }

        template <typename T>
        T Read()
        {
            return (T)msgData[msgPtrRead];

            msgPtrRead += sizeof(T);
        }

        template <typename T>
        void Write(T thing)
        {
            // resize by 1.5x if the size would overflow
            while (msgPtrWrite + sizeof(thing) > header.size)
            {
                uint32_t oldSize = header.size;
                header.size *= 1.5;

                if (header.size >= MAX_PACKET_SIZE)
                    goto overflow;

                uint8_t* newMsgData = new uint8_t[header.size];
                memcpy(newMsgData, msgData, oldSize);

                // delete old message data
                delete msgData;
                msgData = newMsgData;
            }

            if (msgPtrWrite + sizeof(thing) >= MAX_PACKET_SIZE)
                goto overflow;

            memcpy(&msgData[msgPtrWrite], &thing, sizeof(thing));
            msgPtrWrite += sizeof(thing);

            return;
        overflow:
            Logging_LogChannel("NetMessage::Write - overflow (max size is %d)", LogChannel::Error, MAX_PACKET_SIZE);
            return;
        }
    };

    // network mode
    enum NetType
    {
        NETMODE_CLIENT = 0,             // Client only
        NETMODE_SERVER_LISTEN = 1,      // Server and client
        NETMODE_SERVER_DEDICATED = 2,   // Server only
    };
    
    // Cvars
    extern Cvar* netMode;
    extern Cvar* netServerAddress;
    extern Cvar* netPort;

    /* Base for all network modes */
    class NetMode
    {
        public:
            virtual void Init() { };
            virtual void Frame();
            virtual void Tick() { };

            NetMessage* GetMessage();

            // this is very likely to be a temporary interface for testing until we have a real packet system
            void SendMessage(NetMessage msg, NET_Address* address);

            virtual void Shutdown() { };

        protected:
            void GetAllIncomingMessages();

            size_t seqNumber;               // single source of truth for UDP packet sequencing (since it can be out of order, etc.)
            uint16_t port;                  // port
            NET_DatagramSocket* socket;     // socket to use

            // We only process packets every 1/tickrate seconds,
            // but they could theoretically be sent faster. Therefore we have a buffer to put in
            static const int32_t NET_BUFFER_SIZE = 64;    // Maximum number of packets in the buffer that can be serviced at any one time
            NetMessage netBuffer[NET_BUFFER_SIZE];  
            int32_t netBufferPtr = 0;
    };

    /* Net packet factory stuff */

    NetMessage NetFactory_CreateClientHelloPacket(NetCast castType);
    NetMessage NetFactory_CreateServerHelloPacket();
    NetMessage NetFactory_CreateDisconnectPacket(NetCast castType);

    /* 
        Net system init 
        Called "CapyNet" to prevent confusion with exising system
    */
    void CapyNet_Init();
    void CapyNet_Shutdown();
}