#include "benchlib.h"

int main(int argc, char* argv[]) {
  std::vector<FileStats> encodeStats;

  std::vector<std::string> testImgs;
  std::string filename = CMAKE_SOURCE_DIR "/test_images/A2/e50_a-1200-8.ppm";

  std::cout << "Processing " << filename << '\n';
  FileStats fs(filename);

  std::cout << fs;
}