#include <exception>

#include "common_nv.cuh"
#include "image.cuh"
#include "image.h"

namespace anslib {

ChunkingParams::ChunkingParams(const ImgInfo &info, size_t chunkWid)
    : squareChunkWid_(chunkWid) {
  isRightEdgeMisaligned_ = info.width_ % chunkWid;
  isBottomEdgeMisaligned_ = info.height_ % chunkWid;
  chunksCountHor_ = (info.width_ + chunkWid - 1) / chunkWid;
  chunksCountVer_ = (info.height_ + chunkWid - 1) / chunkWid;
  chunksPerPlaneCount_ = chunksCountHor_ * chunksCountVer_;
  totalChunksCount_ = chunksPerPlaneCount_ * info.numOfPlanes_;
  edgeChunkWid_ = info.width_ % squareChunkWid_;
  edgeChunkHei_ = info.height_ % squareChunkWid_;
}

ChunkingParams::ChunkingParams(const ImgInfo &info)
    : ChunkingParams(info, info.chunkWidth_){};

ImageDev::ImageDev(const RawImage &ri) {
  printf("constructor called\n");
  imgInfo.width_ = ri.width_;
  imgInfo.height_ = ri.height_;
  imgInfo.numOfPlanes_ = ri.numOfPlanes_;
  imgInfo.numOfChunksPerPlane_ = ri.chunksPerPlaneCount();
  imgInfo.chunkWidth_ = ri.chunkWidth_;

  CHECK_CUDA_ERROR(cudaMalloc(&rawChunks_, ri.bytesSizeOfImage()));
  AnsSymbolType *tmp_chunksPtr = rawChunks_;
  for (auto chunk : ri.dataPlanes_) {
    size_t chunkSizeBytes = chunk.size() * sizeof(AnsSymbolType);
    CHECK_CUDA_ERROR(cudaMemcpy(tmp_chunksPtr, chunk.data(), chunkSizeBytes,
                                cudaMemcpyHostToDevice));
    tmp_chunksPtr += chunk.size();
  }
}

ImageDev::ImageDev(const CompImage &ci) {
  printf("constructor called\n");
  imgInfo.width_ = ci.width_;
  imgInfo.height_ = ci.height_;
  imgInfo.numOfPlanes_ = ci.numOfPlanes_;
  imgInfo.numOfChunksPerPlane_ = ci.chunksPerPlaneCount();
  imgInfo.chunkWidth_ = ci.chunkWidth_;

  const size_t numOfChunksTotal =
      imgInfo.numOfPlanes_ * imgInfo.numOfChunksPerPlane_;
  cudaMalloc(&chunkCounts_, numOfChunksTotal * ansCountsArrSize);

  cudaMalloc(&compChunksSizes_, numOfChunksTotal * sizeof(size_t));
  size_t compChunksTotalSize = 0;
  std::vector<size_t> compChunksSizes_tmp;
  for (size_t i = 0; i < ci.compressedPlanes_.size(); ++i) {
    compChunksTotalSize += ci.compressedPlanes_.size();
    compChunksSizes_tmp.push_back(ci.compressedPlanes_.at(i).plane.size());
  }

  CHECK_CUDA_ERROR(cudaMemcpy(compChunksSizes_, compChunksSizes_tmp.data(),
                              compChunksSizes_tmp.size() * sizeof(size_t),
                              cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(
      cudaMalloc(&compChunks_, compChunksTotalSize * ansCompTypeSize));

  AnsCompType *compChunksPtr = compChunks_;
  for (size_t i = 0; i < ci.compressedPlanes_.size(); ++i) {
    CHECK_CUDA_ERROR(cudaMemcpy(
        compChunksPtr, ci.compressedPlanes_.at(i).plane.data(),
        ci.compressedPlanes_.at(i).plane.size(), cudaMemcpyHostToDevice));
    compChunksPtr += compChunksSizes_tmp[i];
  }
}

ImageDev::~ImageDev() {
  printf("destructor called\n");

  // release device-side pointer arrays
  if (rawChunks_) CHECK_CUDA_ERROR(cudaFree(rawChunks_));
  rawChunks_ = nullptr;
  if (compChunks_) CHECK_CUDA_ERROR(cudaFree(compChunks_));
  if (chunkCounts_) CHECK_CUDA_ERROR(cudaFree(chunkCounts_));
  if (chunkCumul_) CHECK_CUDA_ERROR(cudaFree(chunkCumul_));
  if (compChunksSizes_) CHECK_CUDA_ERROR(cudaFree(compChunksSizes_));
}

__device__ void d_splitIntoChunks(AnsSymbolType *rawChunk,
                                  ChunkingParams cParams, ImageDev *img) {
  const bool isRightEdge = cParams.isRightEdgeMisaligned_ &&
                           (cParams.chunksCountHor_ - 1 == blockIdx.x);
  const bool isBottomEdge = cParams.isBottomEdgeMisaligned_ &&
                            (cParams.chunksCountVer_ - 1 == blockIdx.y);

  const size_t chunkWid =
      isRightEdge ? cParams.edgeChunkWid_ : cParams.squareChunkWid_;
  const size_t chunkHei =
      isBottomEdge ? cParams.edgeChunkHei_ : cParams.squareChunkWid_;
  if (chunkHei <= threadIdx.y || chunkWid <= threadIdx.x) return;

  const size_t planeOffset =
      blockIdx.z * img->imgInfo.width_ * img->imgInfo.height_;
  const size_t xOffsetOfChunkIn = blockIdx.x * cParams.squareChunkWid_;
  const size_t yOffsetOfChunkIn = blockIdx.y * cParams.squareChunkWid_;
  const size_t inPlanePixIdx =
      (yOffsetOfChunkIn + threadIdx.y) * img->imgInfo.width_ + xOffsetOfChunkIn;
  __syncthreads();
  if (threadIdx.x == 0 && threadIdx.y < chunkHei) {
    memcpy(&rawChunk[threadIdx.y * chunkWid],
           &img->rawChunks_[planeOffset + inPlanePixIdx],
           sizeof(anslib::AnsSymbolType) * chunkWid);
  }
  // rawChunk[threadIdx.x + threadIdx.y * chunkWid] =
  //     img->rawChunks_[planeOffset + inPlanePixIdx];

  // set chunkWidth_ and numOfChunksPerPlane_, only one thread per whole image
  if (!blockIdx.x && !blockIdx.y && !blockIdx.z && !threadIdx.x &&
      !threadIdx.y && !threadIdx.z) {
    img->imgInfo.chunkWidth_ = cParams.squareChunkWid_;
    img->imgInfo.numOfChunksPerPlane_ = cParams.chunksPerPlaneCount_;
  }
}

__device__ void d_joinChunks(AnsSymbolType *outPlanes,
                             const AnsSymbolType *inChunk,
                             const ChunkingParams cParams, ImgInfo &imgInfo) {
  const bool isRightEdge = cParams.isRightEdgeMisaligned_ &&
                           (cParams.chunksCountHor_ - 1 == blockIdx.x);
  const bool isBottomEdge = cParams.isBottomEdgeMisaligned_ &&
                            (cParams.chunksCountVer_ - 1 == blockIdx.y);

  const size_t chunkWid =
      isRightEdge ? cParams.edgeChunkWid_ : cParams.squareChunkWid_;
  const size_t chunkHei =
      isBottomEdge ? cParams.edgeChunkHei_ : cParams.squareChunkWid_;
  if (chunkHei <= threadIdx.y || chunkWid <= threadIdx.x) return;

  const size_t chunkOffsetOut =
      blockIdx.y * ((cParams.chunksCountHor_ - 1) * cParams.squareChunkWid_ *
                        cParams.squareChunkWid_ +
                    cParams.squareChunkWid_ * cParams.edgeChunkWid_) +
      blockIdx.x * chunkHei * cParams.squareChunkWid_;
  const size_t chunkVerPosInPlane = threadIdx.y * cParams.squareChunkWid_;
  const size_t planeOffset = blockIdx.z * imgInfo.width_ * imgInfo.height_;
  const size_t xOffsetOfChunkOut = blockIdx.x * cParams.squareChunkWid_;
  const size_t yOffsetOfChunkOut = blockIdx.y * cParams.squareChunkWid_;
  const size_t outPlanePixIdx =
      (yOffsetOfChunkOut + threadIdx.y) * imgInfo.width_ + xOffsetOfChunkOut;

  if (threadIdx.x == 0 && threadIdx.y < chunkHei) {
    // copy one row per thread, hence only #0 threads in x axis
    memcpy(&outPlanes[planeOffset + outPlanePixIdx],
           &inChunk[threadIdx.y * chunkWid],
           sizeof(anslib::AnsSymbolType) * chunkWid);
  }

  imgInfo.chunkWidth_ = 0;
  // outPlanes[planeOffset + outPlanePixIdx] =
  //     inChunks[planeOffset + chunkOffsetOut + chunkVerPosInPlane];
}

__global__ void compressionPipeline(const ChunkingParams cParams,
                                    ImageDev *imgInOut) {
  extern __shared__ uint8_t sChunkMem[];

  AnsSymbolType *sRawChunk = sChunkMem;
  AnsCountsType *sChunkCounts =
      (AnsCountsType *)(sChunkMem +
                        cParams.squareChunkWid_ * cParams.squareChunkWid_);
  AnsCountsType *sChunkCumul = sChunkCounts + ansCountsArrSize;
  AnsCompType *sCompressedChunk = (AnsCompType *)sChunkCumul + ansCumulArrSize;

  d_splitIntoChunks(sRawChunk, cParams, imgInOut);
}

ImageDev ImageCompressor::compress(size_t chunkWid) {
  if (!chunkWid) {
    throw std::invalid_argument("Provided invalid chunk width (0). Aborting.");
  }
  const ChunkingParams cParams(img_.imgInfo, chunkWid);

  // 2D block indices correspond to chunk location in plane
  dim3 grid(cParams.chunksCountHor_, cParams.chunksCountVer_,
            img_.imgInfo.numOfPlanes_);
  // 2D thread indices correspond to x and y idx in chunk
  const int threadRows = 1024 / cParams.squareChunkWid_;
  dim3 block(threadRows, cParams.squareChunkWid_);

  const size_t sharedMemSize =
      cParams.squareChunkWid_ * cParams.squareChunkWid_ * 2 *
          sizeof(AnsSymbolType) +
      (ansCountsArrSize + ansCumulArrSize) * sizeof(AnsCountsType);
  compressionPipeline<<<grid, block, sharedMemSize>>>(cParams, imgNv_);
  CHECK_LAST_CUDA_ERROR();
  ImageDev compImg;
  CHECK_CUDA_ERROR(
      cudaMemcpy(&compImg, imgNv_, sizeof(ImageDev), cudaMemcpyDeviceToHost));
  size_t compDataSize = 0;
  for (int i = 0; i < cParams.chunksPerPlaneCount_ * img_.imgInfo.numOfPlanes_;
       ++i) {
    compDataSize += compImg.compChunksSizes_[i];
  }
  return compImg;
}

__global__ void decompressionPipeline(const ChunkingParams cParams,
                                      ImageDev *imgInOut) {
  extern __shared__ uint8_t sChunkMem[];
  AnsSymbolType *sRawChunk = sChunkMem;
  AnsCountsType *sChunkCounts =
      (AnsCountsType *)(sChunkMem +
                        cParams.squareChunkWid_ * cParams.squareChunkWid_);
  AnsCountsType *sChunkCumul = sChunkCounts + ansCountsArrSize;
  AnsCompType *sCompressedChunk =
      (AnsCompType *)(sChunkCumul + ansCumulArrSize);
  // decompress

  d_joinChunks(imgInOut->rawChunks_, sRawChunk, cParams, imgInOut->imgInfo);
}

ImageDev ImageCompressor::decompress() {
  const ChunkingParams cParams(img_.imgInfo);

  // 2D block indices correspond to chunk location in plane
  dim3 grid(cParams.chunksCountHor_, cParams.chunksCountVer_,
            img_.imgInfo.numOfPlanes_);
  // 2D thread indices correspond to x and y idx in chunk
  const int threadRows = 1024 / cParams.squareChunkWid_;
  dim3 block(threadRows, cParams.squareChunkWid_);

  const size_t sharedMemSize =
      cParams.squareChunkWid_ * cParams.squareChunkWid_ * 2 + ansCountsArrSize +
      ansCumulArrSize;

  decompressionPipeline<<<grid, block, sharedMemSize>>>(cParams, imgNv_);
  CHECK_LAST_CUDA_ERROR();
  RawImage rawImg;
  CHECK_CUDA_ERROR(
      cudaMemcpy(&rawImg, imgNv_, sizeof(RawImage), cudaMemcpyDeviceToHost));
  return rawImg;
}

const std::vector<AnsSymbolType> ImageDev::getPlane(size_t idx) {
  if (idx >= (imgInfo.numOfPlanes_ * imgInfo.numOfChunksPerPlane_)) {
    return {};
  }
  const size_t planeSize =
      imgInfo.width_ * imgInfo.height_ * sizeof(AnsSymbolType);
  AnsSymbolType *planeTmp = (AnsSymbolType *)malloc(planeSize);
  CHECK_CUDA_ERROR(cudaMemcpy(
      planeTmp, rawChunks_ + (idx * (imgInfo.width_ * imgInfo.height_)),
      planeSize, cudaMemcpyDeviceToHost));
  std::vector<AnsSymbolType> vec_tmp(
      planeTmp, planeTmp + (imgInfo.height_ * imgInfo.width_));
  return vec_tmp;
}

ImageCompressor::ImageCompressor(const RawImage &ri) : img_(ri) {
  CHECK_CUDA_ERROR(cudaMalloc(&imgNv_, sizeof(ImageDev)));
  CHECK_CUDA_ERROR(
      cudaMemcpy(imgNv_, &img_, sizeof(ImageDev), cudaMemcpyHostToDevice));
}

ImageCompressor::ImageCompressor(const CompImage &ci) : img_(ci) {
  CHECK_CUDA_ERROR(cudaMalloc(&imgNv_, sizeof(ImageDev)));
  CHECK_CUDA_ERROR(
      cudaMemcpy(imgNv_, &img_, sizeof(ImageDev), cudaMemcpyHostToDevice));
}

ImageCompressor::~ImageCompressor() {
  if (imgNv_) cudaFree(imgNv_);
}

}  // namespace anslib