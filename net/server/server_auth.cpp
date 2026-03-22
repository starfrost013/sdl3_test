#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    // Evaluates a Client Hello from a client and sees if it is hopelessly confused or a real client.
    // TODO: Is this needed?
    bool Server::ClientIsNew(NetMsg* msg, const char* username)
    {
        // ip doesn't exist -> new client
        if (!ClientByIp(msg))
            return true;
        
        // username exists -> not new client
        if (ClientByUsername(username))
            return false;

        // otherwise multiple ip's on same clieent
        return true;  
    }

    // Log in a new client
    void Server::ClientNew(NetMsg* msg)
    {               
        // ip for debug
        const char* ip = NET_GetAddressString(msg->addr);

        Logging_LogChannel("Client is attempting to connect from IP %s...", LogChannel::Debug, ip);

        NetHelloStatus helloStatus = NetHelloStatus::HELLO_OK;
        uint8_t netVersion = msg->Read<uint8_t>();

        if (netVersion != NET_PROTOCOL_VERSION)
        {
            Logging_LogChannel("Connection rejected: Incorrect client protocol version %d, expected %d", LogChannel::Warning,
            netVersion, NET_PROTOCOL_VERSION);
            helloStatus = NetHelloStatus::HELLO_INVALID_VERSION;
        }

        // read the username
        const char* username = msg->Read<const char*>();

        // check if the client already exists
        if (!ClientIsNew(msg, username))
        {
            Logging_LogChannel("Connection rejected: Duplicate client (username and IP pair already exists)", LogChannel::Warning);
            helloStatus = NetHelloStatus::HELLO_DUPLICATE_CLIENT;
        }

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

            strncpy(client->name, ip, CLIENT_NAME_MAX);
            client->serverOnly.address = msg->addr;
            strncpy(client->serverOnly.ipStr, ip, CLIENT_IP_LENGTH);
            client->serverOnly.port = msg->port;
            numClients++;

            Logging_LogChannel("%s: Connection accepted from %s!", LogChannel::Debug, ip, username);
            SendMessage(serverHello, client);

            // now tell all other clients except this client
            SendMessageToAll(NetFactory_CreateRpcClientCreate(client->name), client);
        }
        // the client will disconnect itself upon receiving the hello packet with a non OK reason
    }

    void Server::ClientRemove(Client* client)
    {
        SendMessageToAll(NetFactory_CreateRpcClientDelete(client->name), client);

        if (!numClients)
            return;

        NET_UnrefAddress(client->serverOnly.address); 
        delete client;

        clients[numClients - 1] = nullptr;

        numClients--;
    }
}