#include "block.h"

Block::Block(Namespace *arg_ns):
    ns(arg_ns), terminate(false), conditional(false), next(nullptr), alter(nullptr) {}

Block::~Block()
{
    for (Action *action : actions) delete action;
}