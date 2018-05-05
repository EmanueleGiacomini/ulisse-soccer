#include "ulisse_print_packets.h"
#include <stdio.h>

int PacketHeader_print(char* buf, PacketHeader* h) {
    int write_bytes = sprintf(buf, "Header: [PacketType]: %d, [PacketSize]: %d, [packetSeq]: %d\n",
                              h->type,
                              h->size,
                              h->seq);
    return write_bytes;
}

void ParamControlPacket_print(char* buf, ParamControlPacket* pcp) {
    int index = PacketHeader_print(buf, &(pcp->header));
    sprintf(buf+index, "ParamControlPacket: [action]: %d, [param_type]: %d\n",
            pcp->action,
            pcp->param_type);
}