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
        // Server to client messages 

        NETMSG_HELLO = 0x0,
        NETMSG_WORLD_DOWNLOAD = 0x1,
        NETMSG_PING = 0x2,
        NETMSG_PONG = 0x3,

        NETMSG_LAST_VALID = NETMSG_PONG,                    // Sentinel value
    };

    //basic check that the message was received (tcp but still)
    #define NETMSG_MAGIC           0x00FF55AA
    #define MAX_PACKET_SIZE        512      // max reliable, may increase later

    struct NetMessage
    {
        struct NetHeader
        {
            uint32_t magic;                 // magic
            std::size_t seqNumber;          // sequence number for message re-ordering etc
            // these are not enums because they could theoretically be out of range
            uint32_t size;                  // size does not need to be 8 bytes (nobody will send 2 GB packet!)
            uint8_t castType;               // see netcast above
            uint8_t messageType;            // type of message
        };

        NetHeader header;                   // header

        uint8_t* messageData;               // data of message
        bool valid;                         // true if message parsed successfulyl
        uint8_t msgPtr;                     // current location within the message

        template <typename T>
        T Read()
        {
            return (T)messageData[msgPtr];

            msgPtr += sizeof(T);
        }
    };

    // network mode
    enum NetType
    {
        NETMODE_CLIENT = 0,             // Client only
        NETMODE_SERVER_LISTEN = 1,      // Server and client
        NETMODE_SERVER_DEDICATED = 2,   // Server only
    };
    
    extern Cvar* netMode;

    /* Base for all network modes */
    class NetMode
    {
        public:
            virtual void Init() { };
            virtual void Frame() { };
            virtual void Tick() { };

            NetMessage GetMessage();

            // this is very likely to be a temporary interface for testing until we have a real packet system
            void SendMessage(NetMessageType msgType, NET_Address* address, uint8_t* data, uint32_t size, NetCast castType);

            virtual void Shutdown() { };

        protected:
            size_t seqNumber;               // single source of truth for UDP packet sequencing (since it can be out of order, etc.)
            uint16_t port;                  // port
            NET_DatagramSocket* socket;     // socket to use

    };

    /* 
        Net system init 
        Called "CapyNet" to prevent confusion with exising system
    */
    void CapyNet_Init();
    void CapyNet_Shutdown();
}