#pragma once

#include "processor.h"

#include <sstream>

struct IPrinter
{
  virtual void open(std::chrono::system_clock::time_point) = 0;
  virtual void print(const std::string&) = 0;
  virtual void close() = 0;
  virtual ~IPrinter(){};
};

struct StreamPrinter : public IProcessor, public IPrinter
{
  virtual void push(std::chrono::system_clock::time_point cmdtime, const std::vector<Command>& cmds) final
  {
    if (!cmds.empty())
    {
      commit();

      std::ostringstream oss;
      auto cmdIt = std::begin(cmds);

      oss << "bulk: " << cmdIt->value.c_str();
      while (++cmdIt != std::end(cmds))
	oss << ", " << cmdIt->value.c_str();
      open(cmdtime);
      print(oss.str());
      close();
    }
  }

  void push_one(std::chrono::system_clock::time_point cmdtime, const Command& cmd)
  {
    if (!_oss.tellp())
    {
      open(cmdtime);
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
      print(_oss.str());
      close();
      _oss.str("");
      _oss.clear();
    }
  }

  virtual ~StreamPrinter()
  {
    StreamPrinter::commit();
  }

  std::ostringstream _oss;
};
