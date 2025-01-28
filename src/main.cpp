#include "filereader.hpp"
#include <iostream>
#include <vector>

int main() {
  std::vector<FileReader::event_t> events = FileReader::read_file("../data/recording.raw");

  for (FileReader::event_t event : events) {
    std::cout << event.timestamp << '\n';
  }

  return 0;
}
