
#include "colorspaces.h"
#include "common.h"
#include "gtest/gtest.h"

TEST(colorspaces, rgb2rgb) {
  std::vector<anslib::AnsSymbol> r1 = {0xCAu, 0xFEu, 0xB1u, 0xBAu};
  std::vector<anslib::AnsSymbol> g1 = {0xFAu, 0xCEu, 0xB3u, 0xB3u};
  std::vector<anslib::AnsSymbol> b1 = {0xB1u, 0xBAu, 0xCAu, 0xFEu};

  std::vector<anslib::AnsSymbol> y, co, cg;
  std::vector<anslib::AnsSymbol> r2, g2, b2;

  anslib::convertRGB2YCoCg(r1, g1, b1, y, co, cg);

  anslib::convertYCoCg2RGB(y, co, cg, r2, g2, b2);

  EXPECT_EQ(r1, r2);
  EXPECT_EQ(g1, g2);
  EXPECT_EQ(b1, b2);
}