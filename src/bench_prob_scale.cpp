#include "benchlib.h"
#include "common.h"
#include "image.h"
#include "ppmlib.h"

int main(int argc, char* argv[]) {
  std::vector<FileStats> encodeStats;
  anslib::ppmlib::PpmImage ppm(CMAKE_SOURCE_DIR
               "/test_images/A2/e50_a-1200-8.ppm");
  anslib::RawImage imgRaw(ppm.r, ppm.g, ppm.b, ppm.width_, ppm.height_);

  for (uint32_t prob_bits = 8; prob_bits < 15; ++prob_bits) {
    anslib::PROB_BITS = prob_bits;
    anslib::PROB_SCALE = 1u << prob_bits;
    std::cout << "Processing for prob_scale = " << anslib::PROB_SCALE << '\n';
    FileStats fs(imgRaw);
    encodeStats.push_back(fs);
    std::cout << encodeStats.back();
  }

  std::string filename;
  if (argc == 2) {
    filename = argv[1];
  } else {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M.csv");
    filename = ss.str();
  }
  writeBenchResultsToCSV(encodeStats, filename.c_str());
}