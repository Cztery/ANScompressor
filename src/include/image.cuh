#pragma once
#include "common.h"
#include "image.h"

namespace anslib {

struct ImgInfo {
  size_t width_;
  size_t height_;
  size_t numOfPlanes_;
  size_t numOfChunksPerPlane_;
  size_t chunkWidth_;
};

struct ChunkingParams {
  size_t squareChunkWid_;
  bool isRightEdgeMisaligned_;
  bool isBottomEdgeMisaligned_;
  size_t chunksCountHor_;
  size_t chunksCountVer_;
  size_t chunksPerPlaneCount_;
  size_t totalChunksCount_;
  size_t edgeChunkHei_;
  size_t edgeChunkWid_;
  size_t maxChunkSize_;
  ChunkingParams(const ImgInfo &info, size_t chunkWid);
  ChunkingParams(const ImgInfo &info);
};

struct ImageDev {
  ImgInfo imgInfo;
  anslib::AnsSymbolType *rawChunks = nullptr;
  anslib::AnsCountsType *chunkCounts = nullptr;
  anslib::AnsCountsType *chunkCumul = nullptr;
  anslib::AnsCompType *compChunks = nullptr;
  size_t *compChunksSizes = nullptr;
  ImageDev(const anslib::RawImage &ri);
  ImageDev(const anslib::CompImage &ci);
  ~ImageDev();

  void splitIntoChunks(size_t chunkWid);
  void joinChunks();

  void runCompressionPipeline(size_t chunkWid);
  void runDecompressionPipeline();

  const std::vector<anslib::AnsSymbolType> getPlane(size_t idx);
};

} // namespace anslib