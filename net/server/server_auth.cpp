#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    // Evaluates a Client Hello from a client and sees if it is hopelessly confused or a real client.
    // TODO: Is this needed?
    bool Server::IsNewClient(NET_Address* address)
    {
        return (GetMessageSender(address) == nullptr);
    }

    // Log in a new client
    void Server::NewClient(NetMsg* msg)
    {               
        // ip for debug
        const char* ip = NET_GetAddressString(msg->addr);
 
        Logging_LogChannel("Client is attempting to connect from IP %s...", LogChannel::Debug, ip);

        NetHelloStatus helloStatus = NetHelloStatus::HELLO_OK;

        if (!IsNewClient(msg->addr))
        {
            Logging_LogChannel("Connection rejected: Duplicate client",  LogChannel::Warning);
            helloStatus = NetHelloStatus::HELLO_DUPLICATE_CLIENT;
        }

        NetMsg serverHello = NetFactory_CreateServerHelloPacket();

        if (numClients >= MAX_CLIENTS)
        {
            Logging_LogChannel("%s: Connection rejected: Too many clients!", LogChannel::Debug, ip);
            helloStatus = NetHelloStatus::HELLO_TOO_MANY;
        }

        serverHello.Write<uint8_t>(static_cast<uint8_t>(helloStatus));

        if (helloStatus == NetHelloStatus::HELLO_OK)
        {
            Logging_LogChannel("Connection accepted!", LogChannel::Debug, ip);

            Client* client = new Client();
            clients[numClients] = client;

            client->serverOnly.address = msg->addr;
            strncpy(client->serverOnly.ipStr, ip, CLIENT_IP_LENGTH);
            
            numClients++;

            SendMessage(serverHello, msg->addr);
        }
        else
        {
            SendMessage(NetFactory_CreateDisconnectPacket(NetCastType::NET_CAST_TO_CLIENT), msg->addr);
        }
    }

    void Server::RemoveClient(Client* client)
    {
        if (!numClients)
            return;

        NET_UnrefAddress(client->serverOnly.address); 
        delete client;

        clients[numClients - 1] = nullptr;

        numClients--;
    }
}