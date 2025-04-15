#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unordered_map>

#include "../predictors/perceptron.h"

class Predictor {
  Perceptron predictor;

public:
  Predictor(void) {}
  void setup() {}
  void terminate() {}

  bool predict(uint64_t seq_no, uint8_t piece, uint64_t pc,
               const bool /* tage_pred */) {
    return predictor.predict(pc, seq_no, piece);
  }

  void history_update(uint64_t seq_no, uint8_t piece, uint64_t pc, bool taken,
                      uint64_t /* nextpc */) {
    predictor.history_update(pc, taken);
  }

  void update(uint64_t seq_no, uint8_t piece, uint64_t pc, bool resolveDir,
              bool /* predDir */, uint64_t /* nextpc */) {
    predictor.update(pc, seq_no, piece, resolveDir);
  }

};

#endif
static Predictor cond_predictor_impl;
