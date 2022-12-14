#include "common_nv.cuh"
#include "image.cuh"
#include "image.h"

namespace anslib {

ChunkingParams::ChunkingParams(const ImgInfo &info, size_t chunkWid)
    : squareChunkWid_(chunkWid) {
  isRightEdgeMisaligned_ = info.width_ % chunkWid;
  isBottomEdgeMisaligned_ = info.height_ % chunkWid;
  chunksCountHor_ = info.width_ / chunkWid;
  chunksCountVer_ = info.height_ / chunkWid;
  chunksPerPlaneCount_ = chunksCountHor_ * chunksCountVer_;
  totalChunksCount_ = chunksPerPlaneCount_ * info.numOfPlanes_;
  edgeChunkHei_ = squareChunkWid_ + info.height_ % squareChunkWid_;
  edgeChunkWid_ = squareChunkWid_ + info.width_ % squareChunkWid_;
  maxChunkSize_ = edgeChunkWid_ * edgeChunkHei_ * ansSymbolTypeSize;
}

ChunkingParams::ChunkingParams(const ImgInfo &info)
    : ChunkingParams(info, info.chunkWidth_) {};

ImageDev::ImageDev(const RawImage &ri) {
  printf("constructor called\n");
  imgInfo.width_ = ri.width_;
  imgInfo.height_ = ri.height_;
  imgInfo.numOfPlanes_ = ri.numOfPlanes_;
  imgInfo.numOfChunksPerPlane_ = ri.chunksPerPlaneCount();
  imgInfo.chunkWidth_ = ri.chunkWidth_;

  CHECK_CUDA_ERROR(cudaMalloc(&rawChunks, ri.bytesSizeOfImage()));
  AnsSymbolType *tmp_chunksPtr = rawChunks;
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
  cudaMalloc(&chunkCounts, numOfChunksTotal * ansCountsArrSize);

  cudaMalloc(&compChunksSizes, numOfChunksTotal * sizeof(size_t));
  size_t compChunksTotalSize = 0;
  std::vector<size_t> compChunksSizes_tmp;
  for (size_t i = 0; i < ci.compressedPlanes_.size(); ++i) {
    compChunksTotalSize += ci.compressedPlanes_.size();
    compChunksSizes_tmp.push_back(ci.compressedPlanes_.at(i).plane.size());
  }

  CHECK_CUDA_ERROR(cudaMemcpy(compChunksSizes, compChunksSizes_tmp.data(),
                              compChunksSizes_tmp.size() * sizeof(size_t),
                              cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(
      cudaMalloc(&compChunks, compChunksTotalSize * ansCompTypeSize));

  AnsCompType *compChunksPtr = compChunks;
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
  if (rawChunks) CHECK_CUDA_ERROR(cudaFree(rawChunks));
  if (compChunks) CHECK_CUDA_ERROR(cudaFree(compChunks));
  if (chunkCounts) CHECK_CUDA_ERROR(cudaFree(chunkCounts));
  if (chunkCumul) CHECK_CUDA_ERROR(cudaFree(chunkCumul));
  if (compChunksSizes) CHECK_CUDA_ERROR(cudaFree(compChunksSizes));
}

__device__ void d_splitIntoChunks(AnsSymbolType *rawChunk,
                                  ChunkingParams cParams,
                                  ImageDev img) {
  const bool isRightEdge = cParams.isRightEdgeMisaligned_ &&
                           (cParams.chunksCountHor_ - 1 == blockIdx.x);
  const bool isBottomEdge = cParams.isBottomEdgeMisaligned_ &&
                            (cParams.chunksCountVer_ - 1 == blockIdx.y);

  const size_t chunkWid =
      isRightEdge ? cParams.edgeChunkWid_ : cParams.squareChunkWid_;
  const size_t chunkHei =
      isBottomEdge ? cParams.edgeChunkHei_ : cParams.squareChunkWid_;
  if (chunkHei <= threadIdx.y || chunkWid <= threadIdx.x) return;

  const size_t planeOffset = blockIdx.z * img.imgInfo.width_ * img.imgInfo.height_;
  const size_t xOffsetOfChunkIn = blockIdx.x * cParams.squareChunkWid_;
  const size_t yOffsetOfChunkIn = blockIdx.y * cParams.squareChunkWid_;
  const size_t inPlanePixIdx = (yOffsetOfChunkIn + threadIdx.x) * img.imgInfo.width_ +
                               xOffsetOfChunkIn + threadIdx.x;

  rawChunk[threadIdx.x + threadIdx.y * chunkWid] =
      img.rawChunks[planeOffset + inPlanePixIdx];
}

__device__ void d_joinChunks(AnsSymbolType *outPlanes, const AnsSymbolType *inChunks,
                             const ChunkingParams cParams, const ImgInfo imgInfo) {
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
  const size_t outPlanePixIdx = (yOffsetOfChunkOut + threadIdx.y) * imgInfo.width_ +
                                xOffsetOfChunkOut + threadIdx.x;
  outPlanes[planeOffset + outPlanePixIdx] =
      inChunks[planeOffset + chunkOffsetOut + chunkVerPosInPlane];
}

__global__ void compressionPipeline(const ChunkingParams &cParams,
      ImageDev *imgInOut) {
  __shared__ uint8_t sChunkMem[];

  AnsSymbolType *sRawChunk = sChunkMem;
  AnsCountsType *sChunkCounts = sChunkMem + cParams.maxChunkSize_;
  AnsCountsType *sChunkCumul = sChunkCounts + ansCountsArrSize;
  AnsCompType *sCompressedChunk = (AnsCompType *)sChunkCumul + ansCumulArrSize;

  d_splitIntoChunks(sRawChunk, cParams, imgIn);
}

void ImageDev::runCompressionPipeline(size_t chunkWid) {
  const ChunkingParams cParams(imgInfo, chunkWid);

  // 2D block indices correspond to chunk location in plane
  dim3 grid(cParams.chunksCountHor_, cParams.chunksCountVer_,
            imgInfo.numOfPlanes_);
  // 2D thread indices correspond to x and y idx in chunk
  dim3 block(cParams.edgeChunkWid_, cParams.edgeChunkHei_);

  const size_t sharedMemSize =
      cParams.maxChunkSize_ * 2 + ansCountsArrSize + ansCumulArrSize;

  ImageDev *imgDevInOut = nullptr;
  CHECK_CUDA_ERROR(cudaMalloc(&imgDevInOut, sizeof(ImageDev)));
  CHECK_CUDA_ERROR(cudaMemcpy(imgDevInOut, this, sizeof(ImageDev),
  compressionPipeline<<<grid, block, sharedMemSize>>>(cParams, imgDevInOut);
  CHECK_CUDA_ERROR(cudaMemcpy(this, imgDevInOut, sizeof(ImageDev),
                              cudaMemcpyDeviceToHost));

  const size_t compDataSize = 0;
  for (int i = 0; i < cParams.chunksPerPlaneCount_ * imgInfo.numOfPlanes_;
       ++i) {
    compDataSize += compChunksSizes[i];
  }
  CHECK_CUDA_ERROR(cudaMalloc(&compChunks, compDataSize));
}

__global__ void ImageDev::decompressionPipeline(const ChunkingParams &cParams,
      ImageDev *imgInOut) {
  __shared__ uint8_t sChunkMem[];
  AnsSymbolType *sRawChunk = sChunkMem;
  AnsCountsType *sChunkCounts = sChunkMem + cParams.maxChunkSize_;
  AnsCountsType *sChunkCumul = sChunkCounts + ansCountsArrSize;
  AnsCompType *sCompressedChunk = sChunkCumul + ansCumulArrSize;
  // decompress

  d_joinChunks(rawChunks, sRawChunk, cParams, imgInfo);
}

void ImageDev::runDecompressionPipeline() { 
  const ChunkingParams cParams(imgInfo);

  // 2D block indices correspond to chunk location in plane
  dim3 grid(cParams.chunksCountHor_, cParams.chunksCountVer_,
            imgInfo.numOfPlanes_);
  // 2D thread indices correspond to x and y idx in chunk
  dim3 block(cParams.edgeChunkWid_, cParams.edgeChunkHei_);

  const size_t sharedMemSize =
      cParams.maxChunkSize_ * 2 + ansCountsArrSize + ansCumulArrSize;

  ImageDev *imgDevInOut = nullptr;
  CHECK_CUDA_ERROR(cudaMalloc(&imgDevInOut, sizeof(ImageDev)));
  CHECK_CUDA_ERROR(cudaMemcpy(imgDevInOut, this, sizeof(ImageDev),
                              cudaMemcpyHostToDevice));
  decompressionPipeline<<<grid, block, sharedMemSize>>>(cParams, imgDevInOut);
  CHECK_CUDA_ERROR(cudaMemcpy(this, imgDevInOut, sizeof(ImageDev),
                              cudaMemcpyDeviceToHost));
}

const std::vector<AnsSymbolType> ImageDev::getPlane(size_t idx) {
  if (idx >= (imgInfo.numOfPlanes_ * imgInfo.numOfChunksPerPlane_)) {
    return {};
  }
  const size_t planeSize =
      imgInfo.width_ * imgInfo.height_ * sizeof(AnsSymbolType);
  AnsSymbolType *planeTmp = (AnsSymbolType *)malloc(planeSize);
  CHECK_CUDA_ERROR(cudaMemcpy(
      planeTmp, rawChunks + (idx * (imgInfo.width_ * imgInfo.height_)),
      planeSize, cudaMemcpyDeviceToHost));
  std::vector<AnsSymbolType> vec_tmp(
      planeTmp, planeTmp + (imgInfo.height_ * imgInfo.width_));
  return vec_tmp;
}

}  // namespace anslib