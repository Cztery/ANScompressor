#include "ppmlib.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>


anslib::ppmlib::PpmImage::PpmImage(const char* filename) {
  std::ifstream f_img(filename, std::ios::binary);
  
  if (!f_img.is_open()) {
      throw std::ios_base::failure("Could not open file.");
  }
  
  std::string line;
  std::getline(f_img, line);
  if (line != "P6") {
      std::cout << "Error: not a binary PPM file." << std::endl;
      return;
  }
  fileMagic = line;
  
  std::getline(f_img, line);
  try {
      std::stringstream ss (line);
      ss >> width_;
      ss >> height_;
      std::getline(f_img, line);
      ss = std::stringstream(line);
      ss >> maxVal_;
  } catch (...) {
      std::cout << "Error: invalid file header\n";
      return;
  }
  
  size_t imSize = width_ * height_;
  r.resize(imSize);
  g.resize(imSize);
  b.resize(imSize);
  
  for (size_t i = 0; i < imSize; ++i) {
      char tmp;
      f_img.read(&tmp, 1);
      r.at(i) = tmp;
      f_img.read(&tmp, 1);
      g.at(i) = tmp;
      f_img.read(&tmp, 1);
      b.at(i) = tmp;
  }
  
  f_img.close();
}
