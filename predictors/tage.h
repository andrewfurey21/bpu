#include <array>
#include <assert.h>
#include <bitset>
#include <cstdint>
#include <math.h>
#include <utility>
#include <vector>

#include "base_predictor.h"

#define NUM_OF_TABLES 5
#define USEFUL_COUNTER_SIZE 1
#define COUNTER_SIZE 2
#define INDEX_SIZE_BITS 10
#define TAG_SIZE_BITS 12

#define SCALE 2
#define BASE_LENGTH 4

/*
 * Memory = number of tables *
 *          (2^(index size in bits)) * size of entry
 *          + size of base entry * number of base entries
 * Size of entry = size of counter + size of tag + size of usefulness
 */

#define TABLE_SIZE (1 << INDEX_SIZE_BITS)
#define TAG_SIZE (1 << TAG_SIZE_BITS)

#ifndef TAGE_PREDICTOR_H
#define TAGE_PREDICTOR_H

class TAGE : public BranchPredictor {
public:
  TAGE() {}

  std::pair<GHR_t, int32_t> predict_branch(uint64_t pc,
                                           GHR_t ghr) const override {
    for (int i = 0; i < NUM_OF_TABLES; i++) {
      uint64_t len = find_length(SCALE, i + 1, BASE_LENGTH);
      uint64_t index = hash(pc, ghr, len);
      uint64_t t = tag(pc, ghr, TAG_SIZE);

    }
  }

  void correct_state(uint64_t pc, GHR_t ghr, int32_t prediction,
                     bool actual) override {}

  uint64_t size() const override { return 0; }

private:
  int provider_component;
  int alt_provider;
  int use_alt_on_na;
  int provider_table_index;

  bool provider_pred;
  bool alt_provider_pred;

  typedef struct {
    uint32_t counter;
    std::bitset<TAG_SIZE> tag;
    int usefulness;
  } table_entry;

  typedef std::array<table_entry, TABLE_SIZE> table;

  std::array<uint32_t, TABLE_SIZE> base_table;
  std::array<table, NUM_OF_TABLES> tables;

  uint64_t folded_history(GHR_t ghr, uint64_t length,
                          uint64_t table_index_size) const {
    assert(!(table_index_size & (table_index_size - 1)));
    uint64_t folded = 0;
    for (int i = 0; i < length; i += table_index_size) {
      folded ^= (ghr >> i).to_ulong() & ((1 << table_index_size) - 1);
    }
    return folded;
  }

  uint64_t hash(uint64_t pc, GHR_t ghr, uint64_t length) const {
    uint64_t fh = folded_history(ghr, length, INDEX_SIZE_BITS);
    return fh ^ (pc % TABLE_SIZE);
  }

  uint64_t tag(uint64_t pc, GHR_t ghr, uint64_t length) const {
    uint64_t fh = folded_history(ghr, length, TAG_SIZE_BITS);
    return fh ^ (pc % TAG_SIZE);
  }

  void set_usefulness(uint64_t pc, bool v) {
    tables[provider_table_index][provider_component].usefulness = v;
  }

  uint64_t find_length(uint64_t alpha, uint64_t table_index,
                       uint64_t base_length) const {
    assert(table_index > 1);
    return pow(alpha, table_index - 1) * base_length;
  }

  int get_base_table_prediction(uint64_t pc) const {
    return base_table[pc & (TABLE_SIZE - 1)];
  }
};

#endif
