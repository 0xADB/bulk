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

  virtual void push(std::chrono::system_clock::time_point cmdtime, const std::vector<Command>& cmds) final
  {
    if (!cmds.empty())
    {
      commit();
      for (auto& p : _processors)
	p->push(cmdtime, cmds);
    }
  }

  virtual void push_one(std::chrono::system_clock::time_point cmdtime, const Command& cmd) final
  {
    _commands.emplace_back(Command{cmd});
    if (_blocktime == std::chrono::system_clock::time_point())
      _blocktime = cmdtime;
    if (_commands.size() == _N)
      commit();
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

    if (_commands.empty())
      return;

    for (auto& p : _processors)
    {
      p->push(_blocktime, _commands);
      p->commit();
    }
    _commands.clear();
    _blocktime = std::chrono::system_clock::time_point();
  }

  std::vector<Command> _commands{};
  size_t _N = 1;
  std::chrono::system_clock::time_point _blocktime{};
  std::vector<std::shared_ptr<IProcessor>> _processors;
};

// ----------------------------------------------------------------------------

