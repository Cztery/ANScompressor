#include <iostream>
#include <vector>
#include "bmplib.h"
#include "ppmlib.h"
#include "common.h"
#include "compressor.h"
#include "image.h"
#include "benchlib.h"

#include <cstdio>

struct ImgInfo {
  size_t width_;
  size_t height_;
  size_t numOfPlanes_;
  size_t numOfChunksPerPlane_;
  size_t chunkWidth_;
};

struct ImageDev {
  ImgInfo *info;
  anslib::AnsSymbol ***rawChunks;
  anslib::AnsCountsType ***chunkCounts;
  anslib::AnsCountsType ***chunkCumul;
  uint8_t ***compChunks;
  
  ImageDev(const anslib::RawImage &ri);
  ImageDev(const anslib::CompImage &ci);
  ~ImageDev();
};

ImageDev::ImageDev(const anslib::RawImage &ri) {
  printf("constructor called\n");

  cudaMalloc((void**)info, sizeof(ImgInfo));
  ImgInfo tmpInfo{ri.width_, ri.height_, ri.numOfPlanes_, ri.chunksPerPlaneCount(), ri.chunkWidth_};
  cudaMemcpy(info, &tmpInfo, sizeof(ImgInfo), cudaMemcpyHostToDevice);

  cudaMalloc((void**)rawChunks, ri.dataPlanes_.size());
  for(size_t chunkIdx = 0; chunkIdx < ri.dataPlanes_.size(); ++chunkIdx) {
    // chunk sizes may slightly vary for the chunks located at the edges of a plane
    const size_t chunkSize = ri.dataPlanes_.at(chunkIdx).size(); 
    cudaMalloc((void**)rawChunks[chunkIdx], chunkSize);
    cudaMalloc((void**)compChunks[chunkIdx], chunkSize);
    cudaMalloc((void**)chunkCounts[chunkIdx], anslib::ansCountsSize);
  }
}

ImageDev::ImageDev(const anslib::CompImage &ci) {
  printf("constructor called\n");
  
  cudaMalloc((void**)info, sizeof(ImgInfo));
  ImgInfo tmpInfo = {ci.width_, ci.height_, ci.numOfPlanes_, ci.chunksPerPlaneCount(), ci.chunkWidth_};
  cudaMemcpy(info, &tmpInfo, sizeof(ImgInfo), cudaMemcpyHostToDevice);
  
  cudaMalloc((void**)compChunks, ci.compressedPlanes_.size());
  for(size_t chunkIdx = 0; chunkIdx < ci.compressedPlanes_.size(); ++chunkIdx) {
    // chunk sizes will vary for all compressed chunks
    const size_t chunkSize = ci.compressedPlanes_.at(chunkIdx).plane.size();

    cudaMalloc((void**)compChunks[chunkIdx], chunkSize);
    cudaMalloc((void**)chunkCounts[chunkIdx], anslib::ansCountsSize);
    cudaMalloc((void**)rawChunks[chunkIdx], ci.compressedPlanes_.at(chunkIdx).rawPlaneSize);
    cudaMemcpy(compChunks[chunkIdx], ci.compressedPlanes_.at(chunkIdx).plane.data(),
      chunkSize, cudaMemcpyHostToDevice);
    cudaMemcpy(chunkCounts[chunkIdx], ci.compressedPlanes_.at(chunkIdx).counts.data(),
      anslib::ansCountsSize, cudaMemcpyHostToDevice);
  }
}

ImageDev::~ImageDev() {
  printf("destructor called\n");
  
  for(size_t chunkIdx = 0; chunkIdx < info->numOfChunksPerPlane_ * info->numOfPlanes_; ++chunkIdx) {
    cudaFree(rawChunks[chunkIdx]);
    cudaFree(compChunks[chunkIdx]);
    cudaFree(chunkCounts[chunkIdx]);
    cudaFree(chunkCumul[chunkIdx]);
  }
  cudaFree(rawChunks);
  cudaFree(compChunks);
  cudaFree(chunkCounts);
  cudaFree(chunkCumul);
  cudaFree(info);
}

// struct ImageDev *copyRawImgToDevice(const anslib::RawImage &inImg) {
//   struct ImageDev *rawImgHandleHost(inImg);
//   struct ImageDev *rawImgHandleDevice;
//   cudaMalloc((void**)rawImgHandleDevice, sizeof(struct ImageDev));
//   cudaMemcpy(rawImgHandleDevice, rawImgHandleHost, cudaMemcpyHostToDevice);
// }

// void copyCompImgFromDevice(anslib::CompImage &outImg, anslib::) {
//   struct ImageDev *compImgDev; // chce comp i metadata, nie chcę raw
//   cudaMemcpy()
// }

// cudaError_t freeRawImageDev(struct ImageDev *RIptr) {
//   cudaFree();
// }

// cudaArray_t x;
// void compressImageParallel(const anslib::RawImage &inImg, anslib::CompImage &outImg) {
//   RawImageCu *copyRawImgToDevice(inImg);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
   
// }

int main(int argc, char *argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/",
                   ".bmp", testImgs);
  for (auto imgPath : testImgs) {
    for (uint32_t chunk_size = 0; chunk_size <= 512; chunk_size += 64) {
      anslib::RawImage rawImg = FileStats::getTestImg(imgPath);
      
      /// TODO: parallelize
      rawImg.splitIntoChunks(chunk_size);
      anslib::CompImage compImg;
      anslib::AnsEncoder::compressImage(rawImg, compImg);
      {
        ImageDev compContext(rawImg);
        ImageDev decompContext(compImg);
      }
      
      /// TODO: make a 
      // anslib::AnsEncoder::compressImage(img, resultImg);
      // anslib::AnsDecoder::decompressImage(resultImg, img);

      // std::cout << "Processing " << imgPath.substr(imgPath.rfind('/') + 1) << " for chunk_size = " << chunk_size << '\n';
      // for (size_t i = 0; i < img.dataPlanes_.size(); ++i) {
      //   assert(img.dataPlanes_.at(i).size() == imgRef.dataPlanes_.at(i).size());
      //   assert(img.dataPlanes_.at(i) == imgRef.dataPlanes_.at(i));
      // }
      
      // FileStats fs(img, imgPath.substr(imgPath.rfind('/') + 1));
      // encodeStats.push_back(fs);
      // std::cout << encodeStats.back();
    }
}
}