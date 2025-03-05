#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  FileReader::filedata_t fileData =
      FileReader::read_file("../data/zybo_recording1.raw");

  int width = fileData.metadata.width;
  int height = fileData.metadata.height;

  // take first 1000 ms slice
  fileData.events = FileReader::filter_event_time(
      fileData.events, fileData.events[0].timestamp,
      fileData.events[0].timestamp + 1000000);

  ContrastMax::image_t prev_image =
      ContrastMax::create_image(fileData.events, width, height);

  ContrastMax::write_image(prev_image, "prev.pgm");

  auto start = std::chrono::high_resolution_clock::now();

  Eigen::Vector3d val;
  if (argc > 1 && std::string(argv[1]) == std::string("--blur")) {
    std::cout << "Running blur optimization" << std::endl;
    val = ContrastMax::maximize(fileData);
  } else {
    std::cout << "Running standard optimization" << std::endl;
    val = ContrastMax::maximize(fileData);
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::vector<ContrastMax::event_t> warped_events =
      ContrastMax::warp_events(fileData.events, val);

  ContrastMax::image_t image =
      ContrastMax::create_image(warped_events, width, height);
  ContrastMax::write_image(image, "warped.pgm");

  std::cout << "Single pass: " << elapsed.count() << std::endl;
  std::cout << val << std::endl;
  std::cout << "Run on " << fileData.events.size() << " events." << std::endl;

  return 0;
}
