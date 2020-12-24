#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#define INT_WIDTH 32

class Action;
bool verify_basic_path(std::vector<Action*> basic_path);
class VerificationError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
class SyntaxError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

template<class T>
std::string stringify(T x)
{
    std::ostringstream builder;
    builder << x;
    return builder.str();
}