
#include <iostream>

#include "aeroarch/core/version.hpp"
namespace aeroarch
{
    void print_welcome()
    {
        std::cout << "================================\n";
        std::cout << "          " << VERSION_NAME << " OS\n";
        std::cout << "================================\n";

        std::cout << "Version: "
                  << VERSION_MAJOR << "."
                  << VERSION_MINOR << "."
                  << VERSION_PATCH << '\n';

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
