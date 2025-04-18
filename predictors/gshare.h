#include <algorithm>
#include <array>
#include <assert.h>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "base_predictor.h"

#ifndef PERCEPTRON_PREDICTOR_H
#define PERCEPTRON_PREDICTOR_H

#define INDEX_SIZE_BITS 20
#define HISTORY_LENGTH 20
#define SIZE_OF_COUNTERS 2

#define TABLE_SIZE (1 << INDEX_SIZE_BITS)

/*
 * Memory = size of counters * table size
 */

class Gshare : public BranchPredictor {
public:
  Gshare(void) : table({0}) {}

  std::pair<GHR_t, int32_t> predict_branch(uint64_t pc,
                                           GHR_t ghr) const override {
    uint64_t index = calculate_index(pc, ghr);
    return std::make_pair(ghr, table[index]);
  }

  void correct_state(uint64_t pc, GHR_t ghr, int32_t prediction,
                     bool actual) override {
    uint64_t index = calculate_index(pc, ghr);
    if ((prediction >= 0) != actual) {
      if (actual) {
        table[index] = clamp(table[index] + 1, SIZE_OF_COUNTERS);
      } else {
        table[index] = clamp(table[index] - 1, SIZE_OF_COUNTERS);
      }
    }
  };

  uint64_t size() const override { return 0; }

private:
  uint64_t calculate_index(uint64_t pc, GHR_t ghr) const {
    std::bitset<GHR_LENGTH> bitsetpc(pc);
    uint64_t index = (bitsetpc ^ ghr).to_ulong() & ((1 << INDEX_SIZE_BITS) - 1);
    return index;
  }

  int64_t clamp(int64_t value, uint64_t bits) {
    int64_t max = ((1 << bits) - 1) - (1 << (bits - 1));
    int64_t min = max - (1 << bits) + 1;
    return std::clamp(value, min, max);
  }

  std::array<int64_t, TABLE_SIZE> table;
};

#endif
