#pragma once

#include "processor.h"

#include <iostream>
#include <iomanip>
#include <sstream>

// ----------------------------------------------------------------------------

struct ConsolePrinter : public IProcessor
{
  ConsolePrinter()
    : IProcessor()
  {
    std::cout << "input | output\n"
	      << "------|-------"
	      << std::endl;
  }

  virtual void push(std::chrono::system_clock::time_point, const Command& cmd) final
  {
    std::cout << cmd.value.c_str()
      << std::setw(cmd.value.size() > 5 ? 1 : 7 - cmd.value.size()) << "|"
      << std::endl;
    if (!out.tellp())
      out << "bulk: ";
    else
      out << ", ";
    out << cmd.value.c_str();
  }

  virtual void commit() final
  {
    std::cout << "      | " << out.str().c_str() << std::endl;;
    out.str("");
    out.clear();
  }

  virtual ~ConsolePrinter()
  {
    ConsolePrinter::commit();
  }

  std::ostringstream out;
};

// ----------------------------------------------------------------------------
