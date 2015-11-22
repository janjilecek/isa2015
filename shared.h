#ifndef SHARED
#define SHARED

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>

class ISAException : public std::runtime_error
{
private:
    std::string m_str;
public:
    ISAException(std::string instr) : std::runtime_error(instr.c_str()), m_str(instr)
    {
    }

    std::string what()
    {
        return m_str;
    }
};


#endif // SHARED
