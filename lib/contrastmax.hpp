#pragma once
#include "filereader.hpp"
#include <Eigen/Dense>
#include <cstdint>
#include <vector>

namespace ContrastMax {
using namespace FileReader;

typedef struct {
  uint64_t width;
  uint64_t height;
  uint64_t max;
  uint64_t num_events;
  std::vector<uint64_t> imagedata;
} image_t;

std::vector<event_t> warp_events(std::vector<event_t>, double[]);
event_t warp_event(event_t, double[]);

image_t create_image(std::vector<event_t>, int width, int height);
void blur_image(image_t, float);
void write_image(std::vector<uint64_t>, uint64_t width, uint64_t height);
void write_image(image_t);

std::vector<uint64_t> flatten_vec(std::vector<std::vector<uint64_t>>);

double calculate_variance(image_t);
double singlepass(std::vector<event_t>, double[]);

Eigen::Matrix3d get_translation_matrix(double x, double y);
Eigen::Matrix3d get_rotation_matrix(double roll, double pitch, double yaw);

} // namespace ContrastMax
