#include "generator_password.h"
#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <string_view>

auto generate_number = []() -> char {
    return (char) rand() % ('9' - '0') + '0';
};

auto generate_letter_lower = []() -> char {
    return (char) rand() % ('z' - 'a') + 'a';
};

auto generate_letter_upper = []() -> char {
    return (char) rand() % ('Z' - 'A') + 'A';
};

auto generate_symbols = []() -> char {
    std::string_view symbol_list = ":/?!@#$%^+=&";
    return (char) symbol_list[rand() % symbol_list.length()];
};
 
std::string generator_password() {
    srand(time(NULL));
    std::vector<std::function<char(void)>> rand_funcs{
        generate_number,
        generate_letter_lower,
        generate_letter_upper,
        generate_symbols
    };
    std::string resulting_password;
    for (int i = 0; i < 14; i++) {
        auto next_idx = rand() % rand_funcs.size();
        resulting_password += rand_funcs[next_idx]();
    }
    return resulting_password;
}