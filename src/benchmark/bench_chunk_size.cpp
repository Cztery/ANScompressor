#include "benchlib.h"
#include "common.h"
#include "image.h"
#include "compressor.h"
#include <exception>

int main(int argc, char *argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/",
                   ".bmp", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A1/", ".ppm", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A2/", ".ppm", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/FASTCOMPRESSION_COM/", ".ppm", testImgs);
  for (auto imgPath : testImgs) {
    for (uint32_t chunk_size = 0; chunk_size <= 512; chunk_size += 64) {
      anslib::RawImage img = FileStats::getTestImg(imgPath);
      
      img.splitIntoChunks(chunk_size);
      anslib::RawImage imgRef = img;
      anslib::CompImage resultImg;
      anslib::AnsEncoder::compressImage(img, resultImg);
      anslib::AnsDecoder::decompressImage(resultImg, img);
      // img.mergeImageChunks();
      std::cout << "Processing " << imgPath.substr(imgPath.rfind('/') + 1) << " for chunk_size = " << chunk_size << '\n';
      for (size_t i = 0; i < img.dataPlanes_.size(); ++i) {
        assert(img.dataPlanes_.at(i).size() == imgRef.dataPlanes_.at(i).size());
        assert(img.dataPlanes_.at(i) == imgRef.dataPlanes_.at(i));
      }
      
      FileStats fs(img, imgPath.substr(imgPath.rfind('/') + 1));
      encodeStats.push_back(fs);
      std::cout << encodeStats.back();
    }
    // if(testImgs.at(3) == imgPath) break;
  }

  std::string resultsFileName;
  if (argc == 2) {
    resultsFileName = argv[1];
  } else {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "cs_%Y-%m-%d_%H-%M");
    resultsFileName = ss.str();
  }
  writeBenchResultsToCSV(encodeStats, std::string(resultsFileName + ".csv").c_str());
  writeBenchResultsToJSON(encodeStats, std::string(resultsFileName + ".json").c_str());
}