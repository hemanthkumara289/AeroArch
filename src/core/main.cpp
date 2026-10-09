
#include <iostream>

namespace aeroarch
{
    void print_welcome()
    {
        std::cout << "================================\n";
        std::cout << "          AeroArch OS\n";
        std::cout << "================================\n";
        std::cout << "Version: 0.1.0\n";
        std::cout << "Status: Development\n";
        std::cout << "Foundation: Linux (initially)\n";
        std::cout << "================================\n";
    }
}

int main()
{
    aeroarch::print_welcome();
    return 0;
}
