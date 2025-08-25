#ifndef GENERATE_LETTER_UPERCASE_H
#define GENERATE_LETTER_UPERCASE_H
#include "generate_number.h"

class LetterUpercase {
private:
    GenerateNumber generateNumber;

    public:
    LetterUpercase();
    char generateAndGetLetter();
};

#endif // GENERATE_LETTER_UPPERCASE_H
