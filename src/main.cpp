#include "contrastmax.hpp"
#include "fastgaussianblur.hpp"
#include "filereader.hpp"
#include <Eigen/Dense>

#include <cstdint>
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
      FileReader::filter_event_time(fileData.events, 1000, 2000);

  std::vector<std::vector<uint64_t>> image =
      ContrastMax::create_image(fileData);
  std::vector<uint64_t> flat_img = ContrastMax::flatten_vec(image);
  std::vector<float> blurred_img(width * height, 0);
  std::vector<float> float_img(flat_img.begin(), flat_img.end());

  float *in = &float_img[0];
  float *out = &blurred_img[0];

  Gaussian::fast_gaussian_blur(in, out, width, height, 1.5);

  std::vector<uint64_t> blurred_int(blurred_img.begin(), blurred_img.end());
  for (const auto &i : blurred_int) {
    std::cout << " " << i << std::endl;
  }

  ContrastMax::write_image(blurred_int, width, height);

  std::cout << "number of events: " << events.size() << std::endl;

  return 0;
}
