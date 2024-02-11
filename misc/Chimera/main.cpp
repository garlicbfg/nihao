#include <iostream>
#include "Chimera.h"




int main()
{
    const std::string inputFile = "input.txt";
    std::cout << "Running Chimera\n";
    Chimera chimera;
    chimera.run(inputFile);
}
