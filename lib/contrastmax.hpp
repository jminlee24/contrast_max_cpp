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
  std::vector<uint64_t> imagedata;
} image_t;

std::vector<event_t> warp_events(std::vector<event_t>, double, double[]);
event_t warp_event(event_t, double, double[]);

std::vector<std::vector<uint64_t>> create_image(filedata_t);
void write_image(std::vector<std::vector<uint64_t>>);
void write_image(std::vector<uint64_t>, uint64_t width, uint64_t height);

std::vector<uint64_t> flatten_vec(std::vector<std::vector<uint64_t>>);

double calculate_variance();

Eigen::Matrix3d get_translation_matrix(double x, double y);
Eigen::Matrix3d get_rotation_matrix(double roll, double pitch, double yaw);

} // namespace ContrastMax
