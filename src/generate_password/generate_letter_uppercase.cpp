#include "generate_letter_uppercase.h"
#include <string>

LetterUpercase::LetterUpercase() {
}

char LetterUpercase::generateAndGetLetter() {
    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int index = generateNumber.generateAndGetValue();
    return alphabet[index];
}