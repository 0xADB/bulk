#pragma once

#include "processor.h"

#include <iostream>
#include <iomanip>
#include <sstream>

// ----------------------------------------------------------------------------

struct ConsolePrinter : public IProcessor
{
  virtual void push(std::chrono::system_clock::time_point, const Command& cmd) final
  {
    if (!out.tellp())
      out << "bulk: ";
    else
      out << ", ";
    out << cmd.value.c_str();
  }

  virtual void commit() final
  {
    if (out.tellp())
    {
      std::cout << out.str().c_str() << std::endl;;
      out.str("");
      out.clear();
    }
  }

  virtual ~ConsolePrinter()
  {
    ConsolePrinter::commit();
  }

  std::ostringstream out;
};

// ----------------------------------------------------------------------------
