#include "common/ulisse_packets.h"

char buf[256];
uint8_t read_bytes = 0;
uint8_t write_bytes = 0;
bool echo_flag = 0;

ParamControlPacket pcp = {0};

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

void setup() {
  Serial.begin(115200);
  //delay(100);
  //Serial.println("Server is in echo mode\nit will print back every message sent");
  //Serial.println(sizeof(ParamControlPacket));

}

void loop() {
  /* ECHO SERVER */
  /*
  if(msg_len > 0) {
    while(write_bytes < read_bytes) {
      Serial.write(buf[write_bytes++]);
    }
    msg_len = 0;
    read_bytes = 0;
    write_bytes = 0;
  }
  */

  if(echo_flag) {
    echo_flag=0;
    pcp.action = 1;
    ParamControlPacket_print(buf, &pcp);
    Serial.write(buf);
    Serial.flush();
    read_bytes = 0;
  }
}

/* ECHO SERVER */
/*
void serialEvent() {
  while (Serial.available()) {
    // bytes are present in the Serial buffer
    char incoming_byte = Serial.read(); 
    buf[read_bytes++] = incoming_byte;
    if(incoming_byte == 0) break;    
  }
  msg_len = read_bytes;
}
*/


/* PACKET RECIVER */
void serialEvent() {
  while (Serial.available()) {
    // bytes are present in the Serial buffer
    char incoming_byte = Serial.read(); 
    buf[read_bytes++] = incoming_byte;
    if(incoming_byte == 0) break;    
  //  else Serial.print(incoming_byte);
  }
  if(read_bytes == sizeof(ParamControlPacket)) {
    memcpy(&pcp, (void*)buf, sizeof(ParamControlPacket));
    echo_flag=1;  // flag to echo back to host the pcp print
  }

}

