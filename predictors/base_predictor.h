#include <bitset>
#include <cstdint>
#include <map>

#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#define TAKEN true
#define NOT_TAKEN !TAKEN

// TODO: make on config file
#define GHR_LENGTH 64

class BranchPredictor {
public:
  virtual bool predict(uint64_t pc, uint64_t seq_no, uint8_t piece) = 0;
  virtual void update(uint64_t pc, uint64_t seq_no, uint8_t piece,
                      bool actual) = 0;
  virtual uint64_t size() const = 0;

  // this updates the ghr speculatively.
  void history_update(uint64_t pc, bool taken) {
    ghr <<= 1;
    ghr[0] = ghr[0] | taken;
  };

protected:
  std::map<std::pair<uint64_t, uint8_t>,
           std::pair<std::bitset<GHR_LENGTH>, int32_t>>
      ghr_vector;

  std::bitset<GHR_LENGTH> ghr;
};

#endif
