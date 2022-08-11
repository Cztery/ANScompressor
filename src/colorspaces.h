#pragma once

#include <vector>

#include "common.h"

namespace anslib {

void convertRGB2YCoCg(std::vector<anslib::AnsSymbol> r,
                      std::vector<anslib::AnsSymbol> g,
                      std::vector<anslib::AnsSymbol> b,
                      std::vector<anslib::AnsSymbol> &y,
                      std::vector<anslib::AnsSymbol> &co,
                      std::vector<anslib::AnsSymbol> &cg);

void convertYCoCg2RGB(std::vector<anslib::AnsSymbol> y,
                      std::vector<anslib::AnsSymbol> co,
                      std::vector<anslib::AnsSymbol> cg,
                      std::vector<anslib::AnsSymbol> &r,
                      std::vector<anslib::AnsSymbol> &g,
                      std::vector<anslib::AnsSymbol> &b);
}  // namespace anslib