#include "filereader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace FileReader {

filedata_t read_file(std::string filename) {

  filedata_t filedata;

  std::vector<event_t> events = {};
  metadata_t metadata;

  std::ifstream input_file(filename, std::ios::in | std::ios::binary);

  if (!input_file.is_open()) {
    std::cerr << "Could not open file: " << filename << "\n";
    throw std::invalid_argument("Could not open input file");
  }

  // get meta data / read header

  while (input_file.peek() == '%') {
    std::string header_line;
    std::getline(input_file, header_line);
    std::cout << header_line << std::endl;
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

      std::cout << "hello" << "\n";
    }
  }

  if (metadata.width < 0 || metadata.height < 0) {
    metadata.width = 1280;
    metadata.height = 720;
  }

  // decode file
  const auto tp_start = std::chrono::system_clock::now();

  // Vector where we'll read the raw data
  static constexpr uint32_t WORDS_TO_READ =
      1000000; // Number of words to read at a time

  // State variables needed for decoding
  bool first_time_base_set = false;
  Evt3::timestamp_t current_time_base = 0; // time high bits
  Evt3::timestamp_t current_time_low = 0;
  Evt3::timestamp_t current_time = 0;

  uint16_t current_ev_addr_y = 0;
  uint16_t current_base_x = 0;
  uint16_t current_polarity = 0;
  unsigned int n_time_high_loop = 0; // Counter of the time high loops

  filedata.metadata = metadata;
  filedata.events = events;

  return filedata;
}

void print_events(std::vector<event_t> events) {
  for (event_t event : events) {
    std::cout << event.timestamp << '\n';
  }
}
} // namespace FileReader
