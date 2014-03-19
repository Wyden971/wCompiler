#ifndef PARSER_H
#define PARSER_H

#define MAX_BUFFER_SIZE 1048576

#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <list>
#include <cmath>

using namespace std;

namespace W
{
class Parser
{
public:
    Parser(std::string fName);
    ~Parser();

    bool open(std::string fName = std::string());
    void close();

    bool flush();
    bool forceFlush();
    bool isDone();

    bool rewind();
    bool seek(long);
    long tell();

    bool pop();
    bool getChar(char & c);
    bool readChar(char & c);
    bool readBlanks();
    bool readIdentifier(std::string & value);
    bool readText(std::string value);
    bool readInteger(int& value);
    bool readFloat(float& value);
    bool readCString(std::string& value);
    bool readPythonString(std::string& value);
    bool readWithout(std::string text, std::string& value);
    bool readWith(std::string text, std::string& value);
    bool readUptoIgnore();

    bool lock();
    bool unlock();

    void ignoreBlanks();

private:
    std::string         filename;
    FILE*               file;
    std::list<char>     list;
    long                cursor;
    long                lockedCursor;
    bool                blanks;
};
}
#endif // PARSER_H
