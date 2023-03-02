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
  ChunkingParams(const ImgInfo &info, size_t chunkWid);
  ChunkingParams(const ImgInfo &info);
};

struct ImageDev {
  ImgInfo imgInfo = {0, 0, 0, 0, 0};
  anslib::AnsSymbolType *rawChunks_ = nullptr;
  anslib::AnsCountsType *chunkCounts_ = nullptr;
  anslib::AnsCountsType *chunkCumul_ = nullptr;
  anslib::AnsCompType *compChunks_ = nullptr;
  size_t *compChunksSizes_ = nullptr;
  ImageDev(){}
  ImageDev(const anslib::RawImage &ri);
  ImageDev(const anslib::CompImage &ci);
  ~ImageDev();

  void splitIntoChunks(size_t chunkWid);
  void joinChunks();

  const std::vector<anslib::AnsSymbolType> getPlane(size_t idx);
};

class ImageCompressor {
  const ImageDev &img_;
  ImageDev *imgNv_ = nullptr;
  public:
  ImageCompressor(const RawImage &ri);
  ImageCompressor(const CompImage &ci);
  ~ImageCompressor();
  ImageDev compress(size_t chunkWid);
  ImageDev decompress();
};

} // namespace anslib