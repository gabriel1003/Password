#include <iostream>
#include <string>
#include "generator_password.h"

using namespace std;
int main() {
    GeneratorPassword password;
    string getPassword = password.generatorPassword();
    cout << "this is a your password: " << getPassword << endl;
    return 0;
}