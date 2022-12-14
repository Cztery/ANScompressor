#include <cuda.h>

#include <cstdio>
#include <iostream>
#include <vector>

#include "benchlib.h"
#include "bmplib.h"
#include "common_nv.cuh"
#include "common.h"
#include "compressor.h"
#include "image.cuh"
#include "image.h"
#include "ppmlib.h"

int main(int argc, char *argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/",
                   ".bmp", testImgs);
  // printGPUproperties();
  anslib::RawImage rawImg = FileStats::getTestImg(testImgs.back());
  ImageDev compContext(rawImg);
  compContext.splitIntoChunks(64);
  {
    auto p0 = compContext.getPlane(0);
    auto p1 = compContext.getPlane(1);
    auto p2 = compContext.getPlane(2);
    anslib::RawImage chunked(p0, p1, p2,
                              compContext.h_info->width_, compContext.h_info->height_);
    anslib::bmplib::BmpImage out(testImgs.back().c_str());
    out.data = chunked.getPlanesAsBmpData();
    out.bmpWrite("chunked.bmp");
  }
  compContext.joinChunks();
  {
    auto p0 = compContext.getPlane(0);
    auto p1 = compContext.getPlane(1);
    auto p2 = compContext.getPlane(2);
    anslib::RawImage rejoined(p0, p1, p2,
                              compContext.h_info->width_, compContext.h_info->height_);
    anslib::bmplib::BmpImage out(testImgs.back().c_str());
    out.data = rejoined.getPlanesAsBmpData();
    out.bmpWrite("rejoined.bmp");
  
    if (rejoined.dataPlanes_[0] != rawImg.dataPlanes_[0])
      return 1;
    if (rejoined.dataPlanes_[1] != rawImg.dataPlanes_[1])
      return 2;
    if (rejoined.dataPlanes_[2] != rawImg.dataPlanes_[2])
      return 3;
    return 5;
  }
  /*for (auto imgPath : testImgs) {
   anslib::RawImage rawImg = FileStats::getTestImg(imgPath);
    for (uint32_t chunk_size = 0; chunk_size <= 512; chunk_size += 64) {
      /// TODO: parallelize
      // rawImg.splitIntoChunks(chunk_size);
      anslib::CompImage compImg;
      anslib::AnsEncoder::compressImage(rawImg, compImg);
      {
        ImageDev compContext(rawImg);
        g_compressImage<<<3, 12>>>(&compContext);
        ImageDev g_decompContext(compImg);
      }

      /// TODO: make a
      // anslib::AnsEncoder::compressImage(img, resultImg);
      // anslib::AnsDecoder::decompressImage(resultImg, img);

      // std::cout << "Processing " << imgPath.substr(imgPath.rfind('/') + 1) <<
  " for chunk_size = " << chunk_size << '\n';
      // for (size_t i = 0; i < img.dataPlanes_.size(); ++i) {
      //   assert(img.dataPlanes_.at(i).size() ==
  imgRef.dataPlanes_.at(i).size());
      //   assert(img.dataPlanes_.at(i) == imgRef.dataPlanes_.at(i));
      // }

      // FileStats fs(img, imgPath.substr(imgPath.rfind('/') + 1));
      // encodeStats.push_back(fs);
      // std::cout << encodeStats.back();
    // }
  }*/
}