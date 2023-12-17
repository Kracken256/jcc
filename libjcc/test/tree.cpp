#include "parser.hpp"
#include <string>
#include <iostream>

int main()
{
    using namespace jcc;

    ASTNode ast;

    ast["a"]["b"] = 0;
    ast["a"]["c"]["d"] = 1;
    ast["a"]["c"]["e"] = 2;

    std::cout << ast << std::endl;


    ast.invert();

    std::cout << ast << std::endl;
}