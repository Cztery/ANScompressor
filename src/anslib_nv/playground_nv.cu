#include <cuda.h>
#include <iostream>
#include <vector>
#include "bmplib.h"
#include "ppmlib.h"
#include "common.h"
#include "compressor.h"
#include "image.h"
#include "benchlib.h"

#include <cstdio>

#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
template <typename T>
void check(T err, char const* const func, char const* const file,
           int const line)
{
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line
                  << std::endl;
        std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK_LAST_CUDA_ERROR() checkLast(__FILE__, __LINE__)
void checkLast(char const* const file, int const line)
{
    cudaError_t err{cudaGetLastError()};
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line
                  << std::endl;
        std::cerr << cudaGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

struct ImgInfo {
  size_t width_;
  size_t height_;
  size_t numOfPlanes_;
  size_t numOfChunksPerPlane_;
  size_t chunkWidth_;
};

struct ImageDev {
  ImgInfo *h_info, *d_info;
  anslib::AnsSymbolType **rawChunks;
  anslib::AnsCountsType **chunkCounts;
  anslib::AnsCountsType **chunkCumul;
  anslib::AnsCompType **compChunks;

  anslib::AnsSymbolType **h_rawChunks;
  anslib::AnsCountsType **h_chunkCounts;
  anslib::AnsCountsType **h_chunkCumul;
  anslib::AnsCompType **h_compChunks;
  
  ImageDev(const anslib::RawImage &ri);
  ImageDev(const anslib::CompImage &ci);
  ~ImageDev();
};

ImageDev::ImageDev(const anslib::RawImage &ri) {
  printf("constructor called\n");

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_info, sizeof(ImgInfo)));
  h_info = (ImgInfo *)malloc(sizeof(struct ImgInfo));
  ImgInfo tmpInfo = {ri.width_, ri.height_, ri.numOfPlanes_, ri.chunksPerPlaneCount(), ri.chunkWidth_};
  memcpy(h_info, &tmpInfo, sizeof(ImgInfo));
  CHECK_CUDA_ERROR(cudaMemcpy(d_info, h_info, sizeof(ImgInfo), cudaMemcpyHostToDevice));

  const size_t chunkCount = ri.dataPlanes_.size();
  h_rawChunks = (anslib::AnsSymbolType **)malloc(chunkCount*sizeof(anslib::AnsSymbolType*));
  h_chunkCounts = (anslib::AnsCountsType **)malloc(chunkCount*sizeof(anslib::AnsCountsType*));
  h_chunkCumul = (anslib::AnsCountsType **)malloc(chunkCount*sizeof(anslib::AnsCountsType*));
  h_compChunks = (anslib::AnsCompType **)malloc(chunkCount*sizeof(anslib::AnsCompType*));

  h_rawChunks = (anslib::AnsSymbolType **)malloc(chunkCount * sizeof(rawChunks));
  for(size_t chunkIdx = 0; chunkIdx < chunkCount; ++chunkIdx) {
    // chunk sizes may slightly vary for the chunks located at the edges of a plane
    const size_t chunkSize = ri.dataPlanes_.at(chunkIdx).size() * sizeof(anslib::AnsSymbolType *);
    CHECK_CUDA_ERROR(cudaMalloc(&h_rawChunks[chunkIdx], chunkSize * sizeof(anslib::AnsSymbolType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_chunkCounts[chunkIdx], anslib::ansCountsSize * sizeof(anslib::AnsCountsType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_chunkCumul[chunkIdx], anslib::ansCumulSize * sizeof(anslib::AnsCountsType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_compChunks[chunkIdx], chunkSize * sizeof(anslib::AnsCompType)));

    CHECK_CUDA_ERROR(cudaMemcpy(h_rawChunks[chunkIdx], ri.dataPlanes_.at(chunkIdx).data(),
      anslib::ansCountsSize, cudaMemcpyHostToDevice));
  }

  CHECK_CUDA_ERROR(cudaMalloc(&rawChunks, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&chunkCounts, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&chunkCumul, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&compChunks, chunkCount*sizeof(void*)));

  CHECK_CUDA_ERROR(cudaMemcpy((void*)rawChunks, (void*)h_rawChunks, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)chunkCounts, (void*)h_chunkCounts, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)compChunks, (void*)h_compChunks, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
}

ImageDev::ImageDev(const anslib::CompImage &ci) {
  printf("constructor called\n");
  
  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_info, sizeof(ImgInfo)));
  h_info = (ImgInfo *)malloc(sizeof(struct ImgInfo));
  ImgInfo tmpInfo = {ci.width_, ci.height_, ci.numOfPlanes_, ci.chunksPerPlaneCount(), ci.chunkWidth_};
  memcpy(h_info, &tmpInfo, sizeof(ImgInfo));
  CHECK_CUDA_ERROR(cudaMemcpy(d_info, h_info, sizeof(ImgInfo), cudaMemcpyHostToDevice));

  const size_t chunkCount = ci.compressedPlanes_.size();
  h_rawChunks = (anslib::AnsSymbolType **)malloc(chunkCount*sizeof(anslib::AnsSymbolType*));
  h_chunkCounts = (anslib::AnsCountsType **)malloc(chunkCount*sizeof(anslib::AnsCountsType*));
  h_chunkCumul = (anslib::AnsCountsType **)malloc(chunkCount*sizeof(anslib::AnsCountsType*));
  h_compChunks = (anslib::AnsCompType **)malloc(chunkCount*sizeof(anslib::AnsCompType*));

  h_rawChunks = (anslib::AnsSymbolType **)malloc(chunkCount * sizeof(rawChunks));
  for(size_t chunkIdx = 0; chunkIdx < chunkCount; ++chunkIdx) {
    // chunk sizes will vary for all compressed chunks
    const size_t chunkSize = ci.compressedPlanes_.at(chunkIdx).plane.size();

    CHECK_CUDA_ERROR(cudaMalloc(&h_rawChunks[chunkIdx], chunkSize * sizeof(anslib::AnsSymbolType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_chunkCounts[chunkIdx], anslib::ansCountsSize * sizeof(anslib::AnsCountsType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_chunkCumul[chunkIdx], anslib::ansCumulSize * sizeof(anslib::AnsCountsType)));
    CHECK_CUDA_ERROR(cudaMalloc(&h_compChunks[chunkIdx], chunkSize * sizeof(anslib::AnsCompType)));

    CHECK_CUDA_ERROR(cudaMemcpy(h_chunkCounts[chunkIdx], ci.compressedPlanes_.at(chunkIdx).counts.data(),
      anslib::ansCountsSize, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(h_compChunks[chunkIdx], ci.compressedPlanes_.at(chunkIdx).plane.data(),
      chunkSize, cudaMemcpyHostToDevice));
  }

  
  CHECK_CUDA_ERROR(cudaMalloc(&rawChunks, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&chunkCounts, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&chunkCumul, chunkCount*sizeof(void*)));
  CHECK_CUDA_ERROR(cudaMalloc(&compChunks, chunkCount*sizeof(void*)));

  CHECK_CUDA_ERROR(cudaMemcpy(rawChunks, h_rawChunks, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
  CHECK_CUDA_ERROR(cudaMemcpy(chunkCounts, h_chunkCounts, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
  CHECK_CUDA_ERROR(cudaMemcpy(chunkCumul, h_chunkCumul, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
  CHECK_CUDA_ERROR(cudaMemcpy(compChunks, h_compChunks, chunkCount*sizeof(void*), cudaMemcpyHostToDevice));
}

ImageDev::~ImageDev() {
  printf("destructor called\n");

  // release device-side chunk data arrays
  for(size_t chunkIdx = 0; chunkIdx < h_info->numOfChunksPerPlane_ * h_info->numOfPlanes_; ++chunkIdx) {
    if(h_rawChunks[chunkIdx])   cudaFree(h_rawChunks[chunkIdx]);
    if(h_compChunks[chunkIdx])  cudaFree(h_compChunks[chunkIdx]);
    if(h_chunkCounts[chunkIdx]) cudaFree(h_chunkCounts[chunkIdx]);
    if(h_chunkCumul[chunkIdx])  cudaFree(h_chunkCumul[chunkIdx]);
  }

  // release host-side pointer arrays
  if(h_rawChunks) free(h_rawChunks);
  if(h_compChunks) free(h_compChunks);
  if(h_chunkCounts) free(h_chunkCounts);
  if(h_chunkCumul) free(h_chunkCumul);
  
  // release device-side pointer arrays
  if(rawChunks) cudaFree(rawChunks);
  if(compChunks) cudaFree(compChunks);
  if(chunkCounts) cudaFree(chunkCounts);
  if(chunkCumul) cudaFree(chunkCumul);
  
  if(h_info) free(h_info);
  if(d_info) cudaFree(d_info);
}


__device__ void countSymbolsInPlane(anslib::AnsSymbolType *symbols, anslib::AnsCountsType *counts) {
  printf("ayooo\n");
}
__global__ void g_compressImage(ImageDev *img) {
  countSymbolsInPlane(img->rawChunks[0], img->chunkCounts[0]);
}


cudaDeviceProp printGPUproperties() {
  int nDevices;
  cudaDeviceProp prop;
  cudaGetDeviceCount(&nDevices);
  for (int i = 0; i < nDevices; i++) {
    
    cudaGetDeviceProperties(&prop, i);
    printf("Device Number: %d\n", i);
    printf("  Device name: %s\n", prop.name);
    printf("  Memory Clock Rate (KHz): %d\n",
          prop.memoryClockRate);
    printf("  Memory Bus Width (bits): %d\n",
          prop.memoryBusWidth);
    printf("  Concurrent Kernels: %d\n",
          prop.concurrentKernels);
    printf("  Async Engine Count: %d\n",
          prop.asyncEngineCount);      
    printf("  Peak Memory Bandwidth (GB/s): %f\n",
          2.0*prop.memoryClockRate*(prop.memoryBusWidth/8)/1.0e6);
    printf("  Shared Memory Per Block: %d\n",
          prop.sharedMemPerBlock);
    printf("  Multiprocessor Count: %d\n",
          prop.multiProcessorCount);
    printf("  Max Blocks Per Multiprocessor: %d\n",
          prop.maxBlocksPerMultiProcessor);
    printf("  Max Threads Per Block: %d\n",
          prop.maxThreadsPerBlock);
    printf("  Max Threads Per Multiprocessor: %d\n\n",
          prop.maxThreadsPerMultiProcessor);
  }
  return prop;
}

__global__ void g_splitIntoChunks_body(anslib::AnsSymbolType **inPlanes, anslib::AnsSymbolType **outChunks,
    size_t planeWid, size_t squareChunkWid, size_t numOfChunksPerPlane, size_t chunksCount_hor) {
  extern __shared__ anslib::AnsSymbolType sChunkLocal[];
  const int xOffsetOfChunk = blockIdx.x * squareChunkWid;
  const int yOffsetOfChunk = blockIdx.y * squareChunkWid;
  for (size_t rowOffset = 0; rowOffset + blockDim.y < squareChunkWid; rowOffset += blockDim.y) {
    const int scindex = (rowOffset + threadIdx.y) * squareChunkWid + threadIdx.x;
    const size_t inPlanePixIdx = (yOffsetOfChunk + rowOffset +
      threadIdx.y) * planeWid + xOffsetOfChunk + threadIdx.x;
    sChunkLocal[scindex] = inPlanes[blockIdx.z][inPlanePixIdx];
  }
  __syncthreads();
  if(threadIdx.x == 0 && threadIdx.y == 0) {
    const size_t outChunkIdx = blockIdx.z * numOfChunksPerPlane + blockIdx.y * chunksCount_hor + blockIdx.x;
    const size_t chunkSizeBytes = sizeof(anslib::AnsSymbolType)*squareChunkWid*squareChunkWid;
    // outChunks[outChunkIdx] = (anslib::AnsSymbolType *)malloc(chunkSizeBytes);
    //if(outChunks[outChunkIdx]) {
    memcpy(outChunks[outChunkIdx], sChunkLocal, chunkSizeBytes);
    //}
  }
  
}

/* If the image dimensions are not multiplicities of chunk size, right and bottom edge chunks contain
 more columns / rows - this kernel takes care of partitioning at right and bottom edge; 
 Consecutive blocks correspond to the image chunks in the following order:
|x |x |0  |
|x |x |1  |
|x |x |2  |
|4 |5 |3  |
|  |  |   |
*/
__global__ void g_splitIntoChunks_edges(anslib::AnsSymbolType **inPlanes, anslib::AnsSymbolType **outChunks,
    size_t planeHei, size_t planeWid, size_t squareChunkWid, size_t edgeChunkWid, size_t edgeChunkHei,
    size_t chunkCount_ver, size_t chunkCount_hor, bool isRightEdgeMisaligned, bool isBottomEdgeMisaligned) {
  extern __shared__ anslib::AnsSymbolType sChunkLocal[];
  const bool isRightEdge = (isRightEdgeMisaligned && isBottomEdgeMisaligned) ?
    (blockIdx.x < chunkCount_ver) : isRightEdgeMisaligned;
  const bool isBottomEdge = (isRightEdgeMisaligned && isBottomEdgeMisaligned) ?
    (blockIdx.x >= chunkCount_ver) : ! isRightEdgeMisaligned;
  
  // right and bottom edge chunks are numbered top to bottom, right to left
  const size_t chunkIdx_x = isRightEdge ? chunkCount_hor - 1 : blockIdx.x - chunkCount_ver;
  const size_t chunkIdx_y = isRightEdge ? blockIdx.x : chunkCount_ver - 1;
  
  const size_t xOffsetOfChunk = squareChunkWid * chunkIdx_x;
  const size_t yOffsetOfChunk = squareChunkWid * chunkIdx_y;

  const size_t chunkWid = isRightEdge ? edgeChunkWid : squareChunkWid;
  const size_t chunkHei = isBottomEdge ? edgeChunkHei : squareChunkWid;

  // because the threads are reserved for the maximal width of a chunk,
  // the thread number may exceed the chunk width - checking it here:
  if (threadIdx.x < chunkWid) { 
    for (size_t rowOffset = 0; rowOffset < chunkHei; rowOffset += blockDim.y) {
      const size_t scindex = (rowOffset + threadIdx.y) * chunkWid + threadIdx.x;
      const size_t inPlanePixIdx = (yOffsetOfChunk + rowOffset + threadIdx.y) *
        planeWid + xOffsetOfChunk + threadIdx.x;
      sChunkLocal[scindex] = inPlanes[blockIdx.z][inPlanePixIdx];
    }
  }

  __syncthreads();
  if(threadIdx.x == 0 && threadIdx.y == 0) {
    const size_t outChunkIdx = blockIdx.z * chunkCount_ver * chunkCount_hor + chunkCount_hor * chunkIdx_y + chunkIdx_x;
    const size_t chunkSizeBytes = sizeof(anslib::AnsSymbolType)*chunkWid*chunkHei;
    outChunks[outChunkIdx] = (anslib::AnsSymbolType *)malloc(chunkSizeBytes);
    memcpy(outChunks[outChunkIdx], sChunkLocal, chunkSizeBytes);
  }
  
}

void run_g_splitIntoChunks(ImageDev *img, size_t chunkWid) {
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  assert(prop.maxThreadsPerBlock > chunkWid);
  const size_t threadRows = prop.maxThreadsPerBlock / chunkWid;
  const size_t chunksCount_ver = img->h_info->height_ / chunkWid;
  const size_t chunksCount_hor = img->h_info->width_ / chunkWid;
  const size_t chunksCount = chunksCount_ver * chunksCount_hor;
  const size_t edgeChunkHei = img->h_info->height_ - chunksCount_ver * chunkWid + chunkWid;
  const size_t edgeChunkWid = img->h_info->width_ - chunksCount_hor * chunkWid + chunkWid;

  // anslib::AnsSymbolType **h_rawChunkedPlanes = nullptr;
  anslib::AnsSymbolType **d_rawChunkedPlanes = nullptr;
  anslib::AnsSymbolType **h_rawChunkedPlanes = nullptr;

  // alloc array of pointers to chunked data
  img->h_info->numOfChunksPerPlane_ = chunksCount_ver * chunksCount_hor;
  const size_t numOfChunksTotal = img->h_info->numOfPlanes_ * img->h_info->numOfChunksPerPlane_;
  const size_t chunkSizeBytes = chunkWid*chunkWid*sizeof(anslib::AnsSymbolType);
  
  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_rawChunkedPlanes, sizeof(anslib::AnsSymbolType*) * numOfChunksTotal));
  h_rawChunkedPlanes = (anslib::AnsSymbolType **)malloc(sizeof(anslib::AnsSymbolType*) * numOfChunksTotal);
  for (size_t y = 0; y < chunksCount_ver; ++y) {
    for (size_t x = 0; x < chunksCount_hor; ++x) {
      size_t chunkW = x == chunksCount_hor - 1 ? edgeChunkWid : chunkWid;
      size_t chunkH = x == chunksCount_ver - 1 ? edgeChunkHei : chunkWid;
      CHECK_CUDA_ERROR(cudaMalloc((void**)&h_rawChunkedPlanes[y * chunksCount_hor + x], chunkH * chunkW * sizeof(anslib::AnsSymbolType)));
      cudaMemset(h_rawChunkedPlanes[y * chunksCount_hor + x], 1, chunkH * chunkW * sizeof(anslib::AnsSymbolType));
    }
  }
  CHECK_CUDA_ERROR(cudaMemcpy(d_rawChunkedPlanes, h_rawChunkedPlanes,
    sizeof(anslib::AnsSymbolType*) * numOfChunksTotal, cudaMemcpyHostToDevice));
  //d_rawChunkedPlanes = h_rawChunkedPlanes;

  // execute chunking
  // 2D block indexes correspond to chunk location in plane
  dim3 grid(chunksCount_hor, chunksCount_ver, img->h_info->numOfPlanes_);
  // 2D thread indexes correspond to pixel location in chunk
  dim3 block(chunkWid, threadRows);
  g_splitIntoChunks_body<<<grid, block, chunkSizeBytes>>>(
    img->rawChunks, d_rawChunkedPlanes, img->h_info->width_, 
    chunkWid, img->h_info->numOfChunksPerPlane_, chunksCount_hor);

  const bool isHorMisaligned = img->h_info->width_ % chunkWid;
  const bool isVerMisaligned = img->h_info->height_ % chunkWid;
  if (isHorMisaligned || isVerMisaligned) {
    const dim3 edgeChunksBlockCount =
      (chunksCount_ver * isVerMisaligned + chunksCount_hor * chunksCount_ver - 1 * (isVerMisaligned && isHorMisaligned),
      1, img->h_info->numOfPlanes_);
    const dim3 edgeChunkThreads = (edgeChunkWid, prop.maxThreadsPerBlock / edgeChunkWid);
    const size_t edgeChunkMaxSizeBytes = edgeChunkHei * edgeChunkWid * sizeof(anslib::AnsSymbolType);
    g_splitIntoChunks_edges<<<edgeChunksBlockCount, edgeChunkThreads, edgeChunkMaxSizeBytes>>>(
      img->rawChunks, d_rawChunkedPlanes, img->h_info->height_, img->h_info->width_, chunkWid,
      edgeChunkWid, edgeChunkHei, chunksCount_ver, chunksCount_hor, isHorMisaligned, isVerMisaligned);
  }
  cudaDeviceSynchronize();
  // free non-chunked data arrays pointed by rawChunks
  for (size_t i = 0; i < img->h_info->numOfPlanes_; ++i) {
    cudaFree(img->h_rawChunks[i]);
  }
  cudaFree(img->rawChunks);

  // swap rawChunks and h_rawChunks with chunked d_rawChunkedPlanes
  img->rawChunks = d_rawChunkedPlanes;
  img->h_rawChunks = d_rawChunkedPlanes;

  // set corresponding metadata
  img->h_info->chunkWidth_ = chunkWid;
  img->h_info->numOfChunksPerPlane_ = chunksCount;
}


int main(int argc, char *argv[]) {
  std::vector<FileStats> encodeStats;
  std::vector<std::string> testImgs;
  listAllImgsInDir(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/",
                   ".bmp", testImgs);
  printGPUproperties();
  anslib::RawImage rawImg = FileStats::getTestImg(testImgs.back());
  ImageDev compContext(rawImg);
  run_g_splitIntoChunks(&compContext, 64);
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

      // std::cout << "Processing " << imgPath.substr(imgPath.rfind('/') + 1) << " for chunk_size = " << chunk_size << '\n';
      // for (size_t i = 0; i < img.dataPlanes_.size(); ++i) {
      //   assert(img.dataPlanes_.at(i).size() == imgRef.dataPlanes_.at(i).size());
      //   assert(img.dataPlanes_.at(i) == imgRef.dataPlanes_.at(i));
      // }
      
      // FileStats fs(img, imgPath.substr(imgPath.rfind('/') + 1));
      // encodeStats.push_back(fs);
      // std::cout << encodeStats.back();
    // }
  }*/
}