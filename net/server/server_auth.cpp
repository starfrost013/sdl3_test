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
            Logging_LogChannel("Connection rejected: Duplicate client", LogChannel::Warning);
            helloStatus = NetHelloStatus::HELLO_DUPLICATE_CLIENT;
        }

        uint8_t netVersion = msg->Read<uint8_t>();

        if (netVersion != NET_PROTOCOL_VERSION)
        {
            Logging_LogChannel("Connection rejected: Incorrect client protocol version %d, expected %d", LogChannel::Warning,
            netVersion, NET_PROTOCOL_VERSION);
            helloStatus = NetHelloStatus::HELLO_INVALID_VERSION;
        }

        NetMsg serverHello = NetFactory_CreateServerHelloPacket();

        if (numClients >= netMaxPlayers->value)
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
            client->serverOnly.port = msg->port;
            numClients++;

            SendMessage(serverHello, client);
        }
        else
        {
            SendMessageToPort(NetFactory_CreateDisconnectPacket(), msg->addr, msg->port);
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