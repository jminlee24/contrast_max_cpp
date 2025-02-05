#include "contrastmax.hpp"

#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace ContrastMax {
using namespace FileReader;

image_t create_image(filedata_t filedata) {
  image_t imageinfo;
  std::vector<event_t> events = filedata.events;

  imageinfo.height = filedata.metadata.height;
  imageinfo.width = filedata.metadata.width;

  std::vector<uint64_t> img(filedata.metadata.height * filedata.metadata.width,
                            0);

  for (event_t event : events) {
    img[event.y * imageinfo.width + event.x] += event.pol;
  }
  imageinfo.imagedata = img;

  return imageinfo;
};

void write_image(std::vector<uint64_t> imgdata, uint64_t width,
                 uint64_t height) {

  std::ofstream imageFile("image.pgm");
  if (imageFile.is_open()) {
    imageFile << "P2" << std::endl;
    imageFile << width << " " << height << std::endl;
    imageFile << "10" << std::endl;
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

double calculate_variance(image_t image) { return 1.0; }

std::vector<event_t> warp_events(std::vector<event_t> events, double t,
                                 double theta[]) {

  std::vector<event_t> warped_events;

  for (event_t event : events) {
    event_t warped_event = warp_event(event, t, theta);
    if (warped_event.pol != 2) {
      warped_events.push_back(warped_event);
    }
  }

  return warped_events;
}

event_t warp_event(event_t event, double t, double theta[]) {
  event_t warped_event;

  double x = event.x;
  double y = event.y;
  Eigen::Vector3d event_vector{{x, y, 1}};

  Eigen::Matrix3d theta_hat =
      get_rotation_matrix(theta[0] * t, theta[1] * t, theta[2] * t);
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
