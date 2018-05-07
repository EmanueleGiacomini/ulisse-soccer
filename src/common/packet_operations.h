#pragma once
#include "packet_header.h"

struct PacketOperations;
struct PacketHandler;

typedef PacketStatus  (*PacketFn)(PacketHeader*, void* args);
typedef PacketHeader*  (*PacketGenFn)(PacketType type,
				      PacketSize size,
				      void* args);

typedef struct PacketOperations {
  PacketType type;
  PacketSize size; // expected size of the payload, -1 if error;
  PacketGenFn initialize_buffer_fn;
  void* initialize_buffer_args;
  PacketFn on_receive_fn;
  void* on_receive_args;
} PacketOperations;