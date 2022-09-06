#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"
#include "image.h"


struct FileStats {
  std::string imgname_;
  size_t dataSizeRaw_;
  size_t dataSizeEnc_;
  double compressionRate_;
  double encodeTime_;
  double decodeTime_;
  double encodeSpeed_;
  double decodeSpeed_;

  anslib::RawImage getTestImg(std::string filePath);
  FileStats(anslib::RawImage imgRaw);
  FileStats(std::string filePath);
};

std::vector<std::string> listAllImgsInDir(const std::string dir_path, const std::string postfix);

double getEncodeTime(const anslib::RawImage &img);

double getDecodeTime(const anslib::CompImage &img);

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs, const char* resultsFileName);

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs);

inline std::ostream &operator<<(std::ostream &os, struct FileStats fs) {
  os << "++++++\n"
     << fs.imgname_ << '\n'
     << "dataSizeRaw_:\n\t" << std::right << fs.dataSizeRaw_ / 1024 << "KB\n"
     << "dataSizeEnc_:\n\t" << std::right << fs.dataSizeEnc_ / 1024 << "KB\n"
     << "compressionRate_:\n\t" << std::right << fs.compressionRate_ << '\n'
     << "encodeTime_:\n\t" << std::right << fs.encodeTime_ << "s\n"
     << "decodeTime_:\n\t" << std::right << fs.decodeTime_ << "s\n"
     << "encodeSpeed_:\n\t" << std::right << fs.encodeSpeed_ << "MB/s\n"
     << "decodeSpeed_:\n\t" << std::right << fs.decodeSpeed_ << "MB/s\n";
  return os;
}