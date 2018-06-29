#pragma once
#include "ulisse_client.h"
#include "../common/ulisse_packets.h"
#include "../common/packet_operations.h"
#include <semaphore.h>

#ifndef _cplusplus
extern "C" {
#endif

  // the client
  extern struct UlisseClient* client;

  // semaphore_update (?)
  extern sem_t state_sem;
  extern int state_sem_init;

  extern sem_t param_sem;
  extern int param_sem_init;

  // is the system running
  extern int run;

  // result of the last packet operation
  extern PacketStatus op_result;

  // variables used by the shell thread to read values
  extern SystemStatusPacket system_status;
  extern JointStatusPacket joint_status;
  extern JointParamPacket joint_params;
  extern JointControlPacket joint_control;
  extern ParamControlPacket param_control;
  extern ResponsePacket response;
  extern EndEpochPacket end_epoch;

#ifndef _cplusplus
}
#endif