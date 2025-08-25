#ifndef GENERATE_NUMBER_H
#define GENERATE_NUMBER_H

#include <random>
#include <chrono>

class GenerateNumber {
private:
    std::mt19937 generator;
    std::uniform_int_distribution<int> distributor;

public:
    GenerateNumber();
    int generateAndGetValue();
};

#endif // GENERATE_NUMBER_H