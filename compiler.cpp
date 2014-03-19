#include "compiler.h"

using namespace W;
Compiler::Compiler(std::string filename) :
    Parser(filename)
{
    ignoreBlanks();
}

Compiler::~Compiler()
{

}

bool Compiler::function()
{
    std::string type;
    if(lock() && !readIdentifier(type))
        return unlock();

    if(type != "function")
    {
        unlock();
        return false;
    }

    std::string name;
    if(!readIdentifier(name))
    {
        cout << "erreur de syntaxe, nom de fonction incorrecte" << endl;
        return false;
    }

    cout << "function " << name << endl;

    if(!blockFunction())
    {
        cout << "incorrect definition of the function " << name << endl;
        return false;
    }


    return true;
}

bool Compiler::blockFunction()
{
    if(!readText("("))
        return false;

    std::string value;
    float flValue;
    bool state = blockFunction() || readIdentifier(value) || readFloat(flValue) || readText(",")

    return true;
}

bool Compiler::blockFunction2()
{
    return false;
}
