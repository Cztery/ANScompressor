#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "common.h"

namespace anslib {
template <typename T>
class Histogram {
  typedef std::pair<T, AnsCountsType> pair;

  template <typename Z>
  void count_freqs(std::vector<Z> vec) {
    for (auto i : vec) {
      counts[i]++;
    }
  }

  void count_cumul() {
    assert(cumul.size() == counts.size() + 1);
    assert(cumul.at(0) == 0);
    cumul.at(0) = 0;
    for (size_t i = 0; i < counts.size(); ++i) {
      cumul.at(i + 1) = cumul.at(i) + counts.at(i);
    }
    total = cumul.back();
  }

  size_t count_max_count() {
    size_t cur_max = 0;
    for (auto c : counts) {
      if (cur_max < c) {
        cur_max = c;
      }
    }
    return cur_max;
  }

 public:
  void norm_freqs(AnsCountsType target_total = PROB_SCALE) {
    // taken from ryg_ans
    cumul_norm = cumul;
    for (size_t i = 0; i <= counts.size(); ++i) {
      cumul_norm[i] = ((uint64_t)target_total * cumul_norm[i]) / total;
    }
    for (size_t i = 0; i <= cumul_norm.size(); i++) {
      if (counts[i] && cumul_norm[i] == cumul_norm[i + 1]) {
        AnsCountsType best_freq = ~0u;
        size_t best_steal_index = 0;
        for (size_t j = 0; j <= cumul_norm.size(); ++j) {
          AnsCountsType freq = cumul_norm[j + 1] - cumul_norm[j];
          if (freq > 1 && freq < best_freq) {
            best_freq = freq;
            best_steal_index = j;
          }
        }
        if (best_steal_index < i) {
          for (size_t j = best_steal_index + 1; j <= i; ++j) {
            cumul_norm[j]--;
          }
        } else {
          assert(best_steal_index > i);
          for (size_t j = i + 1; j <= best_steal_index; ++j) {
            cumul_norm[j]++;
          }
        }
      }
    }
    for (size_t i = 0; i < counts.size(); ++i) {
      counts_norm.push_back(cumul_norm.at(i + 1) - cumul_norm.at(i));
    }
  }

  template <typename Y>
  Histogram(std::vector<Y> syms)
      : counts(std::numeric_limits<T>::max() + 1, 0),
        cumul(std::numeric_limits<T>::max() + 2, 0) {
    count_freqs(syms);
    count_cumul();
    maxSymCount = count_max_count();
  }

  Histogram(std::vector<AnsCountsType> counts) : counts(counts), cumul(counts.size() + 1) {
    count_cumul();
  }

  Histogram& operator=(Histogram&& other) {
    if (this != &other) {
      counts = std::move(other.counts);
      counts_norm = std::move(other.counts_norm);
      cumul = std::move(other.cumul_norm);
      cumul_norm = std::move(other.cumul_norm);
      total = std::move(other.total);
      maxSymCount = std::move(other.maxSymCount);
    }
    return *this;
  }

  std::vector<AnsCountsType> counts, counts_norm;
  std::vector<AnsCountsType> cumul, cumul_norm;
  size_t total;
  size_t maxSymCount;
};

}  // namespace anslib
