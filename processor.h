#pragma once

#include "command.h"
#include "result_recevier.h"
#include <iostream>
#include <vector>
#include <memory>

// ----------------------------------------------------------------------------

struct IProcessor
{
  virtual void push(const Command&) = 0;
  virtual void commit() = 0;
  virtual ~IProcessor(){}
};

// ----------------------------------------------------------------------------

struct Processor : public IProcessor
{
  Processor() = default;

  explicit Processor (size_t N, std::shared_ptr<IResultReceiver> receiver)
    : _N(N), _receiver(receiver)
  {
    if (!_receiver)
      throw std::runtime_error("no receiver provided");
    _commands.reserve(_N);
  }

  virtual ~Processor() final {}

  virtual void push(const Command& cmd) final
  {
    _commands.push_back(cmd);
    if (_commands.size() >= _N)
      commit();
  }

  virtual void commit() final
  {
    if (!_receiver)
      throw std::runtime_error("no receiver provided"); // is that better than checking _receiver repeatedly?

    if (_commands.empty()) return;
    _receiver->receive("bulk: ");
    for (auto cIt = std::begin(_commands)
	, cEndIt = std::end(_commands)
	; cIt != cEndIt
	; ++cIt
	)
    {
      if (cIt != std::begin(_commands))
	_receiver->receive(", ");
      _receiver->receive(cIt->_text.c_str());
    }
    _receiver->flush();
    _commands.clear();
  }

  std::vector<Command> _commands{};
  size_t _N = 1;
  std::shared_ptr<IResultReceiver> _receiver;
};

// ----------------------------------------------------------------------------

