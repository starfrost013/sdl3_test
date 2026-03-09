//
// client_rpc.cpp: Client RPC handling (e.g. updating other clients)
//

#include <core/game.hpp>
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
            Logging_LogChannel("***** Failed to invoke Client RPC type %d: %s -- ", LogChannel::Error, e.what());
        }
        
    }

    void Client::OnRpcClientCreate(NetMsg* msg)
    {
        
    }
}