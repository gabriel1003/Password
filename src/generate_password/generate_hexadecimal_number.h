#ifndef GENERATE_HEXADECIMAL_NUMBER_H
#define GENERATE_HEXADECIMAL_NUMBER_H
#include <string>
#include "generate_number.h"

class GenerateHexadecimalNumber {
private:
    GenerateNumber generateNumber;

public:
    std::string generateAndGetHexadecimalValue();
};

#endif