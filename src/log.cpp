#include <utility>
#include <iostream>
#include "log.h"

std::ostream* Log::out = &std::cerr;
LogStream<0> Log::debug;
LogStream<1> Log::info;
LogStream<2> Log::warning;
LogStream<3> Log::error;