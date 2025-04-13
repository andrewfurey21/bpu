#include <bitset>
#include <cstdint>
#include <array>
#include <vector>

#define GHIST_SIZE 128

#define USEFUL_COUNTER_SIZE 1
#define TAG_SIZE 10

#define NUM_OF_TABLES 5

#define BASE_COUNTER_SIZE 2

typedef struct {
    uint32_t counter;
    std::bitset<TAG_SIZE> tag;
    std::bitset<USEFUL_COUNTER_SIZE> usefulness;
} table_entry;

typedef std::vector<table_entry> table;

class TAGE {
public:
    bool compute_prediction(uint32_t program_counter) {
        return false;
    }

private:
    std::bitset<GHIST_SIZE> history;
    uint32_t provider_index;
    uint32_t alt_pred_index;

    bool provider_prediction;
    bool alt_pred_prediction;

    std::array<table, NUM_OF_TABLES> tables;
};
