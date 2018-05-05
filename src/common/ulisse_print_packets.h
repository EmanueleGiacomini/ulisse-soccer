#pragma once
#include "ulisse_packets.h"


#ifdef __cplusplus 
extern "C" {
#endif

int PacketHeader_print(char* buf, PacketHeader* h);

void ParamControlPacket_print(char* buf, ParamControlPacket* pcp);

#ifdef __cplusplus 
}
#endif
