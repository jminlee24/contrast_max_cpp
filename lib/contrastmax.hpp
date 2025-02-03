#pragma once
#include "filereader.hpp"
#include <Eigen/Dense>
#include <vector>

namespace ContrastMax {
using namespace FileReader;

std::vector<event_t> warp_events(std::vector<event_t>, double, double[]);
event_t warp_event(event_t, double, double[]);

Eigen::Matrix3d get_translation_matrix(double x, double y);
Eigen::Matrix3d get_rotation_matrix(double roll, double pitch, double yaw);

} // namespace ContrastMax
