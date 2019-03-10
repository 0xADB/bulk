#pragma once

#include "processor.h"

#include <iostream>
#include <iomanip>
#include <sstream>

// ----------------------------------------------------------------------------

struct ConsolePrinter : public IProcessor
{
  virtual void push(std::chrono::system_clock::time_point, const std::vector<Command>& cmds) final
  {
    if (!cmds.empty())
    {
      commit();

      std::ostringstream oss;
      auto cmdIt = std::begin(cmds);

      oss << "bulk: " << cmdIt->value.c_str();
      while (++cmdIt != std::end(cmds))
	oss << ", " << cmdIt->value.c_str();

      std::cout << oss.str().c_str() << std::endl;
    }
  }

  void push_one(std::chrono::system_clock::time_point, const Command& cmd)
  {
    if (!_oss.tellp())
    {
      _oss << "bulk: ";
    }
    else
      _oss << ", ";
    _oss << cmd.value.c_str();
  }

  virtual void commit() final
  {
    if (_oss.tellp())
    {
      std::cout << _oss.str().c_str() << std::endl;
      _oss.str("");
      _oss.clear();
    }
  }

  virtual ~ConsolePrinter()
  {
    ConsolePrinter::commit();
  }

  std::ostringstream _oss;
};

// ----------------------------------------------------------------------------
