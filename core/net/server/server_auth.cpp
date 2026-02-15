#include <core/net/client/client.hpp>
#include <core/net/server/server.hpp>

namespace Capy
{
    // Evaluates a Client Hello from a client and sees if it is hopelessly confused or a real client.
    // TODO: Is this needed?
    bool Server::IsNewClient(NET_Address* address)
    {
        for (Client* client : clients)
        {
            if (client != nullptr)
            {
                if (NET_CompareAddresses(client->serverOnly.address, address))
                    return false; 
            }
        }

        return true; 
    }

    void Server::NewClient(NetMsg* msg)
    {
        if (!IsNewClient(msg->addr))
        {
            Logging_LogChannel("??????? Existing client sent NETMSG_HELLO ???????",  LogChannel::Warning);
            return;
        }

        // ip for debug
        const char* ip = NET_GetAddressString(msg->addr);

        Logging_LogChannel("Client is attempting to connect from IP %s...", LogChannel::Debug, ip);

        NetMsg netMsg = NetFactory_CreateServerHelloPacket();
        NetHelloStatus helloStatus = NetHelloStatus::HELLO_OK;

        if (numClients >= MAX_CLIENTS)
        {
            Logging_LogChannel("%s: Connection rejected: Too many clients!", LogChannel::Debug, ip);
            helloStatus = NetHelloStatus::HELLO_TOO_MANY;
        }

        netMsg.Write<uint8_t>(static_cast<uint8_t>(helloStatus));

        if (helloStatus == NetHelloStatus::HELLO_OK)
        {
            Client* client = new Client();
            clients[numClients] = client;

            client->serverOnly.address = netMsg.addr;
            
            numClients++;
        }
        else
        {
            SendMessage(NetFactory_CreateDisconnectPacket(NetCastType::NET_CAST_TO_CLIENT), netMsg.addr);
        }
    }

    void Server::RemoveClient(Client* client)
    {
        NET_UnrefAddress(client->serverOnly.address); 
        delete client;
    }
}