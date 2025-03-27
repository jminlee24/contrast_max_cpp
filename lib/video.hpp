#pragma once
#include "contrastmax.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <vector>

namespace Video {
using namespace ContrastMax;

void generate_video(std::vector<event_t>, std::vector<Eigen::Vector3d>,
                    uint64_t);
} // namespace Video
