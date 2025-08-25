#include <vector>
#include <functional>
#include <iostream>

auto gen_number = []() -> char {
    return (char) rand() % ('9' - '0') + '0';
};

auto gen_letter_lower = []() -> char {
    return (char) rand() % ('z' - 'a') + 'a';
};

auto gen_letter_upper = []() -> char {
    return (char) rand() % ('Z' - 'A') + 'A';
};

auto gen_symbols = []() -> char {
    std::string_view symbol_list = "!@#$%^&";
    return (char) symbol_list[rand() % symbol_list.length()];
};

int main() {
    srand(time(NULL));

    std::vector<std::function<char(void)>> rand_funcs{
        gen_number,
        gen_letter_lower,
        gen_letter_upper,
        gen_symbols
    };

    std::string resulting_password;
    for (int i = 0; i < 16; i++) {
        auto next_idx = rand() % rand_funcs.size();
        resulting_password += rand_funcs[next_idx]();
    }

    std::cout << resulting_password << std::endl;
}
