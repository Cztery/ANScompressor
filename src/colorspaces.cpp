#include "colorspaces.h"

void anslib::convertRGB2YCoCg(std::vector<anslib::AnsSymbol> r,
                      std::vector<anslib::AnsSymbol> g,
                      std::vector<anslib::AnsSymbol> b,
                      std::vector<anslib::AnsSymbol> &y,
                      std::vector<anslib::AnsSymbol> &co,
                      std::vector<anslib::AnsSymbol> &cg) {
  for (size_t i = 0; i < r.size(); ++i) {
    y.push_back(r.at(i) / 4 + g.at(i) / 2 + b.at(i) / 4);
    co.push_back(r.at(i) / 2 - b.at(i) / 2);
    cg.push_back(-r.at(i) / 4 + g.at(i) / 2 - b.at(i) / 4);
  }
}

void anslib::convertYCoCg2RGB(std::vector<anslib::AnsSymbol> y,
                      std::vector<anslib::AnsSymbol> co,
                      std::vector<anslib::AnsSymbol> cg,
                      std::vector<anslib::AnsSymbol> &r,
                      std::vector<anslib::AnsSymbol> &g,
                      std::vector<anslib::AnsSymbol> &b) {
  for (size_t i = 0; i < y.size(); ++i) {
    r.push_back(y.at(i) + co.at(i) - cg.at(i));
    g.push_back(y.at(i) + cg.at(i));
    b.push_back(y.at(i) - co.at(i) - cg.at(i));
  }
}