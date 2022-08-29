#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"
#include "image.h"


struct FileStats {
  std::string filename_;
  size_t dataSizeRaw_;
  size_t dataSizeEnc_;
  double compressionRate_;
  double encodeTime_;
  double decodeTime_;
  double encodeSpeed_;
  double decodeSpeed_;

  size_t bytesSizeOfImage(const anslib::Image &img);
  FileStats(std::string filePath);
};

std::vector<std::string> listAllImgsInDir(const std::string dir_path, const std::string postfix);

void compressPlane(const std::vector<anslib::AnsSymbol> &inData,
                              std::vector<anslib::AnsCountsType> &symCounts,
                              std::vector<uint8_t> &outData);

void decompressPlane(
    const std::vector<anslib::AnsCountsType> &sym_counts,
    std::vector<uint8_t> data);

void compressImage(anslib::Image inImg);

void compressImage(anslib::Image inImg, anslib::Image &outImg);

void decompressImage(anslib::Image inImg);

float calcCompressionRate(std::vector<uint8_t> &raw,
                          std::vector<uint8_t> &compressed);

double getEncodeTime(anslib::Image img);

double getDecodeTime(anslib::Image img);

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs, const char* resultsFileName);

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs);

inline std::ostream &operator<<(std::ostream &os, struct FileStats fs) {
  os << "++++++\n"
     << fs.filename_ << '\n'
     << "dataSizeRaw_:\n\t" << std::right << fs.dataSizeRaw_ / 1024 << "KB\n"
     << "dataSizeEnc_:\n\t" << std::right << fs.dataSizeEnc_ / 1024 << "KB\n"
     << "compressionRate_:\n\t" << std::right << fs.compressionRate_ << '\n'
     << "encodeTime_:\n\t" << std::right << fs.encodeTime_ << "s\n"
     << "decodeTime_:\n\t" << std::right << fs.decodeTime_ << "s\n"
     << "encodeSpeed_:\n\t" << std::right << fs.encodeSpeed_ << "MB/s\n"
     << "decodeSpeed_:\n\t" << std::right << fs.decodeSpeed_ << "MB/s\n";
  return os;
}