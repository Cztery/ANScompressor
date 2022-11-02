#pragma once

#include <vector>

#include "common.h"

namespace anslib {

void convertRGB2YCoCg(std::vector<anslib::AnsSymbolType> r,
                      std::vector<anslib::AnsSymbolType> g,
                      std::vector<anslib::AnsSymbolType> b,
                      std::vector<anslib::AnsSymbolType> &y,
                      std::vector<anslib::AnsSymbolType> &co,
                      std::vector<anslib::AnsSymbolType> &cg);

void convertYCoCg2RGB(std::vector<anslib::AnsSymbolType> y,
                      std::vector<anslib::AnsSymbolType> co,
                      std::vector<anslib::AnsSymbolType> cg,
                      std::vector<anslib::AnsSymbolType> &r,
                      std::vector<anslib::AnsSymbolType> &g,
                      std::vector<anslib::AnsSymbolType> &b);
}  // namespace anslib