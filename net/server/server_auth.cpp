#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    // Evaluates a Client Hello from a client and sees if it is hopelessly confused or a real client.
    // TODO: Is this needed?
    bool Server::ClientIsNew(NET_Address* address)
    {
        return (GetMessageSender(address) == nullptr);
    }

    // Log in a new client
    void Server::ClientNew(NetMsg* msg)
    {               
        // ip for debug
        const char* ip = NET_GetAddressString(msg->addr);

        Logging_LogChannel("Client is attempting to connect from IP %s...", LogChannel::Debug, ip);

        NetHelloStatus helloStatus = NetHelloStatus::HELLO_OK;

        // check if the client already exists
        if (!ClientIsNew(msg->addr))
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

        // read the username
        const char* username = msg->Read<const char*>();

        // create the server hello packet
        NetMsg serverHello = NetFactory_CreateServerHelloPacket();

        if (numClients >= netMaxPlayers->value)
        {
            Logging_LogChannel("%s: Connection rejected: Too many clients!", LogChannel::Debug, ip);
            helloStatus = NetHelloStatus::HELLO_TOO_MANY;
        }

        serverHello.Write<uint8_t>(static_cast<uint8_t>(helloStatus));

        if (helloStatus == NetHelloStatus::HELLO_OK)
        {

            Client* client = new Client();
            clients[numClients] = client;

            strncpy(client->name, ip, CLIENT_NAME_LENGTH);
            client->serverOnly.address = msg->addr;
            strncpy(client->serverOnly.ipStr, ip, CLIENT_IP_LENGTH);
            client->serverOnly.port = msg->port;
            numClients++;

            Logging_LogChannel("%s: Connection accepted from %s!", LogChannel::Debug, ip, username);
            SendMessage(serverHello, client);
        }
        else
            SendMessageToPort(NetFactory_CreateDisconnectPacket(), msg->addr, msg->port);
    }

    void Server::ClientRemove(Client* client)
    {
        if (!numClients)
            return;

        NET_UnrefAddress(client->serverOnly.address); 
        delete client;

        clients[numClients - 1] = nullptr;

        numClients--;
    }
}