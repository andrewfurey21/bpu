#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <assert.h>

#include "base_predictor.h"

#ifndef PERCEPTRON_PREDICTOR_H
#define PERCEPTRON_PREDICTOR_H

#define INDEX_SIZE_BITS 10
// TODO: size of each weight

#define NUM_OF_WEIGHTS (GHR_LENGTH + 1)
#define THRESHOLD ((int32_t)floor(1.93f * GHR_LENGTH + 14))
#define TABLE_SIZE (1 << INDEX_SIZE_BITS)

class Perceptron : public BranchPredictor {
public:
  Perceptron(void) : table({0}) {}

  bool predict(uint64_t pc, uint64_t seq_no, uint8_t piece) override {
    // std::cout << "Predict Seq no: " << seq_no << "\n";

    uint64_t index = hash(pc, ghr);
    int32_t prediction = 0;

    prediction += table[index][0]; // bias
    for (int i = 1; i < NUM_OF_WEIGHTS; i++) {
      if (ghr[i - 1])
        prediction += table[index][i];
      else
        prediction -= table[index][i];
    }

    auto key = std::make_pair(seq_no, piece);
    auto value = std::make_pair(ghr, prediction);
    ghr_vector[key] = value;

    return prediction >= 0;
  }

  void update(uint64_t pc, uint64_t seq_no, uint8_t piece, bool actual) override {
    // std::cout << "Update Seq no: " << seq_no << "\n";
    auto key = std::make_pair(seq_no, piece);
    int32_t prediction = ghr_vector[key].second;
    std::bitset old_ghr = ghr_vector[key].first;

    if ((prediction >= 0) != actual || abs(prediction) <= THRESHOLD) {
      int index = hash(pc, old_ghr);
      int32_t sign_actual = actual ? 1 : -1;

      table[index][0] += sign_actual;
      for (int i = 1; i < NUM_OF_WEIGHTS; i++) {
        int32_t sign_history = old_ghr[i - 1] ? 1 : -1;
        table[index][i] += sign_actual * sign_history;
      }
    }
    ghr_vector.erase(key);
  };

  uint64_t size() const override {
    return 0;
  }

private:
  uint64_t folded_history(std::bitset<GHR_LENGTH> ghr) const {
    uint64_t fghr = 0;
    for (int i = 0; i < GHR_LENGTH; i += INDEX_SIZE_BITS) {
      fghr ^= (TABLE_SIZE - 1) & (ghr >> i).to_ulong();
    }
    return fghr;
  }

  uint64_t hash(uint64_t pc, std::bitset<GHR_LENGTH> current_ghr) const {
    return (pc % TABLE_SIZE) ^ folded_history(current_ghr);
  }

  std::array<std::array<int32_t, NUM_OF_WEIGHTS>, TABLE_SIZE> table;
};

#endif
