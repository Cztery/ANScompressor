#include <numeric>
#include <fstream>
#include <ostream>

#include "bmplib.h"
#include "histogram.h"
#include "image.h"
using namespace anslib;

std::ostream& operator<<(std::ostream &os, std::vector<AnsCountsType> vec) {
  for (auto &n : vec) {
    os << n;
    if (&n != &vec.back()) {
      os << ", ";
    }
  }
  return os;
}

int main(int argc, char* argv[]) {
  anslib::bmplib::BmpImage inImg;
  inImg.bmpRead(CMAKE_SOURCE_DIR
                "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0001.bmp");
  std::cout << inImg.fileHeader_ << inImg.infoHeader_;
  const char* statsFileName = argv[1];
  anslib::RawImage img(inImg);
  
  Histogram<uint8_t> h0(img.dataPlanes_.at(0));
  h0.norm_freqs();

  std::vector<long> counts;
  for (auto count : h0.counts) {
    counts.push_back(count);
  }

  std::vector<int> xs(h0.cumul.size());
  std::iota(xs.begin(), xs.end(), 0);

  std::ofstream statsFile(statsFileName, std::ios_base::out);
  statsFile << "{\n"
            << "\"counts\": [" << h0.counts << "],\n"
            << "\"counts_norm\": [" << h0.counts_norm << "],\n"
            << "\"cumul\": [" << h0.cumul << "],\n"
            << "\"cumul_norm\": [" << h0.cumul_norm << "]\n"
            << "}\n";

}
