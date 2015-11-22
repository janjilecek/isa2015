#ifndef SHARED
#define SHARED

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>

/**
 * @brief The ISAException class
 */
class ISAException : public std::runtime_error
{
private:
    std::string m_str;
public:
    ISAException(std::string instr) : std::runtime_error(instr.c_str()), m_str(instr)
    {
    }

    /**
     * @brief what
     * @return
     */
    std::string what()
    {
        return m_str;
    }

    virtual ~ISAException() throw() {}
};


#endif // SHARED
