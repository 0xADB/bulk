#pragma once

#include "processor.h"
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>

struct Accumulator : public IProcessor
{
  Accumulator() = default;
  virtual ~Accumulator() final = default;

  explicit Accumulator (size_t N, const std::vector<std::shared_ptr<IProcessor>>& processors)
    : _N(N), _processors(processors)
  {
    _commands.reserve(_N);
  }

  void registerProcessor(std::shared_ptr<IProcessor> processor)
  {
    _processors.push_back(processor);
  }

  virtual void push(std::chrono::system_clock::time_point cmdtime, const Command& cmd) final
  {
    if (cmd.value == "{")
    {
      if (_level == 0)
      {
	if (!_commands.empty())
	  commit();
	_commands.emplace_back(std::vector<Command>());
      }
      ++_level;
    }
    else if (cmd.value == "}")
    {
      if (_level != 0)
      {
	if (--_level == 0)
	  commit();
      }
      else
	throw std::runtime_error("syntax error: unexpected '}' at you know where... you gotta be... you'd better be.");
    }
    else if (_level != 0)
    {
      if (_commands.back().empty() && _blocktime == std::chrono::system_clock::time_point())
	_blocktime = cmdtime;
      _commands.back().push_back(cmd);
    }
    else
    {
      _commands.emplace_back(std::vector<Command>{cmd});
      if (_blocktime == std::chrono::system_clock::time_point())
	_blocktime = cmdtime;
      if (_commands.size() == _N)
	commit();
    }
  }

  virtual void commit() final
  {
    if (_processors.empty())
    {
#ifndef NDEBUG
      throw std::logic_error("no processors");
#endif
      _commands.clear();
      return;
    }

    if (_level != 0 && !_commands.empty())
      _commands.pop_back();

    if (_commands.empty())
      return;

    bool sent_something = false; // to avoid commiting empty packets
    for (const auto& v : _commands)
    {
      if (!v.empty())
      {
	for (const auto& c : v)
	{
	  for (auto& p : _processors)
	    p->push(_blocktime, c);
	}
	sent_something = true;
      }
    }

    if (sent_something)
    {
      for (auto& p : _processors)
	p->commit();
    }
    _commands.clear();
    _blocktime = std::chrono::system_clock::time_point();
  }

  std::vector<std::vector<Command>> _commands{};
  size_t _level = 0;
  size_t _N = 1;
  std::chrono::system_clock::time_point _blocktime{};
  std::vector<std::shared_ptr<IProcessor>> _processors;
};

// ----------------------------------------------------------------------------

