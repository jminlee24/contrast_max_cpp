#include "contrastmax.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>

#include <iostream>
#include <vector>

int main() {
  FileReader::filedata_t fileData =
      FileReader::read_file("../data/recording.raw");

  std::cout << "image height: " << fileData.metadata.height << std::endl;
  std::cout << "image width: " << fileData.metadata.width << std::endl;

  int width = fileData.metadata.width;
  int height = fileData.metadata.height;

  std::vector<FileReader::event_t> events =
      FileReader::filter_event_time(fileData.events, 1000, 1000000);

  ContrastMax::image_t image =
      ContrastMax::create_image(fileData.events, width, height);

  ContrastMax::blur_image(image, .0);
  ContrastMax::write_image(image);

  std::cout << "number of events: " << events.size() << std::endl;
  std::cout << ContrastMax::calculate_variance(image) << std::endl;

  return 0;
}
