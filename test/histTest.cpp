#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>

#include "bmplib.h"
#include "common.h"
#include "gtest/gtest.h"
#include "histogram.h"
#include "image.h"

using namespace anslib;
using namespace bmplib;

class HistogramTestFix : public testing::Test {
 protected:
  virtual void SetUp() override {
    for (unsigned short sym = 0; sym <= 255; ++sym) {
      v1.push_back(sym);
    }
    h1 = std::make_unique<Histogram<AnsSymbol>>(v1);
    h1->norm_freqs(256);

    std::default_random_engine generator;
    vRand.resize(vRandSize);
    for (auto &sym : vRand) {
      sym = generator() % (std::numeric_limits<AnsSymbol>::max() + 1);
    }
    hRand = std::make_unique<Histogram<AnsSymbol>>(vRand);
    hRand->norm_freqs(exampleNormMax);
  }

  std::vector<AnsSymbol> v1;
  std::unique_ptr<Histogram<AnsSymbol>> h1;

  const AnsCountsType exampleNormMax = 1u << 14;
  const size_t vRandSize = 1024;
  std::vector<AnsSymbol> vRand;
  std::unique_ptr<Histogram<AnsSymbol>> hRand;
};

TEST_F(HistogramTestFix, hist_creation) {
  EXPECT_EQ(h1->counts.size(), 256);
  for (auto co : h1->counts) {
    EXPECT_EQ(co, 1);
  }
  EXPECT_EQ(h1->total, 256);
  EXPECT_EQ(h1->maxSymCount, 1);
}

TEST_F(HistogramTestFix, nothing_changed_after_renormalization) {
  EXPECT_EQ(h1->counts, h1->counts_norm);
}

TEST_F(HistogramTestFix, monotonicity_after_normalization) {
  EXPECT_NE(hRand->counts, hRand->counts_norm);

  // check if monotonically non-decreasing
  for (size_t a = 0; a < hRand->cumul_norm.size() - 1; ++a) {
    if (hRand->counts.at(a) == 0) {
      EXPECT_EQ(hRand->cumul_norm.at(a), hRand->cumul_norm.at(a + 1));
    } else {
      EXPECT_LT(hRand->cumul_norm.at(a), hRand->cumul_norm.at(a + 1));
    }
  }
}

TEST_F(HistogramTestFix, last_val_of_cumul_after_renorm) {
  EXPECT_EQ(h1->cumul_norm.back(), 256);
  EXPECT_EQ(hRand->cumul_norm.back(), exampleNormMax);
}

TEST(HistogramTest, basic_vec_histogram) {
  std::vector<AnsSymbol> v1{0, 1, 1, 2, 2, 2, 3, 3, 3, 3};

  Histogram<AnsSymbol> h1(v1);

  std::vector<AnsCountsType> expectedCounts{1, 2, 3, 4};
  expectedCounts.resize(std::numeric_limits<AnsSymbol>::max() + 1);

  EXPECT_EQ(h1.counts, expectedCounts);
  EXPECT_EQ(h1.counts.size(), expectedCounts.size());
}