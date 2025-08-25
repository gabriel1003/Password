#include "generator_password.h"
#include <string>

std::string GeneratorPassword::generatorPassword() {
    std::string password = "";
    for (int i = 0; i < 13; ++i) {
        int randonType = generateNumber.generateAndGetValue() % 4;
        switch (randonType) {
            case 0:
                password += letterLowercase.generateAndGetLetter();
            break;
            case 1:
                password += letterUpercase.generateAndGetLetter();
            break;
            case 2:
                password += character.generateAndGetCharacter();
            break;
            case 3:
                password += hexadecimalValue.generateAndGetHexadecimalValue();
            break;
        }
    }

    return password;
}