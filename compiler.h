#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"

namespace W
{
class Compiler : public Parser
{

public:
    Compiler(std::string filename);
    ~Compiler();

    bool function();
    bool blockFunction();
    bool blockFunction2();
};
};

#endif // COMPILER_H
