#include "generate_hexadecimal_number.h"
#include <sstream>
#include <iomanip>

std::string GenerateHexadecimalNumber::generateAndGetHexadecimalValue() {
    std::stringstream stream;

    int decimalValue = generateNumber.generateAndGetValue() % 16;
    stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << decimalValue;
    return stream.str();
}