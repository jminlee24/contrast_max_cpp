#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using json = nlohmann::json;

#define PBSTR "============================================================"
#define PBWIDTH 60

void printProgress(double percentage) {
  int val = (int)(percentage * 100);
  int lpad = (int)(percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;

  if (val > 100) {
    return;
  }

  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
  if (val == 100) {
    printf("\n");
  }
}

int main() {

  std::ifstream f("../config.json");
  json config = json::parse(f);

  FileReader::filedata_t fileData = FileReader::read_file(config["filepath"]);

  int width = fileData.metadata.width;
  int height = fileData.metadata.height;

  // check if we want to run the maximization with a gaussian blur
  // with sigma = 2 * sqrt(variance)

  // runmax is a function pointer
  Eigen::Vector3d (*runmax)(FileReader::filedata_t);
  if (config["blur"]) {
    std::cout << "Running the optimization with a gaussian blur" << std::endl;
    runmax = ContrastMax::maximize_blur;
  } else {
    std::cout << "Running the standard optimization" << std::endl;
    runmax = ContrastMax::maximize;
  }

  auto start = std::chrono::high_resolution_clock::now();
  uint64_t start_timestamp = fileData.metadata.min_time;
  uint64_t end_timestamp = fileData.metadata.max_time;
  uint64_t window_size = config["timeslice"];

  std::cout << "Start time: " << start_timestamp << std::endl;
  std::cout << "End time: " << end_timestamp << std::endl;

  auto num_slices = (end_timestamp - start_timestamp) / window_size;
  if (num_slices == 0 || config["sliding_window"] == false) {
    num_slices = 1;
    window_size = start_timestamp - end_timestamp;
  }

  std::cout << "Running optimization with " << num_slices << " sliding windows"
            << std::endl;

  // initialize with size
  std::vector<Eigen::Vector3d> res(num_slices);

  uint64_t i = 0;

  do {
    FileReader::filedata_t temp;
    temp.metadata = fileData.metadata;
    temp.events = FileReader::filter_event_time(
        fileData.events, start_timestamp + (i * window_size),
        start_timestamp + (i + 1) * window_size);

    res[i] = runmax(temp);
    i++;
    printProgress(i / (float)num_slices);
  } while (start_timestamp + (i * window_size) < end_timestamp);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  // if we want to show some images of the warped events and the previous image
  if (config["generate_images"]) {
    std::cout
        << "Generated images for the first window at: warped.pgm, and prev.pgm"
        << std::endl;

    FileReader::filedata_t tempData = {
        ContrastMax::filter_event_time(fileData.events, start_timestamp,
                                       start_timestamp + window_size),
        fileData.metadata};

    ContrastMax::image_t prev_image =
        ContrastMax::create_image(tempData.events, width, height);

    std::vector<ContrastMax::event_t> warped_events =
        ContrastMax::warp_events(tempData.events, res[0]);

    ContrastMax::image_t image =
        ContrastMax::create_image(warped_events, width, height);

    ContrastMax::write_image(prev_image, "prev.pgm");
    ContrastMax::write_image(image, "warped.pgm");
  }

  std::cout << "Elapsed Time: " << elapsed.count() << " s" << std::endl;
  std::cout << "Vector: " << res[0] << std::endl;

  return 0;
}
