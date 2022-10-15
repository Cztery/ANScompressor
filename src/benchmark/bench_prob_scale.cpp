#include "benchlib.h"
#include "common.h"
#include "image.h"

int main(int argc, char *argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/",
                   ".bmp", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A1/", ".ppm", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A2/", ".ppm", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/FASTCOMPRESSION_COM/", ".ppm", testImgs);

  for (auto imgPath : testImgs) {
    for (uint32_t prob_bits = 8; prob_bits < 15; ++prob_bits) {
      anslib::PROB_BITS = prob_bits;
      anslib::PROB_SCALE = 1u << prob_bits;
      std::cout << "Processing for prob_scale = " << anslib::PROB_SCALE << '\n';
      FileStats fs(imgPath);
      encodeStats.push_back(fs);
      std::cout << encodeStats.back();
    }
    // if(testImgs.at(7) == imgPath) break;
  }

  std::string filename;
  if (argc == 2) {
    filename = argv[1];
  } else {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M");
    filename = ss.str();
  }
  writeBenchResultsToCSV(encodeStats, std::string(filename + ".csv").c_str());
  writeBenchResultsToJSON(encodeStats, std::string(filename + ".json").c_str());
}