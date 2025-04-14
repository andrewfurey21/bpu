#include <bitset>
#include <cstdint>
#include <array>
#include <vector>

#include "branch_predictor.h"

#define GHIST_SIZE 128
#define USEFUL_COUNTER_SIZE 1
#define TAG_SIZE 10
#define NUM_OF_TABLES 5
#define BASE_COUNTER_SIZE 2

#ifndef TAGE_PREDICTOR_H
#define TAGE_PREDICTOR_H

typedef struct {
    uint32_t counter;
    std::bitset<TAG_SIZE> tag;
    std::bitset<USEFUL_COUNTER_SIZE> usefulness;
} table_entry;

typedef std::vector<table_entry> table;

class TAGE : public BranchPredictor {
public:
    bool predict(uint64_t PC) const override {
        return true;
    }

    void history_update(uint64_t PC, bool taken) override {
    };

    void update(uint64_t PC, bool prediction, bool taken) override {
    };

private:
    std::bitset<GHIST_SIZE> history;
    uint32_t provider_index;
    uint32_t alt_pred_index;

    bool provider_prediction;
    bool alt_pred_prediction;

    std::array<table, NUM_OF_TABLES> tables;
};

#endif
