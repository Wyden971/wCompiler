#include <iostream>
#include "compiler.h"
#define _BOOL(VALUE) (VALUE?"true":"false")

using namespace std;

int main()
{
    W::Compiler compiler("test.w");
    compiler.ignoreBlanks();
    compiler.function();
    return 0;
}
