#include "benchlib.h"

int main(int argc, char* argv[]) {
  std::vector<FileStats> encodeStats;

  std::vector<std::string> testImgs;
//   testImgs = listAllImgsInDir(
//      CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/", ".bmp");
//   for (const std::string &filename : testImgs) {
//     std::cout << "Processing " << filename << '\n';
//     encodeStats.push_back(FileStats(filename));
//   }

  testImgs = listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A1/", ".ppm");
  for (const std::string &filename : testImgs) {
    std::cout << "Processing " << filename << '\n';
    encodeStats.push_back(FileStats(filename));
  }

  for (auto fs : encodeStats) {
    std::cout << fs;
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