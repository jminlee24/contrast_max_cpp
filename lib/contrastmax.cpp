#include "contrastmax.hpp"
#include "fastgaussianblur.hpp"
#define OPTIM_ENABLE_EIGEN_WRAPPERS
#include "optim/optim.hpp"

#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace ContrastMax {
using namespace FileReader;

image_t create_image(std::vector<event_t> events, int width, int height) {
  image_t imageinfo;

  imageinfo.height = height;
  imageinfo.width = width;
  imageinfo.max = 0;
  imageinfo.num_events = 0;

  std::vector<uint64_t> img(height * width, 0);

  for (event_t event : events) {
    img[event.y * imageinfo.width + event.x] += event.pol;
    uint64_t val = img[event.y * imageinfo.width + event.x];
    imageinfo.num_events++;
    if (val > imageinfo.max) {
      imageinfo.max = val;
    }
  }

  imageinfo.imagedata = img;

  return imageinfo;
};

void blur_image(image_t image, float std) {
  std::vector<float> blurred_img(image.width * image.height, 0);
  std::vector<float> float_img(image.imagedata.begin(), image.imagedata.end());

  float *in = &float_img[0];
  float *out = &blurred_img[0];

  Gaussian::fast_gaussian_blur(in, out, image.width, image.height, std);

  std::vector<uint64_t> blurred_int(blurred_img.begin(), blurred_img.end());

  image.imagedata = blurred_int;
}

void write_image(std::vector<uint64_t> imgdata, uint64_t width,
                 uint64_t height) {

  std::ofstream imageFile("image.pgm");
  if (imageFile.is_open()) {
    imageFile << "P2" << std::endl;
    imageFile << width << " " << height << std::endl;
    imageFile << "10" << std::endl;
    for (uint64_t i = 0; i < height; i++) {
      for (uint64_t j = 0; j < width; j++) {
        imageFile << imgdata[width * i + j] << " ";
      }
      imageFile << std::endl;
    }
    imageFile.close();
  } else {
    std::cerr << "Issue opening image.pgm\n";
  }
  return;
}

void write_image(image_t image) {
  std::vector<uint64_t> imgdata = image.imagedata;
  int width = image.width;
  int height = image.height;

  std::ofstream imageFile("image.pgm");
  if (imageFile.is_open()) {
    imageFile << "P2" << std::endl;
    imageFile << width << " " << height << std::endl;
    imageFile << image.max << std::endl;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        imageFile << imgdata[width * i + j] << " ";
      }
      imageFile << std::endl;
    }
    imageFile.close();
  } else {
    std::cerr << "Issue opening image.pgm\n";
  }
  return;
}

std::vector<uint64_t> flatten_vec(std::vector<std::vector<uint64_t>> vector) {
  std::vector<uint64_t> flat_vec;

  for (auto vec : vector) {
    flat_vec.insert(flat_vec.end(), vec.begin(), vec.end());
  }

  return flat_vec;
}

Eigen::Vector3d maximize(filedata_t filedata) {
  Eigen::VectorXd x0(3);
  x0 << 0, 0, 0;

  bool success = optim::nm(x0, singlepass_optim, &filedata.events);

  return x0;
}

double singlepass_optim(Eigen::VectorXd x0, Eigen::VectorXd *grad_out,
                        void *fd) {
  if (x0.size() != 3) {
    throw std::runtime_error("x0 must have exactly 3 elements");
  }

  Eigen::Vector3d rx0;
  rx0 << x0(0), x0(1), x0(2); // ✅ Assign explicitly

  filedata_t filedata = *((filedata_t *)fd);

  return singlepass(rx0, filedata);
}

double singlepass(Eigen::Vector3d x0, filedata_t filedata) {

  std::vector<event_t> warped_events = warp_events(filedata.events, x0);

  image_t image = create_image(warped_events, filedata.metadata.width,
                               filedata.metadata.height);

  double variance = calculate_variance(image);

  return -variance;
}

double calculate_variance(image_t image) {
  double n_p = image.num_events;
  if (n_p < 1) {
    return 0;
  }
  int size = image.width * image.height;

  double mean = n_p / size;
  double variance = 0;

  for (auto val : image.imagedata) {
    variance += (val - mean) * (val - mean);
  }

  return variance / size;
}

std::vector<event_t> warp_events(std::vector<event_t> events,
                                 Eigen::Vector3d theta) {

  std::vector<event_t> warped_events;
  uint64_t t0 = events[0].timestamp;

  for (event_t event : events) {
    uint64_t t = event.timestamp - t0;

    event_t warped_event = warp_event(event, t, theta);
    if (warped_event.pol != 2) {
      warped_events.push_back(warped_event);
    }
  }

  return warped_events;
}

event_t warp_event(event_t event, uint64_t t, Eigen::Vector3d theta) {
  event_t warped_event;

  double x = event.x;
  double y = event.y;

  Eigen::Vector3d event_vector{{x, y, 1}};

  Eigen::Matrix3d theta_hat = get_rotation_matrix(
      theta.coeff(0) * t, theta.coeff(1) * t, theta.coeff(2) * t);
  Eigen::Matrix3d trans = get_translation_matrix(1, 1);
  Eigen::Matrix3d trans_inv = get_translation_matrix(-1, -1);

  Eigen::Vector3d warped_event_vector =
      trans_inv * theta_hat * trans * event_vector;

  warped_event.x = warped_event_vector.coeff(0);
  warped_event.y = warped_event_vector.coeff(1);
  warped_event.timestamp = warped_event_vector.coeff(2);
  warped_event.pol = event.pol;

  if (warped_event_vector.coeff(0) < 0 || warped_event_vector.coeff(1) < 0) {
    warped_event.pol = 2;
  }
  return warped_event;
}

Eigen::Matrix3d get_translation_matrix(double x, double y) {
  // clang-format off
  Eigen::Matrix3d matrix{
    {1, 0, x}, 
    {0, 1, y},
    {0, 0, 1},
  };
  // clang-format on
  return matrix;
}

Eigen::Matrix3d get_rotation_matrix(double roll, double pitch, double yaw) {
  // clang-format off
  Eigen::Matrix3d yaw_mat{
    {cos(yaw), -sin(yaw), 0}, 
    {sin(yaw),  cos(yaw), 0},
    {0, 0, 1}
  };

  Eigen::Matrix3d pitch_mat{
    { cos(pitch), 0, sin(pitch)}, 
    {0, 1, 0},
    {-sin(pitch), 0, cos(pitch)}
  };

  Eigen::Matrix3d roll_mat{
    {1, 0, 0}, 
    {0, cos(roll), -sin(roll)},
    {0, sin(roll),  cos(roll)}
  };

  // clang-format on
  return yaw_mat * pitch_mat * roll_mat;
}

} // namespace ContrastMax
