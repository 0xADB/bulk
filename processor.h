#pragma once

#include "command.h"
#include <chrono>

// ----------------------------------------------------------------------------

struct IProcessor
{
  virtual void push(std::chrono::system_clock::time_point time, const Command&) = 0;
  virtual void commit() = 0;
  virtual ~IProcessor(){}
};

// ----------------------------------------------------------------------------

