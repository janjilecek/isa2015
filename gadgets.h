#ifndef GADGETS_H
#define GADGETS_H
#include <string>
#endif // GADGETS_H

char* strToChar(std::string s)
{
    char *cstr = &s[0u];
    return cstr;
}
