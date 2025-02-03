#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>

#include <cstdint>
#include <iostream>
#include <vector>

int main() {
  FileReader::filedata_t fileData =
      FileReader::read_file("../data/recording.raw");

  std::cout << fileData.metadata.height << std::endl;
  std::cout << fileData.metadata.width << std::endl;

  std::vector<FileReader::event_t> events =
      FileReader::filter_event_time(fileData.events, 1000, 2000);

  std::vector<std::vector<uint64_t>> image =
      ContrastMax::create_image(fileData);
  ContrastMax::write_image(image);

  std::cout << events.size() << std::endl;

  return 0;
}
