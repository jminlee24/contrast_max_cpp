#include <cstdint>
#include <vector>
#include <string>

class FileReader {
  public: 
    typedef struct event_t{
      uint64_t  time;
      uint32_t x;
      uint32_t y;
      bool polarity;
    } event_t;

    event_t events[];
    
    std::vector<event_t> read_file(std::string filename);

};
