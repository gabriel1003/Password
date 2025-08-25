#include "generate_number.h"

GenerateNumber::GenerateNumber() : distributor(0, 25) {
    generator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
int GenerateNumber::generateAndGetValue() {
    return distributor(generator);
}