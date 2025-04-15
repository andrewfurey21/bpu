#include <bitset>
#include <cstdint>
#include <map>
#include <utility>

#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#define TAKEN true
#define NOT_TAKEN !TAKEN

// TODO: make on config file
#define GHR_LENGTH 64
typedef std::bitset<GHR_LENGTH> GHR_t;

class BranchPredictor {
public:
  bool predict(uint64_t pc, uint64_t seq_no, uint8_t piece) {
    auto key = std::make_pair(seq_no, piece);
    auto value = predict_branch(pc, ghr);
    ghr_vector[key] = value;
    return value.second >= 0;
  }

  void update(uint64_t pc, uint64_t seq_no, uint8_t piece, bool actual) {
    auto key = std::make_pair(seq_no, piece);
    int32_t prediction = ghr_vector[key].second;
    std::bitset old_ghr = ghr_vector[key].first;
    correct_state(pc, old_ghr, prediction, actual);
    ghr_vector.erase(key);
  }

  virtual uint64_t size() const = 0;

  void history_update(uint64_t pc, bool taken) {
    ghr <<= 1;
    ghr[0] = ghr[0] | taken;
  };

protected:
  virtual std::pair<GHR_t, int32_t> predict_branch(uint64_t pc, GHR_t ghr) = 0;
  virtual void correct_state(uint64_t pc, GHR_t ghr, int32_t prediction,
                             bool actual) = 0;

  std::map<std::pair<uint64_t, uint8_t>, std::pair<GHR_t, int32_t>> ghr_vector;

  GHR_t ghr;
};

#endif
