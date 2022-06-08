#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

template <typename T>
class Histogram {
  typedef std::pair<T, uint32_t> pair;

  template <typename Z>
  void count_freqs(std::vector<Z> vec) {
    for (auto i : vec) {
      counts[i]++;
    }
  }

  void count_cumul() {
    assert(cumul.size() == counts.size()+1);
    assert(cumul.at(0) == 0);
    cumul.at(0) = 0;
    for (size_t i = 0; i < counts.size(); ++i) {
      cumul.at(i+1) = cumul.at(i) + counts.at(i);
    }
    total = cumul.back();
  }

  // size_t check_sanity() {
  //   for (size_t a = 1; a < cumul_norm.size() - 1; ++a) {
  //     if (cumul_norm.at(a) > cumul_norm.at(a+1)) {
  //       return a;
  //     }
  //   }
  //   return 0;
  // }

public:
  void norm_freqs(uint32_t target_total = (1u << 14)) {
    // taken from ryg_ans
    cumul_norm = cumul;
    for (size_t i = 0; i <= counts.size(); ++i) {
      cumul_norm[i] = ((uint64_t)target_total * cumul_norm[i])/total;
    }
    for (size_t i = 0; i <= cumul_norm.size(); i++) {
      if(counts[i] && cumul_norm[i] == cumul_norm[i+1]) {
        uint32_t best_freq = ~0u;
        size_t best_steal_index = 0;
        for (size_t j = 0; j <= cumul_norm.size(); ++j) {
          uint32_t freq = cumul_norm[j+1] - cumul_norm[j];
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
    for(size_t i = 0; i < counts.size(); ++i) {
      counts_norm.push_back(cumul_norm.at(i+1) - cumul_norm.at(i));
    }
  }

  template <typename Y>
  Histogram(std::vector<Y> vec)
      : counts(std::numeric_limits<T>::max() + 1, 0),
        cumul(std::numeric_limits<T>::max() + 2, 0) {
    count_freqs(vec);
    count_cumul();
  }

  void sort_counts() {
    auto comp = [](const pair &l, const pair &r) {
      if (l.second != r.second) {
        return l.second > r.second;
      }
      return l.first > r.first;
    };

    for (size_t i = 0; i < counts.size(); ++i) {
      sorted.push_back(pair(i, counts.at(i)));
    }
    std::sort(sorted.begin(), sorted.end(), comp);
    maxInMap = sorted[0].second;
  }

  void print(uint maxLines = 50, bool sort = true) {
    const uint maxw = 50;
    
    if (sort) {
      sort_counts();
    } else {
      maxInMap = *(std::max_element(counts.begin(), counts.end()));
    }

    float factor = maxInMap >= maxw ? (float)maxInMap / (float)maxw : 1.0;
    std::cout << std::string(maxw, '-') << '\n';
    for (auto m = sorted.begin();
         m != sorted.end() && (std::distance(sorted.begin(), m) != maxLines);
         ++m) {
      std::cout << static_cast<unsigned>(m->first) << "\t| " << m->second
                << "\t| " << std::string((int)((float)m->second / factor), '*')
                << '\n';
    }
    std::cout << "total : " << total << '\n';
  }

  std::vector<T> counts, counts_norm;
  std::vector<uint32_t> cumul, cumul_norm;
  std::vector<pair> sorted;
  size_t total;
  size_t maxInMap;
};