#pragma once
#include <stdint.h>

#ifdef __cplusplus 
extern "C" {
#endif

typedef uint8_t PacketType;
typedef uint8_t PacketSize;
typedef uint16_t PacketSeq;

#define PACKET_SIZE_ANY 0xFF
#define PACKET_TYPE_MAX 16
#define PACKET_SIZE_MAX 254

#pragma pack(push, 1)
typedef struct {
	PacketType type;	// type of the packet < PACKET_TYPE_MAX
	PacketSize size;
	PacketSeq seq;
} PacketHeader;
#pragma pack(pop)

#ifdef __cplusplus 
}
#endif
	
