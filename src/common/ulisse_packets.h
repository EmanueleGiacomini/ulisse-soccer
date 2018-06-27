#pragma once
#include "packet_header.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_MOTORS 4

// simple macro to initialize a packet
#define INIT_PACKET(var, id)			\
  {						\
    var.header.type=id;				\
    var.header.size=sizeof(var);		\
    var.header.seq=0;				\
    }

#pragma pack(push, 1)
typedef struct {
	uint16_t encoder_position;	// absolute position
	int16_t encoder_speed;		// difference in position between two ticks
	int16_t desidered_speed;	// speed set from pc
} JointInfo;

typedef struct {
	int16_t kp, ki, kd;			// pid parameters * 256
	int16_t max_i;				// max vlaue of the integral term in PID
	int16_t min_pwm, max_pwm;	// minimum and maximum magnitude of values that can be sent as output
	int16_t max_speed; 			// max input value (encoder ticks)
	int16_t slope; 				// max slope for ramp between two cycles (encoder ticks)
} JointParams;

typedef struct{
  int16_t speed;
  uint8_t mode;
}  JointControl;

// sent by the robot when something goes wrong
// the seq of the error packet is set to the incoming packet that triggered the error
typedef struct {	
  PacketHeader header;
  uint8_t   p_type;       // this is the type of the parameter triggering the response
  uint16_t  p_seq;        // this is the seq of the parameter triggering the response
  uint8_t   p_result;     // this is the outcome of the deferred packet handler
} ResponsePacket;
#define RESPONSE_PACKET_ID 0

typedef enum {
  ParamSystem = 0,
  ParamJoints = 1,
  ParamDrive  = 2,
  ParamSonar  = 3
} ParamType;

typedef enum {
  ParamRequest = 0,
  ParamLoad = 1,
  ParamSave  = 2
} ParamAction;

//! sent from the pc to the robot causes
//! the robot to send a ParamPacket to the PC (with the same seq)
typedef struct {
  PacketHeader header;

  //0: send current params
  //1: load params from eeprom and send them
  //2: write current params to eeprom, read them and send them
  uint8_t action;

  // identifies the parameter class on which command will be executed
  // 0: system
  // 1: joints
  // 2: drive
  uint8_t param_type;
} ParamControlPacket;
#define PARAM_CONTROL_PACKET_ID 1

typedef struct  {
  PacketHeader header;
  uint16_t rx_buffer_size;
  uint16_t rx_packets;
  uint16_t rx_packet_errors;
  uint16_t tx_buffer_size;
  uint16_t tx_packets;
  uint16_t tx_packet_errors;
  uint16_t battery_level;
  int16_t watchdog_count;
  uint16_t rx_seq;
  uint8_t rx_packet_queue;
  uint32_t idle_cycles;
} SystemStatusPacket;
#define SYSTEM_STATUS_PACKET_ID 2

typedef struct  {
  PacketHeader header;
  JointInfo joints[NUM_MOTORS];  // the joint states
} JointStatusPacket;
#define JOINT_STATUS_PACKET_ID 4

typedef struct {
  PacketHeader header;
  JointControl joints[NUM_MOTORS];
} JointControlPacket;
#define JOINT_CONTROL_PACKET_ID 5

typedef struct {
  PacketHeader header;
  JointParams joints[NUM_MOTORS];
} JointParamPacket;
#define JOINT_PARAM_PACKET_ID 6

typedef PacketHeader EndEpochPacket;
#define END_EPOCH_PACKET_ID 10


#ifdef __cplusplus
}
#endif
