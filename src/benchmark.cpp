#include "compressor.h"
#include "common.h"
#include "bmplib.h"
#include "benchmark.h"
#include "image.h"

#include <algorithm>
#include <chrono>

std::vector<std::string> listAllBmpsInDir(const std::string dir_path) {
    std::vector<std::string> vec_of_filenames;
    for (const auto & entry : std::filesystem::directory_iterator(dir_path)) {
        const std::string postfix = ".bmp";
        const std::string path = entry.path();
        if(path.find(postfix) == (path.size() - postfix.size())) {
            vec_of_filenames.push_back(entry.path());
        }
    }
    return vec_of_filenames;
}

/*inline*/ void compressPlane(const std::vector<anslib::AnsSymbol> &inData, std::vector<anslib::AnsCountsType> &symCounts, std::vector<uint8_t> &outData) {
    anslib::AnsEncoder encoder(inData, symCounts);
    outData = encoder.encodePlane();
}

/*inline*/ void decompressPlane(const std::vector<anslib::AnsCountsType> &sym_counts, std::vector<uint8_t> data) {
    anslib::AnsDecoder decoder(sym_counts, data);
}

/*inline*/ void compressImage(anslib::Image inImg) {
    for (auto &plane : inImg.dataPlanes) {
        std::vector<anslib::AnsCountsType> symCounts;
        compressPlane(plane, symCounts, plane);
        inImg.symCountsIfCompressed.push_back(symCounts);
    }
}

void compressImage(anslib::Image inImg, anslib::Image &outImg) {
    for (auto &plane : inImg.dataPlanes) {
        std::vector<anslib::AnsCountsType> symCounts;
        compressPlane(plane, symCounts, plane);
        inImg.symCountsIfCompressed.push_back(symCounts);
    }
    outImg = inImg;
}

/*inline*/ void decompressImage(anslib::Image inImg) {
    for (size_t i = 0; i<inImg.dataPlanes.size(); ++i) {
        decompressPlane(inImg.symCountsIfCompressed.at(i), inImg.dataPlanes.at(i));
    }
}

float calcCompressionRate(std::vector<uint8_t> &raw, std::vector<uint8_t> &compressed) {
    return (float)raw.size() / (float)compressed.size();
}

double getEncodeTime(anslib::Image img) {
    std::cout << "Counting encode time...\n";
    std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
    compressImage(img);
    std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(t2-t1).count();
}

double getDecodeTime(anslib::Image img) {
    std::cout << "Counting decode time...\n";
    std::chrono::time_point t1 = std::chrono::high_resolution_clock::now();
    decompressImage(img);
    std::chrono::time_point t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(t2-t1).count();
}


struct FileStats {
    std::string filename_;
    size_t dataSizeRaw_;
    size_t dataSizeEnc_;
    double compressionRate_;
    double encodeTime_;
    double decodeTime_;
    double encodeSpeed_;
    double decodeSpeed_;
    size_t bytesSizeOfImage(const anslib::Image &img) {
        size_t byteSize = 0;
        for (auto plane : img.dataPlanes) {
            byteSize += plane.size() * sizeof(decltype(plane.back()));
        }
        byteSize += img.symCountsIfCompressed.size() * sizeof(decltype(img.symCountsIfCompressed.back()));
        return byteSize;
    }
    FileStats(std::string filename) : filename_(filename) {
        anslib::bmplib::BmpImage bmp(filename.c_str());
        anslib::Image imgRaw(bmp);
        anslib::Image imgEncoded(imgRaw);
        compressImage(imgRaw, imgEncoded);

        dataSizeRaw_ = bmp.data.size() * sizeof(decltype(bmp.data.back()));
        dataSizeEnc_ = bytesSizeOfImage(imgEncoded);
        compressionRate_ = (double)dataSizeRaw_ / (double)dataSizeEnc_;

        encodeTime_ = getEncodeTime(imgRaw);
        decodeTime_ = getDecodeTime(imgEncoded);

        encodeSpeed_ = dataSizeRaw_ / encodeTime_ / 1048576;
        decodeSpeed_ = dataSizeRaw_ / decodeTime_ / 1048576;
    }
};

inline std::ostream& operator<<(std::ostream& os, struct FileStats fs) {
    os << "++++++\n"
       << fs.filename_ << '\n'
       << "dataSizeRaw_:\n\t"
       << std::right << fs.dataSizeRaw_ / 1024 << "KB\n"
       << "dataSizeEnc_:\n\t"
       << std::right << fs.dataSizeEnc_ / 1024 << "KB\n"
       << "compressionRate_:\n\t"
       << std::right << fs.compressionRate_ << '\n'
       << "encodeTime_:\n\t"
       << std::right << fs.encodeTime_ << "s\n"
       << "decodeTime_:\n\t"
       << std::right << fs.decodeTime_ << "s\n"
       << "encodeSpeed_:\n\t"
       << std::right << fs.encodeSpeed_ << "MB/s\n"
       << "decodeSpeed_:\n\t"
       << std::right << fs.decodeSpeed_ << "MB/s\n";
    return os;
}

int main() {
    std::vector<std::string> testBmps = listAllBmpsInDir((CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/"));
    std::vector<FileStats> encodeStats;
    for (const std::string &filename : testBmps) {
        std::cout << "Processing " << filename << '\n';
        encodeStats.push_back(FileStats(filename));
        break;
    }
    for (auto fs : encodeStats) {
        std::cout << fs;
    }
}