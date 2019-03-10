#pragma once

#include "command.h"
#include <chrono>
#include <vector>

// ----------------------------------------------------------------------------

struct IProcessor
{
  virtual void push(std::chrono::system_clock::time_point time, const std::vector<Command>&) = 0;
  virtual void push_one(std::chrono::system_clock::time_point time, const Command&) = 0;
  virtual void commit() = 0;
  virtual ~IProcessor(){}
};

// ----------------------------------------------------------------------------

