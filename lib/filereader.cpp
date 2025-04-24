#include "filereader.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace FileReader {

void write_file_events(std::string filename, filedata_t file) {

  std::ofstream f(filename);

  if (f.is_open()) {
    f << file.metadata.height << " " << file.metadata.width << std::endl;
    f << file.metadata.max_time << std::endl;

    for (auto events : file.events) {
      f << events.x << " " << events.y << " " << events.pol << " "
        << events.timestamp << std::endl;
    }
  }

  return;
}

std::vector<event_t> filter_event_time(std::vector<event_t> events,
                                       uint64_t t_0, uint64_t t_end) {

  if (t_end < t_0) {
    throw std::invalid_argument(
        "t_0 must be greater than 0 and less than t_end");
  }

  std::vector<event_t> filtered_events;
  std::copy_if(events.begin(), events.end(),
               std::back_inserter(filtered_events),
               [t_0, t_end](event_t event) {
                 return event.timestamp < t_end && event.timestamp > t_0;
               });

  return filtered_events;
}

filedata_t read_file(std::string filename) {

  filedata_t filedata;

  std::vector<event_t> events = {};
  metadata_t metadata;

  std::ifstream input_file(filename, std::ios::in | std::ios::binary);
  const auto tp_start = std::chrono::system_clock::now();

  if (!input_file.is_open()) {
    std::cerr << "Could not open file: " << filename << "\n";
    throw std::invalid_argument("Could not open input file");
  }

  // get meta data / read header

  while (input_file.peek() == '%') {
    std::string header_line;
    std::getline(input_file, header_line);
    if (header_line == "\% end") {
      break;
    } else if (header_line.substr(0, 9) == "\% format ") {
      std::istringstream sf(header_line.substr(9));
      std::string format_name;
      std::getline(sf, format_name, ';');
      if (format_name != "EVT3") {
        throw std::invalid_argument("Input file must be in the EVT3 format");
      }
    } else if (header_line.substr(0, 11) == "\% geometry ") {
      std::istringstream sg(header_line.substr(11));
      std::string sw, sh;
      std::getline(sg, sw, 'x');
      std::getline(sg, sh);

      metadata.height = std::stoi(sh);
      metadata.width = std::stoi(sw);
    }
  }

  if (metadata.width == 0 || metadata.height == 0) {
    metadata.width = 1280;
    metadata.height = 720;
  }

  metadata.min_time = UINT64_MAX;
  metadata.max_time = 0;

  // Vector where we'll read the raw data
  static constexpr uint32_t WORDS_TO_READ =
      1000000; // Number of words to read at a time
  std::vector<Evt3::RawEvent> buffer_read(WORDS_TO_READ);

  // State variables needed for decoding
  bool first_time_base_set = false;
  Evt3::timestamp_t current_time_base = 0; // time high bits
  Evt3::timestamp_t current_time_low = 0;
  Evt3::timestamp_t current_time = 0;

  uint16_t current_ev_addr_y = 0;
  uint16_t current_base_x = 0;
  uint16_t current_polarity = 0;
  unsigned int n_time_high_loop = 0; // Counter of the time high loops

  while (input_file) {
    input_file.read(reinterpret_cast<char *>(buffer_read.data()),
                    WORDS_TO_READ * sizeof(Evt3::RawEvent));
    Evt3::RawEvent *current_word = buffer_read.data();

    Evt3::RawEvent *last_word =
        current_word + input_file.gcount() / sizeof(Evt3::RawEvent);

    for (; !first_time_base_set && current_word != last_word; ++current_word) {
      Evt3::EventTypes type = static_cast<Evt3::EventTypes>(current_word->type);
      if (type == Evt3::EventTypes::EVT_TIME_HIGH) {
        Evt3::RawEventTime *ev_timehigh =
            reinterpret_cast<Evt3::RawEventTime *>(current_word);
        current_time_base = (Evt3::timestamp_t(ev_timehigh->time) << 12);
        first_time_base_set = true;
        break;
      }
    }
    for (; current_word != last_word; ++current_word) {
      Evt3::EventTypes type = static_cast<Evt3::EventTypes>(current_word->type);
      switch (type) {
      case Evt3::EventTypes::EVT_ADDR_X: {
        Evt3::RawEventXAddr *ev_addr_x =
            reinterpret_cast<Evt3::RawEventXAddr *>(current_word);
        // We have a new Event CD with
        // x = ev_addr_x->x
        // y = current_ev_addr_y
        // polarity = ev_addr_x->pol
        // time = current_time (in us)
        event_t event;
        event.x = ev_addr_x->x;
        event.y = current_ev_addr_y;
        event.timestamp = current_time;
        event.pol = ev_addr_x->pol;

        events.push_back(event);

        if (event.timestamp < metadata.min_time) {
          metadata.min_time = current_time;
        }
        if (event.timestamp > metadata.max_time) {
          metadata.max_time = current_time;
        }

        break;
      }
      case Evt3::EventTypes::VECT_12: {
        uint16_t end = current_base_x + 12;

        Evt3::RawEventVect12 *ev_vec_12 =
            reinterpret_cast<Evt3::RawEventVect12 *>(current_word);
        uint32_t valid = ev_vec_12->valid;
        for (uint16_t i = current_base_x; i != end; ++i) {
          if (valid & 0x1) {
            // We have a new Event CD with
            // x = i
            // y = current_ev_addr_y
            // polarity = current_polarity
            // time = current_time (in us)
            event_t event;
            event.x = i;
            event.y = current_ev_addr_y;
            event.timestamp = current_time;
            event.pol = current_polarity;

            if (current_time < metadata.min_time) {
              metadata.min_time = current_time;
            }
            if (current_time > metadata.max_time) {
              metadata.max_time = current_time;
            }

            events.push_back(event);
          }
          valid >>= 1;
        }
        current_base_x = end;
        break;
      }
      case Evt3::EventTypes::VECT_8: {
        uint16_t end = current_base_x + 8;

        Evt3::RawEventVect8 *ev_vec_8 =
            reinterpret_cast<Evt3::RawEventVect8 *>(current_word);
        uint32_t valid = ev_vec_8->valid;
        for (uint16_t i = current_base_x; i != end; ++i) {
          if (valid & 0x1) {
            // We have a new Event CD with
            // x = i
            // y = current_ev_addr_y
            // polarity = current_polarity
            // time = current_time (in us)
            event_t event;
            event.x = i;
            event.y = current_ev_addr_y;
            event.timestamp = current_time;
            event.pol = current_polarity;

            if (current_time < metadata.min_time) {
              metadata.min_time = current_time;
            }

            if (current_time > metadata.max_time) {
              metadata.max_time = current_time;
            }

            events.push_back(event);
          }
          valid >>= 1;
        }
        current_base_x = end;
        break;
      }
      case Evt3::EventTypes::EVT_ADDR_Y: {
        Evt3::RawEventY *ev_addr_y =
            reinterpret_cast<Evt3::RawEventY *>(current_word);
        current_ev_addr_y = ev_addr_y->y;
        break;
      }
      case Evt3::EventTypes::VECT_BASE_X: {
        Evt3::RawEventXBase *ev_xbase =
            reinterpret_cast<Evt3::RawEventXBase *>(current_word);
        current_polarity = ev_xbase->pol;
        current_base_x = ev_xbase->x;
        break;
      }
      case Evt3::EventTypes::EVT_TIME_HIGH: {
        // Compute some useful constant variables :
        //
        // -> MaxTimestampBase is the maximum value that the variable
        // current_time_base can have. It corresponds to the case where an event
        // Metavision::Evt3::RawEventTime of type EVT_TIME_HIGH has all the bits
        // of the field "timestamp" (12 bits total) set to 1 (value is (1 << 12)
        // - 1). We then need to shift it by 12 bits because this field
        // represents the most significant bits of the event time base (range 23
        // to 12). See the event description at the beginning of the file.
        //
        // -> TimeLoop is the loop duration (in us) before the time_high value
        // wraps and returns to 0. Its value is MaxTimestampBase + (1 << 12)
        //
        // -> LoopThreshold is a threshold value used to detect if a new value
        // of the time high has decreased because it looped. Theoretically, if
        // the new value of the time high is lower than the last one, then it
        // means that is has looped. In practice, to protect ourselves from a
        // transmission error, we use a threshold value, so that we consider
        // that the time high has looped only if it differs from the last value
        // by a sufficient difference (i.e. greater than the threshold)
        static constexpr Evt3::timestamp_t MaxTimestampBase =
            ((Evt3::timestamp_t(1) << 12) - 1) << 12; // = 16773120us
        static constexpr Evt3::timestamp_t TimeLoop =
            MaxTimestampBase + (1 << 12); // = 16777216us
        static constexpr Evt3::timestamp_t LoopThreshold =
            (10 << 12); // It could be another value too, as long as it is a big
                        // enough value that we can be sure that the time high
                        // looped

        Evt3::RawEventTime *ev_timehigh =
            reinterpret_cast<Evt3::RawEventTime *>(current_word);
        Evt3::timestamp_t new_time_base =
            (Evt3::timestamp_t(ev_timehigh->time) << 12);
        new_time_base += n_time_high_loop * TimeLoop;

        if ((current_time_base > new_time_base) &&
            (current_time_base - new_time_base >=
             MaxTimestampBase - LoopThreshold)) {
          // Time High loop :  we consider that we went in the past because the
          // timestamp looped
          new_time_base += TimeLoop;
          ++n_time_high_loop;
        }

        current_time_base = new_time_base;
        current_time = current_time_base;
        break;
      }
      case ::Evt3::EventTypes::EVT_TIME_LOW: {
        Evt3::RawEventTime *ev_timelow =
            reinterpret_cast<Evt3::RawEventTime *>(current_word);
        current_time_low = ev_timelow->time;
        current_time = current_time_base + current_time_low;
        break;
      }
      case Evt3::EventTypes::EXT_TRIGGER: {
        break;
      }
      default:
        break;
      }
    }
  }
  const auto tp_end = std::chrono::system_clock::now();
  const double duration_s =
      std::chrono::duration_cast<std::chrono::microseconds>(tp_end - tp_start)
          .count() /
      1e6;
  std::cout << "Decoded " << duration_s << " s" << std::endl;

  filedata.metadata = metadata;
  filedata.events = events;

  return filedata;
}

} // namespace FileReader
