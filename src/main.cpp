#define OPTIM_ENABLE_EIGEN_WRAPPERS
#include "contrastmax.hpp"
#include "filereader.hpp"
#include "optim/optim.hpp"
#include <Eigen/Dense>

#include <iostream>
#include <vector>

int main() {
  FileReader::filedata_t fileData =
      FileReader::read_file("../data/recording.raw");

  int width = fileData.metadata.width;
  int height = fileData.metadata.height;

  std::vector<FileReader::event_t> events =
      FileReader::filter_event_time(fileData.events, 1000, 1000000);

  ContrastMax::image_t image =
      ContrastMax::create_image(fileData.events, width, height);
  ContrastMax::blur_image(image, .0);

  auto start = std::chrono::high_resolution_clock::now();

  ContrastMax::singlepass(fileData, Eigen::Vector3d(0, 0, 0));

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  ContrastMax::write_image(image);

  std::cout << "Single pass: " << elapsed.count() << std::endl;
  std::cout << events.size() << std::endl;

  return 0;
}
