#include "benchlib.h"
#include "common.h"
#include "image.h"
#include "ppmlib.h"
#include "matplotlibcpp.h"

#include <set>
#include <map>

namespace plt = matplotlibcpp;

void plotSpeedToProbScale(const std::vector<FileStats> &fss) {
  std::set <int> xBits;
  std::map<std::string, std::map<int, long>> ySpeedsByImg;
  for (const auto &fs : fss) {
    xBits.insert(fs.prob_bits_);
    ySpeedsByImg.try_emplace(fs.imgname_, std::map<int, long>());
    ySpeedsByImg.at(fs.imgname_).try_emplace(fs.prob_bits_, fs.encodeSpeed_ * 1024);
  }

  std::vector<int> x(xBits.begin(), xBits.end());
  std::vector<long> yAvg;
  std::map<int, long> ySpeedsAvg;
  for (const auto &fs : fss) {
    ySpeedsAvg.try_emplace(fs.prob_bits_, 0);
    ySpeedsAvg.at(fs.prob_bits_) += fs.encodeSpeed_ * 1024;
  }
  for (const auto &speed : ySpeedsAvg) {
    yAvg.push_back(speed.second / ySpeedsByImg.size());
  }

  for (auto imgStats : ySpeedsByImg) {
    std::vector <long> y = {};
    for (auto stat : imgStats.second) {
      y.push_back(stat.second);
    }
    plt::plot(x, y, "b-o");
  }
  plt::plot(x, yAvg, "r-o");
  plt::grid(true);
  plt::title("compression speed / freq size 1(B)");
  plt::show(false);
}

void plotCompRateToProbScale(const std::vector<FileStats> &fss) {
  std::set <int> xBits;
  std::map<std::string, std::map<int, long>> yRatesByImg;
  for (const auto &fs : fss) {
    xBits.insert(fs.prob_bits_);
    yRatesByImg.try_emplace(fs.imgname_, std::map<int, long>());
    yRatesByImg.at(fs.imgname_).try_emplace(fs.prob_bits_, fs.compressionRate_ * 1024);
  }

  std::vector<int> x(xBits.begin(), xBits.end());
  std::vector<long> yAvg;
  std::map<int, long> yRatesAvg;
  for (const auto &fs : fss) {
    yRatesAvg.try_emplace(fs.prob_bits_, 0);
    yRatesAvg.at(fs.prob_bits_) += fs.compressionRate_ * 1024;
  }
  for (const auto &rate : yRatesAvg) {
    yAvg.push_back(rate.second / yRatesByImg.size());
  }

  for (auto imgStats : yRatesByImg) {
    std::vector <long> y = {};
    for (auto stat : imgStats.second) {
      y.push_back(stat.second);
    }
    plt::plot(x, y, "b-o");
  }
  plt::plot(x, yAvg, "r-o");
  plt::grid(true);
  plt::title("compression rate / freq size 1(B)");
  plt::show(false);
}

int main(int argc, char* argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/", ".bmp", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A1/", ".ppm", testImgs);
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/A2/", ".ppm", testImgs);

  for ( auto imgPath : testImgs) {
    for (uint32_t prob_bits = 8; prob_bits < 15; ++prob_bits) {
      anslib::PROB_BITS = prob_bits;
      anslib::PROB_SCALE = 1u << prob_bits;
      std::cout << "Processing for prob_scale = " << anslib::PROB_SCALE << '\n';
      FileStats fs(imgPath);
      encodeStats.push_back(fs);
      std::cout << encodeStats.back();
    }
    // if(testImgs.at(3) == imgPath) break;
  }

  std::string csvFilename;
  if (argc == 2) {
    csvFilename = argv[1];
  } else {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M.csv");
    csvFilename = ss.str();
  }
  writeBenchResultsToCSV(encodeStats, csvFilename.c_str());

  plt::figure(1);
  plotSpeedToProbScale(encodeStats);
  plt::figure(2);
  plotCompRateToProbScale(encodeStats);
  plt::show();
}