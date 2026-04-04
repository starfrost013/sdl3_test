//
// client_rpc.cpp: Client RPC handling (e.g. updating other clients)
//

#include <core/engine.hpp>
#include <core/render/render.hpp>
#include <world/world.hpp>
#include <net/client/client.hpp>

namespace Capy
{
    // Run on RPC being received.
    void Client::OnRpcReceive(NetMsg* msg)
    {   
        uint8_t rpcType = msg->Read<uint8_t>();

        try
        {
            std::invoke(rpcReceiveMethods[rpcType], this, msg);
        }
        catch(const std::exception& e)
        {
            Logging_LogChannel("Client::OnRpcReceive - Failed to invoke Client RPC type %d: %s", LogChannel::Error, e.what());
        }
    }

    // Called on create client RPC request being received
    void Client::OnRpcClientCreate(NetMsg* msg)
    {
        assert(numClients <= MAX_CLIENTS);

        // sanity check
        if (numClients == MAX_CLIENTS)
            return;

        // clients dont' store as much information about other clients as servers do
        // only information about *other* clients are stored

        Client* client = new Client();

        const char* username = msg->Read<const char*>();
        strncpy(client->name, username, CLIENT_NAME_MAX);
        
        clients[numClients] = client;

        Logging_LogChannel("%s joined the game", LogChannel::Debug, username);

        numClients++; 
    }

    // Called on create client RPC request being received
    void Client::OnRpcClientDelete(NetMsg* msg)
    {
        assert(numClients >= 0);

        // sanity check
        if (numClients == MAX_CLIENTS)
            return;

        // clients dont' store as much information about other clients as servers do
        // only information about *other* clients are stored
        // should we have a numerical id?
        const char* username = msg->Read<const char*>();
        bool realClientFound = false;

        for (Client* client : clients)
        {
            if (client
            && !strncmp(client->name, username, CLIENT_NAME_MAX))
            {
                realClientFound = true; 
                delete client;
                clients[numClients] = nullptr; 
            }
        }

        if (!realClientFound)
        {
            Logging_LogChannel("Client::OnRpcClientDelete: Invalid client name for RpcDelete %s (time for an ID system...)", LogChannel::Warning, username);
            return; 
        }

        Logging_LogChannel("%s left the game", LogChannel::Debug, username);

        numClients--; 
    }
}