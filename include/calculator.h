#pragma once
#include <string>

class Calculator {

public:
    void draw(); // draws calculator window

private:
    bool open = true;
    bool op_active = false;
    const char *oper = "";
    std::string input1 = "";
    std::string input2 = "";
    double res = 0;
    bool result_shown = false;
};