#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>

#include <iostream>
#include <vector>

int main() {
  FileReader::filedata_t fileData =
      FileReader::read_file("../data/recording.raw");

  std::cout << fileData.metadata.height << std::endl;
  std::cout << fileData.metadata.width << std::endl;

  std::vector<FileReader::event_t> events =
      FileReader::filter_event_time(fileData.events, 1000, 2000);

  for (FileReader::event_t event : events) {
    std::cout << "first" << std::endl;
    std::cout << "x: " << event.x << std::endl;
    std::cout << "y: " << event.y << std::endl;
    std::cout << "timestamp: " << event.timestamp << std::endl;
    std::cout << "polarity: " << event.pol << std::endl;

    double theta[] = {0.0, 1.0, 0.0};
    event = ContrastMax::warp_event(event, 1, theta);
    std::cout << "next" << std::endl;

    std::cout << "x: " << event.x << std::endl;
    std::cout << "y: " << event.y << std::endl;
    std::cout << "timestamp: " << event.timestamp << std::endl;
    std::cout << "polarity: " << event.pol << std::endl;
  }

  std::cout << events.size() << std::endl;

  return 0;
}
