#pragma once
#include "packet_header.h"

typedef enum {
  /*error conditions*/
  UnknownType=-1,
  RxBufferError=-2,
  InvalidSize=-3,
  ChecksumError=-4,
  Unsync=-5,
  PacketTypeOutOfBounds=-6,
  PacketInstallError=-7,
  PacketUninstallError=-8,
  TxBufferFull=-9,
  NoHandler=-10,
  Timeout=-11,
  GenericError=-127,
  
  /*success conditions*/
  Success=0,
  SyncAA=1,
  Sync55=2,
  SyncType=3,
  SyncSize=4,
  SyncPayload=5,
  SyncPayloadComplete=6,
  SyncChecksum=7
} PacketStatus;

typedef PacketStatus (*PacketFn)(PacketHeader* h, void* args);
typedef PacketHeader* (*PacketGenFn)(PacketType type,
           PacketSize size,
           void* args);

typedef struct PacketOperations {
  PacketType type;
  PacketSize size;
  PacketGenFn initialize_buffer_fn;
  void* initialize_buffer_args;
  PacketFn on_recive_fn;
  void* on_recive_args;
}PacketOperations;