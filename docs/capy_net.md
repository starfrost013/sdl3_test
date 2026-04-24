# CapyNet - Capybara Wars Network Protocol v5 Specification

## 1. Notes
Capybara wars implements a buffered UDP network specification that is intended to minimise data sent from server to client while being server-authoritative. It operates as a set of packets that are defined by the NetMsg enum; packets are created using a factory design pattern (the
NetFactory_* functions)

## 2. Message Format
Messages are formatted in a way that is unfortunately not really designed and kind of experimental, so I hope that a decent design will be reached. The message format is as follows:

**Magic (4 bytes, `uint32_t`)**: should be ```NETMSG_MAGIC``` (0x00FF55AA). THis magic is designed 

**seqNumber (8 bytes; `size_t`)**: Will be used for automatic message reordering over UDP. Not currently implemented

**size (4 bytes; `uint32_t`)**: The size of the packet.

**type (1 byte; `uint8_t`)**: The type of the packet.

After this, the packet data follows.

The packet types currently implemented as of 23 April 2026 are as follows. Bit 0 of the packet type has special significance; bit 0 is set (or if you like, the packet ID has a bitwise AND with 0x80 applied) to indicate that the originf rom the packet is the server.

| Type                                    | Purpose                            
| --------------------------------------- | ----------------------------------------------------------- |
| `NETMSG_HELLO` (0x0)                    | The client is requesting to log-on                          |
| `NETMSG_WORLD_DOWNLOAD_START` (0x1)     | The client is requesting the current world to be downloaded |
| `NETMSG_WORLD_DOWNLOAD_PACKET` (0x2)    | The client is requesting a new packet of the current world  |


## 3. Connection & Authentication
Connection starts when the server receives a valid connection request from a client. The client's connection request is a message of the type `CLIENT_CONNECT`

## 4. Heartbeat

## 5. Updating the world

## 6. Server RPCs

## 7. Client RPCs

## Appendix A: Protocol Versions

| Protocol version | Game version        | Changes                             |
| ---------------- | ------------------- | ----------------------------------- |
| 1                | Pre-Alpha 4.0/5.0   | No protocol version. Initial        |
| 2                | Pre-Alpha 6.0       | World downloading                   |
| 3                | Pre-Alpha 7.0 (pre) | NetIdentities (edicts)              |
| 4                | Pre-Alpha 7.0       | Usernames, RPCs                     |
| 5                | Pre-Alpha 8.0       | Usernames, RPCs                     |