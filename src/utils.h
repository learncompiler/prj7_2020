#pragma once

#include <vector>
#include <stdexcept>
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