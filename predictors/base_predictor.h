#include <cstdint>

#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

class BranchPredictor {
public:
    virtual bool predict(uint64_t PC) const = 0;
    // TODO: double check taken here means resolveDir
    virtual void history_update(uint64_t PC, bool taken) = 0;
    virtual void update(uint64_t PC, bool prediction, bool taken) = 0;
};

#endif
