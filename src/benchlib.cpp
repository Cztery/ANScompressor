#include "benchlib.h"

#include <algorithm>
#include <chrono>
#include <fstream>

#include "bmplib.h"
#include "common.h"
#include "compressor.h"
#include "image.h"
#include "ppmlib.h"

void listAllImgsInDir(const char *dir_path, const char *postfix,
                      std::vector<std::string> &list_to_append) {
  for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
    const std::string path = entry.path();
    if (path.find(postfix) != std::string::npos) {
      list_to_append.push_back(entry.path());
    }
  }
}

double getEncodeTime(const anslib::RawImage &img) {
  anslib::CompImage resultImg;
  std::cout << "Counting encode time...\n";
  std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
  anslib::AnsEncoder::compressImage(img, resultImg);
  std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double>(t2 - t1).count();
}

double getDecodeTime(const anslib::CompImage &img) {
  anslib::RawImage resultImg;
  std::cout << "Counting decode time...\n";
  std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
  anslib::AnsDecoder::decompressImage(img, resultImg);
  std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double>(t2 - t1).count();
}

anslib::RawImage FileStats::getTestImg(std::string filePath) {
  anslib::RawImage imgRaw;
  if (filePath.rfind(".bmp") != std::string::npos) {
    anslib::bmplib::BmpImage bmp(filePath.c_str());
    imgRaw = anslib::RawImage(bmp);
  } else {
    anslib::ppmlib::PpmImage raw(filePath.c_str());
    imgRaw = anslib::RawImage(raw.r, raw.g, raw.b, raw.width_, raw.height_);
  }
  return imgRaw;
}

FileStats::FileStats(anslib::RawImage imgRaw, std::string imgname) {
  anslib::CompImage imgEncoded;
  anslib::AnsEncoder::compressImage(imgRaw, imgEncoded);

  imgname_ = imgname;
  dataSizeRaw_ = imgRaw.bytesSizeOfImage();
  dataSizeEnc_ = imgEncoded.bytesSizeOfImage();
  compressionRate_ = (double)dataSizeRaw_ / (double)dataSizeEnc_;

  encodeTime_ = getEncodeTime(imgRaw);
  decodeTime_ = getDecodeTime(imgEncoded);

  encodeSpeed_ = dataSizeRaw_ / encodeTime_ / 1048576;
  decodeSpeed_ = dataSizeRaw_ / decodeTime_ / 1048576;
  probBits_ = anslib::PROB_BITS;
  chunkSize_ = imgRaw.chunkWidth_;
}

FileStats::FileStats(std::string filePath) : FileStats(getTestImg(filePath), filePath.substr(filePath.rfind('/') + 1)) {}

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs,
                            const char *resultsFileName) {
  std::stringstream buffer;
  std::ofstream csvFile(resultsFileName, std::ios_base::in);
  if (!csvFile.good()) {  // if writing to a new file, add header
    buffer << "Filename;DataSizeRaw;DataSizeEnc;CompressionRate;EncodeTime;"
              "DecodeTime;EncodeSpeed;DecodeSpeed;probBits;chunkSize\n";
  }
  csvFile.close();
  csvFile.open(resultsFileName, std::ios_base::app | std::ios_base::out);
  for (const auto &fs : vfs) {
    buffer << fs.imgname_ << ';' << fs.dataSizeRaw_ << ';' << fs.dataSizeEnc_
           << ';' << fs.compressionRate_ << ';' << fs.encodeTime_ << ';'
           << fs.decodeTime_ << ';' << fs.encodeSpeed_ << ';' << fs.decodeSpeed_
           << fs.probBits_ << ';' << fs.chunkSize_ << '\n';
  }
  csvFile << buffer.rdbuf();
  csvFile.close();
}

void writeBenchResultsToJSON(const std::vector<FileStats> &vfs,
                            const char *resultsFileName) {
  std::stringstream buffer;
  std::ofstream csvFile(resultsFileName, std::ios_base::out);
  buffer << "[\n";
  for (auto i = vfs.size(); const auto &fs : vfs) {
    buffer << "{\n"
           << "\"imgname\": \""   << fs.imgname_      << "\",\n"
           << "\"dataSizeRaw\": " << fs.dataSizeRaw_  << ",\n"
           << "\"dataSizeEnc\": " << fs.dataSizeEnc_  << ",\n"
           << "\"compressionRate\": " << fs.compressionRate_  << ",\n"
           << "\"encodeTime\": "  << fs.encodeTime_ << ",\n"
           << "\"decodeTime\": "  << fs.decodeTime_ << ",\n"
           << "\"encodeSpeed\": "  << fs.encodeSpeed_ << ",\n"
           << "\"decodeSpeed\": "  << fs.decodeSpeed_ << ",\n"
           << "\"probBits\": "  << fs.probBits_ << ",\n"
           << "\"chunkSize\": " << fs.chunkSize_ << "\n"
           << "}";
    if (--i == 0) {
      buffer << "\n";
    } else {
      buffer << ",\n";
    }
  }
  buffer << "]"; 
  csvFile << buffer.rdbuf();
  csvFile.close();
}

void writeBenchResultsToCSV(const std::vector<FileStats> &vfs) {
  writeBenchResultsToCSV(vfs, "compresults.csv");
}