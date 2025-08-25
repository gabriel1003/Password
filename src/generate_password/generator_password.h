#ifndef GENERATOR_PASSWORD_H
#define GENERATOR_PASSWORD_H
#include "generate_character.h"
#include "generate_hexadecimal_number.h"
#include "generate_letter_lowercase.h"
#include "generate_letter_uppercase.h"
#include "generate_number.h"

class GeneratorPassword {
private:
    LetterLowercase letterLowercase;
    LetterUpercase letterUpercase;
    GenerateCharacter character;
    GenerateHexadecimalNumber hexadecimalValue;
    GenerateNumber generateNumber;
public:
    std::string generatorPassword();
};

#endif