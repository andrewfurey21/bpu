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

#define INDEX_SIZE_BITS 10
#define NUM_OUTPUT_BITS 16
#define NUM_WEIGHT_BITS 8

/*
 * Memory =
 *          (2^(index size in bits)) * size of entry
 * Size of entry = GHR size * size of weights + size of output
*/

#define THRESHOLD ((int32_t)floor(1.93f * GHR_LENGTH + 14))
#define NUM_OF_WEIGHTS (GHR_LENGTH + 1)
#define TABLE_SIZE (1 << INDEX_SIZE_BITS)
#define WEIGHT_TYPE int64_t
#define MAX_WEIGHT (WEIGHT_TYPE)((1 << NUM_WEIGHT_BITS) - 1) / 2
#define MIN_WEIGHT (WEIGHT_TYPE)(-MAX_WEIGHT - 1)
#define MAX_OUTPUT (WEIGHT_TYPE)((1 << NUM_OUTPUT_BITS) - 1) / 2
#define MIN_OUTPUT (WEIGHT_TYPE)(-MAX_OUTPUT - 1)

class Perceptron : public BranchPredictor {
public:
  Perceptron(void) : table({0}) {}

  std::pair<GHR_t, int32_t> predict_branch(uint64_t pc, GHR_t ghr) const override {
    uint64_t index = hash(pc, ghr);
    int32_t prediction = 0;
    prediction += table[index][0];
    for (int i = 1; i < NUM_OF_WEIGHTS; i++) {
      if (ghr[i - 1])
        prediction =
            std::clamp(prediction + table[index][i], MIN_OUTPUT, MAX_OUTPUT);
      else
        prediction =
            std::clamp(prediction - table[index][i], MIN_OUTPUT, MAX_OUTPUT);
    }
    return std::make_pair(ghr, prediction);
  }

  void correct_state(uint64_t pc, GHR_t ghr, int32_t prediction,
                     bool actual) override {
    if ((prediction >= 0) != actual || abs(prediction) <= THRESHOLD) {
      int index = hash(pc, ghr);
      int64_t sign_actual = actual ? 1 : -1;

      table[index][0] =
          std::clamp(table[index][0] + sign_actual, MIN_WEIGHT, MAX_WEIGHT);
      for (int i = 1; i < NUM_OF_WEIGHTS; i++) {
        int32_t sign_history = ghr[i - 1] ? 1 : -1;
        table[index][i] =
            std::clamp(table[index][i] + sign_actual * sign_history, MIN_WEIGHT,
                       MAX_WEIGHT);
      }
    }
  };

  uint64_t size() const override { return 0; }

private:
  uint64_t folded_history(GHR_t ghr) const {
    uint64_t fghr = 0;
    for (int i = 0; i < GHR_LENGTH; i += INDEX_SIZE_BITS) {
      fghr ^= (TABLE_SIZE - 1) & (ghr >> i).to_ulong();
    }
    return fghr;
  }

  uint64_t hash(uint64_t pc, GHR_t ghr) const {
    return (pc % TABLE_SIZE) ^ folded_history(ghr);
  }

  std::array<std::array<WEIGHT_TYPE, NUM_OF_WEIGHTS>, TABLE_SIZE> table;
};

#endif
