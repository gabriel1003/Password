#include "generate_character.h"
#include <string>

char GenerateCharacter::generateAndGetCharacter() {
    const std::string characters = ".:?/!@#_+,*&;-";
    int index = generateNumber.generateAndGetValue();
    return characters[index % characters.length()];
}