#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>


class BadFormatException : public std::exception
{
public:
    BadFormatException(const std::string &what)
        : m_what(what) {}

    const char *what() const throw () override { return m_what.c_str(); }

private:
    std::string m_what;

};


struct UnexpectedEofException : std::exception {};

struct CantReadException : std::exception {};

#endif // EXCEPTIONS_H
