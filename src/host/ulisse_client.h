#pragma once
#include "../common/packet_handler.h"
#include "../common/ulisse_packets.h"

#ifdef __cplusplus
extern "C" {
#endif
struct UlisseClient;

// creates a new ulisse client, opening a serial connection on device at the selected baudrate
struct UlisseClient* UlisseClient_init(const char* device, uint32_t baudrate);

// destroyes a previously created ulisse client
void UlisseClient_destroy(struct UlisseClient* cl);

// sends a packet
// if timeout ==0, the packet sending is deferred
// otherwise it enables a synchronous operation that waits for timeout packets to be received
PacketStatus UlisseClient_sendPacket(struct UlisseClient* cl, PacketHeader* p, int timeout);

//gets a packet from Ulisse client
PacketStatus UlisseClient_get(struct UlisseClient* cl, PacketHeader* dest, PacketType type);

/** High level functionalities, multithreaded */

// flushes the deferred tx queues,
// reads all packets of an epoch (same seq),
// and returns
// call it periodically
PacketStatus UlisseClient_sync(struct UlisseClient* cl, int cycles);

//to be called at the beginning after a few loops of sync
PacketStatus UlisseClient_readConfiguration(struct UlisseClient* cl, int timeout);

PacketStatus UlisseClient_setJointControl(struct UlisseClient* cl,
              uint8_t joint_num,
              uint16_t speed);

#ifdef __cplusplus
}
#endif

