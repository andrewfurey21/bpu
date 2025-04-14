#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unordered_map>

#include "../tage.h"

class Predictor {
  TAGE predictor;

public:
  Predictor(void) {}
  void setup() {}
  void terminate() {}

  bool predict(uint64_t /* seq_no */, uint8_t /* piece */, uint64_t PC,
               const bool /* tage_pred */) {
    return predictor.predict(PC);
  }

  void history_update(uint64_t /* seq_no */, uint8_t /* piece */, uint64_t PC,
                      bool taken, uint64_t /* nextPC */) {
    predictor.history_update(PC, taken);
  }

  void update(uint64_t /* seq_no */, uint8_t /* piece */, uint64_t PC,
              bool resolveDir, bool predDir, uint64_t /* nextPC */) {
    predictor.update(PC, predDir, resolveDir);
  }
};

#endif
static Predictor cond_predictor_impl;
