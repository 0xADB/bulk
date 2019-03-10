#pragma once

#include "stream_printer.h"

#include <iostream>
#include <iomanip>
#include <sstream>

// ----------------------------------------------------------------------------

struct ConsolePrinter : public StreamPrinter
{
  virtual void open(std::chrono::system_clock::time_point) final
  {
  }

  virtual void print(const std::string& text) final
  {
    std::cout << text.c_str();
  }

  virtual void close() final
  {
    std::cout << std::endl;
  }
};

// ----------------------------------------------------------------------------
