#include "colorspaces.h"

void anslib::convertRGB2YCoCg(std::vector<anslib::AnsSymbolType> r,
                              std::vector<anslib::AnsSymbolType> g,
                              std::vector<anslib::AnsSymbolType> b,
                              std::vector<anslib::AnsSymbolType> &y,
                              std::vector<anslib::AnsSymbolType> &co,
                              std::vector<anslib::AnsSymbolType> &cg) {
  for (size_t i = 0; i < r.size(); ++i) {
    y.push_back((r.at(i) + b.at(i)) / 4 + g.at(i) / 2);
    co.push_back((r.at(i) - b.at(i)) / 2);
    cg.push_back((-r.at(i) - b.at(i)) / 4 + g.at(i) / 2);
  }
}

void anslib::convertYCoCg2RGB(std::vector<anslib::AnsSymbolType> y,
                              std::vector<anslib::AnsSymbolType> co,
                              std::vector<anslib::AnsSymbolType> cg,
                              std::vector<anslib::AnsSymbolType> &r,
                              std::vector<anslib::AnsSymbolType> &g,
                              std::vector<anslib::AnsSymbolType> &b) {
  for (size_t i = 0; i < y.size(); ++i) {
    r.push_back(y.at(i) + co.at(i) - cg.at(i));
    g.push_back(y.at(i) + cg.at(i));
    b.push_back(y.at(i) - co.at(i) - cg.at(i));
  }
}