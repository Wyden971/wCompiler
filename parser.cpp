#include "parser.h"

using namespace W;

Parser::Parser(std::string fName) :
    filename(fName),
    file(NULL),
    cursor(0),
    lockedCursor(0),
    blanks(false)
{
    open();
}

Parser::~Parser()
{
    close();
}

bool Parser::open(std::string fName)
{
    close();

    if(fName.empty())
        fName = filename;

    if(fName.empty())
        return false;

    file = fopen(fName.c_str(), "r+");
    if(file == NULL)
        return false;

    filename = fName;
    cursor = 0;
    lockedCursor = 0;
    blanks = false;

    return true;
}

void Parser::close()
{
    if(file == NULL)
        return;

    fclose(file);
    file = NULL;
    cursor = 0;
    lockedCursor = 0;
}

bool Parser::flush()
{
    if(file == NULL)
        return false;

    if(feof(file))
        return !list.empty();

    unsigned bSize = MAX_BUFFER_SIZE - list.size();

    if(bSize == 0)
        return true;

    char* buf = new char[bSize];

    int len = fread(buf, 1, bSize, file);
    if(len <= 0)
    {
        return false;
        delete[] buf;
    }

    for(int i = 0; i<len; i++)
        list.push_back(buf[i]);


    delete[] buf;
    return true;
}

bool Parser::forceFlush()
{
    list.clear();
    return flush();
}

bool Parser::isDone()
{
    return feof(file) && list.empty();
}

bool Parser::rewind()
{
    if(file == NULL)
        return false;

    ::rewind(file);
    return true;
}

bool Parser::seek(long cur)
{
    if(file == NULL)
        return false;

    if(cur < 0)
        cur = 0;

    cursor = cur;
    lockedCursor = cursor;

    fseek(file, cursor, SEEK_SET);

    return forceFlush();
}

long Parser::tell()
{
    if(file == NULL)
        return -1;

    return cursor;
}


bool Parser::pop()
{
    if(!flush())
        return false;

    if(list.empty())
        return false;

    list.pop_front();
    cursor++;
    return true;
}

bool Parser::getChar(char & c){
    if(!flush())
        return false;
    c = list.front();
    return true;
}

bool Parser::readChar(char & c)
{
    if(getChar(c)){
        pop();
        return true;
    }
    return false;
}

bool Parser::readBlanks()
{
    if(!flush())
        return false;

    char c;
    bool state = false;
    while(getChar(c))
    {
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            pop();
        else
            break;
        state = true;
    }
    return state;
}

bool Parser::readIdentifier(std::string & value)
{
    if(!flush())
        return false;

    if(blanks)
        readBlanks();

    value = "";
    char c;
    while(getChar(c))
    {
        if(value.empty())
        {
            if((c>='a' && c<='z') || (c>='A'&&c<='Z') || c == '_')
            {
                pop();
                value+=c;
                continue;
            }
            else return false;
        }

        if((c>='a' && c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c == '_')
        {
            value+=c;
            pop();
        }
        else
            break;
    }

    return true;
}

bool Parser::readText(std::string value)
{
    if(!flush())
        return false;

    if(value.empty())
        return false;

    if(blanks)
        readBlanks();

    char c;
    unsigned i = 0;
    long lastCursor = cursor;
    while(i < value.size() && readChar(c) && value[i] == c)
        i++;

    if(i != value.size())
    {
        seek(lastCursor);
        return false;
    }
    getChar(c);
    return true;
}

bool Parser::readInteger(int& value)
{
    if(!flush())
        return false;

    if(blanks)
        readBlanks();

    char c;
    std::string number;
    while(getChar(c)){
        if(c >= '0' && c <= '9')
        {
            number+=c;
            pop();
        }
        else
            break;
    }
    if(number.empty())
        return false;
    value = 0;
    for(unsigned i = 0; i<number.size(); i++){
        int j = number[i]-'0';
        value+= j*pow(10, (number.size()-i-1));
    }
    return true;
}

bool Parser::readFloat(float& value)
{
    if(!flush())
        return false;

    if(blanks)
        readBlanks();

    int tmp, tmp2;
    bool s1, s2, s3;

    value = 0;
    long lastCursor = cursor;

    s1 = readInteger(tmp);
    s2 = readText(".");
    s3 = readInteger(tmp2);

    if(!s1 && !s3)
    {
        seek(lastCursor);
        return false;
    }

    if(s1)
    {
        value = tmp;
    }
    if(s2 && s3)
    {
        float fVal = tmp2;
        while((tmp2=fVal)>0)
            fVal/=10;
        value+=fVal;
    }
    return true;
}
bool Parser::readCString(std::string& value)
{
    if(blanks)
        readBlanks();

    value = "";
    long lastCursor = cursor;
    bool state = false;
    if(readText("\""))
    {
        char c;
        bool escape = false;
        while(readChar(c))
        {
            if(escape)
            {
                switch(c)
                {
                    case 'n':
                        value+='\n';
                        break;
                    case 't':
                        value+='\t';
                        break;
                    case 'r':
                        value+='\r';
                        break;
                    case '\\':
                        value+='\\';
                        break;
                    default:
                        value+='\\';
                        value+=c;
                }
                escape = false;
                continue;
            }

            if(c == '\\')
            {
                escape = true;
                continue;
            }
            else if(c == '"')
            {
                break;
                state = true;
            }
            else
                value+=c;
        }
    }

    if(!state)
    {
        seek(lastCursor);
        value = "";
    }

    return state;
}

bool Parser::readPythonString(std::string& value)
{
    if(blanks)
        readBlanks();

    value = "";
    int lastCursor = cursor;
    bool state = false;
    if(readText("'"))
    {
        char c;
        bool escape = false;
        while(readChar(c))
        {
            if(escape)
            {
                switch(c)
                {
                    case 'n':
                        value+='\n';
                        break;
                    case 't':
                        value+='\t';
                        break;
                    case 'r':
                        value+='\r';
                        break;
                    case '\\':
                        value+='\\';
                        break;
                    default:
                        value+='\\';
                        value+=c;
                }
                escape = false;
                continue;
            }

            if(c == '\\')
            {
                escape = true;
                continue;
            }
            else if(c == '\'')
            {
                break;
                state = true;
            }
            else
                value+=c;
        }
    }

    if(!state)
    {
        seek(lastCursor);
        value = "";
    }

    return state;
}

bool Parser::readWithout(std::string text, std::string& value)
{
    long lastCursor = cursor;
    char c;
    bool state = false;
    value = "";
    while(!isDone())
    {
        long cCursor = cursor;
        if(readText(text))
        {
            seek(cCursor);
            state = true;
            break;
        }

        if(readChar(c))
        {
            value+=c;
        }
    }
    if(!state)
    {
        value = "";
        seek(lastCursor);
        return false;
    }
    return true;
}

bool Parser::readWith(std::string text, std::string& value)
{
    long lastCursor = cursor;
    char c;
    bool state = false;
    value = "";
    while(!isDone())
    {
        if(readText(text))
        {
            state = true;
            break;
        }
        else
        {
            if(readChar(c))
                value+=c;
        }
    }
    if(!state)
    {
        value = "";
        seek(lastCursor);
        return false;
    }
    value+=text;
    return true;
}

bool Parser::readUptoIgnore()
{
    char c;
    return readChar(c);
}

bool Parser::lock()
{
    lockedCursor = cursor;
    return true;
}

bool Parser::unlock()
{
    seek(lockedCursor);
    return false;
}

void Parser::ignoreBlanks()
{
    blanks = true;
}
