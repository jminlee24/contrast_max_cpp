#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>
#include <fstream>
#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using json = nlohmann::json;

int main() {

  std::ifstream f("../config.json");
  json config = json::parse(f);

  FileReader::filedata_t fileData = FileReader::read_file(config["filepath"]);

  int width = fileData.metadata.width;
  int height = fileData.metadata.height;

  // take first 1000 ms slice
  fileData.events = FileReader::filter_event_time(
      fileData.events, fileData.events[0].timestamp,
      fileData.events[0].timestamp + 1000000);

  auto start = std::chrono::high_resolution_clock::now();

  // check if we want to run the maximization with a gaussian blur
  // with sigma = 2 * sqrt(variance)

  // runmax is a function pointer
  Eigen::Vector3d (*runmax)(FileReader::filedata_t);
  Eigen::Vector3d val;
  if (config["blur"]) {
    std::cout << "Running the optimization with a gaussian blur" << std::endl;
    runmax = ContrastMax::maximize_blur;
  } else {
    std::cout << "Running the standard optimization" << std::endl;
    runmax = ContrastMax::maximize;
  }
  val = runmax(fileData);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  // if we want to show some images of the warped events and the previous image
  if (config["generate_images"]) {
    std::cout << "Generate previous image at prev.pgm, and the warped image at "
                 "warped.pgm \n";

    ContrastMax::image_t prev_image =
        ContrastMax::create_image(fileData.events, width, height);

    std::vector<ContrastMax::event_t> warped_events =
        ContrastMax::warp_events(fileData.events, val);

    ContrastMax::image_t image =
        ContrastMax::create_image(warped_events, width, height);

    ContrastMax::write_image(prev_image, "prev.pgm");
    ContrastMax::write_image(image, "warped.pgm");
  }
  std::cout << "Single pass: " << elapsed.count() << " s" << std::endl;
  std::cout << val << std::endl;
  std::cout << "Run on " << fileData.events.size() << " events" << std::endl;

  return 0;
}
