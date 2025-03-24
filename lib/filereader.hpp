#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace FileReader {

typedef struct {
  uint64_t timestamp;
  uint32_t x;
  uint32_t y;
  uint16_t pol;
} event_t;

typedef struct {
  uint32_t width;
  uint32_t height;
  uint64_t max_time;
  uint64_t min_time;
} metadata_t;

typedef struct {
  std::vector<event_t> events;
  metadata_t metadata;
} filedata_t;

filedata_t read_file(std::string filename);

std::vector<event_t> filter_event_time(std::vector<event_t>, uint64_t,
                                       uint64_t);

void print_events(std::vector<event_t>);

}; // namespace FileReader

namespace Evt3 {
enum class EventTypes : uint8_t {
  EVT_ADDR_Y = 0x0,
  EVT_ADDR_X = 0x2,
  VECT_BASE_X = 0x3,
  VECT_12 = 0x4,
  VECT_8 = 0x5,
  EVT_TIME_LOW = 0x6,
  EVT_TIME_HIGH = 0x8,
  EXT_TRIGGER = 0xA
};

struct RawEvent {
  uint16_t pad : 12; // Padding
  uint16_t type : 4; // Event type
};

struct RawEventTime {
  uint16_t time : 12;
  uint16_t type : 4; // Event type : EventTypes::EVT_TIME_LOW OR
                     // EventTypes::EVT_TIME_HIGH
};

struct RawEventXAddr {
  uint16_t x : 11;   // Pixel X coordinate
  uint16_t pol : 1;  // Event polarity:
                     // '0': decrease in illumination
                     // '1': increase in illumination
  uint16_t type : 4; // Event type : EventTypes::EVT_ADDR_X
};

struct RawEventVect12 {
  uint16_t valid : 12; // Encodes the validity of the events in the vector :
                       // foreach i in 0 to 11
                       //   if valid[i] is '1'
                       //      valid event at X = VECT_BASE_X.x + i
  uint16_t type : 4;   // Event type : EventTypes::VECT_12
};

struct RawEventVect8 {
  uint16_t valid : 8; // Encodes the validity of the events in the vector :
                      // foreach i in  0 to 7
                      //   if valid[i] is '1'
                      //      valid event at X = VECT_BASE_X.x + i
  uint16_t unused : 4;
  uint16_t type : 4; // Event type : EventTypes::VECT_8
};

struct RawEventY {
  uint16_t y : 11;   // Pixel Y coordinate
  uint16_t orig : 1; // Identifies the System Type:
                     // '0': Master Camera (Left Camera in Stereo Systems)
                     // '1': Slave Camera (Right Camera in Stereo Systems)
  uint16_t type : 4; // Event type : EventTypes::EVT_ADDR_Y
};

struct RawEventXBase {
  uint16_t x : 11;   // Pixel X coordinate
  uint16_t pol : 1;  // Event polarity:
                     // '0': decrease in illumination
                     // '1': increase in illumination
  uint16_t type : 4; // Event type : EventTypes::VECT_BASE_X
};

struct RawEventExtTrigger {
  uint16_t value : 1; // Trigger current value (edge polarity):
                      // - '0' (falling edge);
                      // - '1' (rising edge).
  uint16_t unused : 7;
  uint16_t id : 4;   // Trigger channel ID.
  uint16_t type : 4; // Event type : EventTypes::EXT_TRIGGER
};

using timestamp_t = uint64_t; // timestamp in microseconds
} // namespace Evt3
