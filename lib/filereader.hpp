#include <cstdint>
#include <string>
#include <vector>

namespace FileReader {

typedef struct {
  uint64_t timestamp;
  uint64_t x;
  uint64_t y;
  bool res;
} event_t;

std::vector<event_t> read_file(std::string filename);

void print_events(std::vector<event_t>);

}; // namespace FileReader
