#include "generate_letter_lowercase.h"
#include <string>

char LetterLowercase::generateAndGetLetter() {
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    int index = generateNumber.generateAndGetValue();
    return alphabet[index];
}