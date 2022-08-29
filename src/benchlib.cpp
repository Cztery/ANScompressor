#include "benchlib.h"

#include <algorithm>
#include <chrono>
#include <fstream>

#include "bmplib.h"
#include "common.h"
#include "compressor.h"
#include "image.h"
#include "ppmlib.h"

std::vector<std::string> listAllImgsInDir(const std::string dir_path,
                                          const std::string postfix) {
  std::vector<std::string> vec_of_filenames;
  for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
    const std::string path = entry.path();
    if (path.find(postfix) == (path.size() - postfix.size())) {
      vec_of_filenames.push_back(entry.path());
    }
  }
  return vec_of_filenames;
}

/*inline*/ void compressPlane(const std::vector<anslib::AnsSymbol> &inData,
                              std::vector<anslib::AnsCountsType> &symCounts,
                              std::vector<uint8_t> &outData) {
  anslib::AnsEncoder encoder(inData, symCounts);
  outData = encoder.encodePlane();
}

/*inline*/ void decompressPlane(
    const std::vector<anslib::AnsCountsType> &sym_counts,
    std::vector<uint8_t> data) {
  anslib::AnsDecoder decoder(sym_counts, data);
}

/*inline*/ void compressImage(anslib::Image inImg) {
  for (auto &plane : inImg.dataPlanes_) {
    std::vector<anslib::AnsCountsType> symCounts;
    compressPlane(plane, symCounts, plane);
    inImg.symCountsIfCompressed_.push_back(symCounts);
  }
}

void compressImage(anslib::Image inImg, anslib::Image &outImg) {
  for (auto &plane : inImg.dataPlanes_) {
    std::vector<anslib::AnsCountsType> symCounts;
    compressPlane(plane, symCounts, plane);
    inImg.symCountsIfCompressed_.push_back(symCounts);
  }
  outImg = inImg;
}

/*inline*/ void decompressImage(anslib::Image inImg) {
  for (size_t i = 0; i < inImg.dataPlanes_.size(); ++i) {
    decompressPlane(inImg.symCountsIfCompressed_.at(i),
                    inImg.dataPlanes_.at(i));
  }
}

float calcCompressionRate(std::vector<uint8_t> &raw,
                          std::vector<uint8_t> &compressed) {
  return (float)raw.size() / (float)compressed.size();
}

double getEncodeTime(anslib::Image img) {
  std::cout << "Counting encode time...\n";
  std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
  compressImage(img);
  std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double>(t2 - t1).count();
}

double getDecodeTime(anslib::Image img) {
  std::cout << "Counting decode time...\n";
  std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
  decompressImage(img);
  std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double>(t2 - t1).count();
}

size_t FileStats::bytesSizeOfImage(const anslib::Image &img) {
    size_t byteSize = 0;
    for (auto plane : img.dataPlanes_) {
      byteSize += plane.size() * sizeof(decltype(plane.back()));
    }
    byteSize += img.symCountsIfCompressed_.size() *
                sizeof(decltype(img.symCountsIfCompressed_.back()));
    return byteSize;
  }

FileStats::FileStats(std::string filePath) {
    filename_ = filePath.substr(filePath.rfind('/') + 1);
    anslib::Image imgRaw;
    if (filePath.rfind(".bmp") != std::string::npos) {
      anslib::bmplib::BmpImage bmp(filePath.c_str());
      imgRaw = anslib::Image(bmp);
      dataSizeRaw_ = bmp.data.size() * sizeof(decltype(bmp.data.back()));
    } else {
      anslib::ppmlib::PpmImage raw(filePath.c_str());
      imgRaw = anslib::Image(raw.r, raw.g, raw.b, raw.width_, raw.height_);
      dataSizeRaw_ = raw.height_ * raw.width_ * sizeof(decltype(raw.r.back()));
    }

    dataSizeRaw_ = 0;
    for (const auto &plane : imgRaw.dataPlanes_) {
      dataSizeRaw_ += plane.size() * sizeof(decltype(plane.back()));
    }

    anslib::Image imgEncoded(imgRaw);
    compressImage(imgRaw, imgEncoded);

    dataSizeEnc_ = bytesSizeOfImage(imgEncoded);
    compressionRate_ = (double)dataSizeRaw_ / (double)dataSizeEnc_;

    encodeTime_ = getEncodeTime(imgRaw);
    decodeTime_ = getDecodeTime(imgEncoded);

    encodeSpeed_ = dataSizeRaw_ / encodeTime_ / 1048576;
    decodeSpeed_ = dataSizeRaw_ / decodeTime_ / 1048576;
  }

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs, const char* resultsFileName) {
  std::stringstream buffer;
  std::ofstream csvFile(resultsFileName, std::ios_base::in);
  if (!csvFile.good()) {  // if writing to a new file, add header
    buffer << "Filename;DataSizeRaw;DataSizeEnc;CompressionRate;EncodeTime;"
              "DecodeTime;EncodeSpeed;DecodeSpeed\n";
  }
  csvFile.close();
  csvFile.open(resultsFileName, std::ios_base::app | std::ios_base::out);
  for (const auto &fs : vfs) {
    buffer << fs.filename_ << ';' << fs.dataSizeRaw_ << ';' << fs.dataSizeEnc_
           << ';' << fs.compressionRate_ << ';' << fs.encodeTime_ << ';'
           << fs.decodeTime_ << ';' << fs.encodeSpeed_ << ';' << fs.decodeSpeed_
           << '\n';
  }
  csvFile << buffer.rdbuf();
  csvFile.close();
}

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs) {
  writeBenchResultsToCSV(vfs, "compresults.csv");
}