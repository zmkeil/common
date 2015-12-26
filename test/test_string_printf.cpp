#include <iostream>
#include "string_printf.h"

int main()
{
    std::string str;
    str.reserve(2048);
    
    common::string_appendf(&str, "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello");

    std::cout << str << std::endl;

    common::string_appendn(&str, "\nworld\nworld", 6);
    std::cout << str << std::endl;

    return 0;
}
