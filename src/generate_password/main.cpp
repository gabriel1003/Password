#include <iostream>
#include <string>
#include "generator_password.h"

using namespace std;
int main() {
    std::string password = generator_password();
    std::cout << "This is your password: " << password << std::endl;
    return 0;
}