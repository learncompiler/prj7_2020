#pragma once
#define CUSTOM_LOG_LEVEL 1

#include <type_traits>
#include <ostream>

class EnabledLogStream;
class DisabledLogStream;
template<int level>
using LogStream = std::conditional_t<(level >= CUSTOM_LOG_LEVEL), EnabledLogStream, DisabledLogStream>;

class Log
{
public:
    static std::ostream *out;
    static LogStream<0> debug;
    static LogStream<1> info;
    static LogStream<2> warning;
    static LogStream<3> error;
};

class EnabledLogStream
{
public:
    template<class T>
    EnabledLogStream& operator<<(T &&data)
    {
        (*Log::out) << std::forward<T>(data);
        return *this;
    }
};
class DisabledLogStream
{
public:
    template<class T>
    DisabledLogStream& operator<<(T &&data)
    {
        return *this;
    }
};
