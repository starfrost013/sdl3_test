/* 
    Shared network headers 
*/

#pragma once
#include <SDL3_net/SDL_net.h>
#include <Capy.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    //
    // DEFINES
    //

    #define PORT_DEFAULT                6769            // 6-7 69
    #define MAX_CLIENTS                 32

    #define NETMSG_MAGIC                0x00FF55AA      // basic check that the message was received (tcp but still)
    #define MAX_PACKET_SIZE             1400            // maximum reliable packet size for IP/Ethernet (1400 bytes)
    #define PACKET_RESIZE_FACTOR        1.5             // amount packets get resized by when they are full
    #define NET_PROTOCOL_VERSION        3               // network protocol version (1 = pre-alpha 4.0/5.0, 2 = pre-alpha 6)
    
    // 
    // CVARS
    //  

    extern Cvar* netMode;
    extern Cvar* netServerAddress;
    extern Cvar* netPort;
    extern Cvar* netMaxPlayers; 

    //
    // STRUCTURES & ENUMS
    //

    // net cast types
    // these are encoded into the
    enum NetCastType
    {  
        NET_CAST_TO_SERVER = 0,                             // Message from client to server
        NET_CAST_TO_CLIENT = 1,                             // Message from server to client
        NET_CAST_TO_ALL_CLIENTS = 2,                        // Message from server to all clients
        NET_CAST_LAST_VALID = NET_CAST_TO_ALL_CLIENTS,                    // Sentinel value
    };

    // maybe these should be classes...maybe not
    enum NetMsgType
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

        NETMSG_LAST_VALID = NETMSG_SERVER_HEARTBEAT,                   // Sentinel value
    };

    // enumerates network hello response statuses from the server to the client
    enum NetHelloStatus
    {
        HELLO_OK = 0,                                               // Go ahead and have fun
        HELLO_DUPLICATE_USERNAME = 1,                               // Duplicate username
        HELLO_GO_AWAY = 2,                                          // YOU HAVE BEEN IP BAAAAAAAAAAAAANED
        HELLO_TOO_MANY = 3,                                         // Maximum client reached
        HELLO_DUPLICATE_CLIENT = 4,                                 // Duplicate client
        HELLO_INVALID_VERSION = 5,                                  // Incorrect version
    };

    /* 
        Net system init and shutdown
        Called "CapyNet" to prevent confusion with SDL3_NET "NET" functions
    */
    void CapyNet_Init();
    void CapyNet_Shutdown();

    // 
    // Network message 
    // Everything is sent over UDP and buffered internally
    //

    struct NetMsg
    {
        struct NetHeader
        {
            uint32_t magic;                 // magic
            std::size_t seqNumber;          // sequence number for message re-ordering etc
            // these are not enums because they could theoretically be out of range
            uint32_t size;                  // size does not need to be 8 bytes (nobody will send 2 GB packet!)
            uint8_t msgType;                // type of message
        };

        NetHeader header;                   // header
        uint16_t port;                      // port message came from (usually not important)
        uint8_t* msgData;                   // data of message
        bool valid;                         // true if message parsed successfully
        uint32_t msgPtrRead;                // current location within the message while reading    
        uint32_t msgPtrWrite;               // current location within the message while writing    

        NET_Address* addr;                  // sender's address

        // Create a new NetMsg. Used for GetMessage
        NetMsg()
        {

        }

        // Create a new NetMsg with a given message type, cast type, size and optional data
        NetMsg(NetMsgType msgType, uint8_t data[], uint32_t size)
        {
            if (size > MAX_PACKET_SIZE)
            {
                Logging_LogChannel("NetMsg::NetMsg - %d is larger than max packet size %d, ignoring", LogChannel::Warning, size, MAX_PACKET_SIZE);
                return;
            }

            header.magic = NETMSG_MAGIC;
            header.size = size;
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

        // Read something from the buffer. Some types have specialised implementations (mostly pointers e.g. const char*)
        template <typename T>
        T Read()
        {
            T thing = *(T*)&msgData[msgPtrRead];
            
            if (EnsureCapacityRead(sizeof(thing)))
                msgPtrRead += sizeof(thing);

            return thing;
        }

        // Returns a boolean indicating that there are at least size bytes left in the packet to write.
        bool EnsureCapacityRead(size_t size)
        {
            bool success = ((msgPtrRead + size) <= MAX_PACKET_SIZE);

            if (!success)
                Logging_LogChannel("NetMsg::EnsureCapacityRead - overflow (max size is %d)", LogChannel::Error, MAX_PACKET_SIZE);

            return success; 
        }

        // Returns a boolean indicating that there are at least size bytes left in the packet to write.
        bool EnsureCapacityWrite(size_t size)
        {
            // resize by 1.5x if the size would overflow
            while (msgPtrWrite + size > header.size)
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

            if (msgPtrWrite + size >= MAX_PACKET_SIZE)
                goto overflow;

            return true; 

        overflow:
            Logging_LogChannel("NetMsg::EnsureCapacityWrite - overflow (max size is %d)", LogChannel::Error, MAX_PACKET_SIZE);
            return false;
        }

        template <typename T>
        void Write(T thing)
        {
            if (!EnsureCapacityWrite(sizeof(thing)))
                return;

            memcpy(&msgData[msgPtrWrite], &thing, sizeof(thing));
            msgPtrWrite += sizeof(thing);

            return;
        }
    };

    // explicit specialisations for strings
    template<> char* NetMsg::Read<char*>();
    template<> const char* NetMsg::Read<const char*>();
    template<> void NetMsg::Write<char*>(char* thing);
    template<> void NetMsg::Write<const char*>(const char* thing);

    // network mode
    enum NetModeEnum
    {
        NETMODE_CLIENT = 0,             // Client only
        NETMODE_SERVER_LISTEN = 1,      // Server and client
        NETMODE_SERVER_DEDICATED = 2,   // Server only
    };

    /* Base for all network modes */
    class NetMode
    {
        public:
            virtual void Init() { };
            virtual void Frame();
            virtual void Tick() { };

            NetMsg* GetMessage();

            // this is very likely to be a temporary interface for testing until we have a real packet system
            void SendMessageToPort(NetMsg msg, NET_Address* address, uint16_t port);

            virtual void Shutdown() { };

            
        protected:


            size_t seqNumber;               // single source of truth for UDP packet sequencing (since it can be out of order, etc.)
            NET_DatagramSocket* socket;     // socket to use

            // We only process packets every 1/tickrate seconds,
            // but they could theoretically be sent faster. Therefore we have a buffer to put in
            static const int32_t NET_BUFFER_SIZE = 64;    // Maximum number of packets in the buffer that can be serviced at any one time
            NetMsg netBuffer[NET_BUFFER_SIZE];  
            int32_t netBufferPtr = -1;

            WorldEntity world;              // replicated from server

            void GetAllIncomingMessages();  // add incoming messages to the buffer 
            void DoneMessage(NetMsg* msg);  // discards a message when done
            
            // check mode
            bool IsClient() { return (static_cast<NetModeEnum>(netMode->value) == NETMODE_CLIENT); }; 
            bool IsDedicated() { return (static_cast<NetModeEnum>(netMode->value) == NETMODE_SERVER_DEDICATED); }; 
            bool IsListen() { return (static_cast<NetModeEnum>(netMode->value) == NETMODE_SERVER_LISTEN); }; 
    };

    /* Net packet factory stuff */

    NetMsg NetFactory_CreateClientHelloPacket();
    NetMsg NetFactory_CreateServerHelloPacket();
    NetMsg NetFactory_CreateDisconnectPacket();
    NetMsg NetFactory_CreateDownloadStartPacket_Client();
    NetMsg NetFactory_CreateDownloadStartPacket_Server(const char* mapName, Vector2<int32_t> size, uint32_t expectedBytes);
    NetMsg NetFactory_CreateDownloadPacket(uint32_t size);


}