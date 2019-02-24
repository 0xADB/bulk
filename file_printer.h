#pragma once

#include "processor.h"

#include <fstream>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>
#include <stdint.h>

// ----------------------------------------------------------------------------

struct FilePrinter : public IProcessor
{
  void open(time_t opening_time)
  {
    std::vector<char> name(4 + std::numeric_limits<uint64_t>::digits10 + 4 + 1); // "bulk1517223860.log"
    std::snprintf(
	name.data()
	, name.size()
	, "bulk%" PRIu64 ".log"
	, static_cast<uint64_t>(opening_time)
	);
    name.back() = 0;
    out.open(name.data());
    if (!out.is_open())
      throw std::runtime_error(std::string("unable to open") + " " + name.data());
  }

  virtual void push(std::chrono::system_clock::time_point cmdtime, const Command& cmd) final
  {
    if (!out.is_open())
    {
      open(std::chrono::system_clock::to_time_t(cmdtime));
      out << "bulk: ";
    }
    else
      out << ", ";
    out << cmd.value.c_str();
  }

  virtual void commit() final
  {
    if (out.is_open())
      out.close();
  }

  virtual ~FilePrinter()
  {
    FilePrinter::commit();
  }

  std::ofstream out;
};

// ----------------------------------------------------------------------------
