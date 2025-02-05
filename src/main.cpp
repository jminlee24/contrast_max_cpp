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

  ContrastMax::image_t imageinfo = ContrastMax::create_image(fileData);
  std::vector<float> blurred_img(imageinfo.width * imageinfo.height, 0);
  std::vector<float> float_img(imageinfo.imagedata.begin(),
                               imageinfo.imagedata.end());

  float *in = &float_img[0];
  float *out = &blurred_img[0];

  Gaussian::fast_gaussian_blur(in, out, imageinfo.width, imageinfo.height, 1.5);

  std::vector<uint64_t> blurred_int(blurred_img.begin(), blurred_img.end());

  ContrastMax::write_image(blurred_int, width, height);

  std::cout << "number of events: " << events.size() << std::endl;

  return 0;
}
