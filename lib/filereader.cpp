#include "filereader.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace FileReader {

std::vector<event_t> read_file(std::string filename) {
  std::vector<event_t> res = {{1, 2, 3, 0}};
  return res;
}

void print_events(std::vector<event_t> events) {
  for (event_t event : events) {
    std::cout << event.timestamp << '\n';
  }
}
} // namespace FileReader
