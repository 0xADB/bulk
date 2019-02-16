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
	if (--_level == 0 && _commands.size() == _N)
	  commit();
      }
      else
	throw std::runtime_error("syntax error: unexpected '}' at you know where... you gotta be... you'd better be.");
    }
    else if (_level != 0)
    {
      _commands.back().push_back(cmd);
    }
    else
    {
      _commands.emplace_back(std::vector<Command>{cmd});
      if (_commands.size() == _N)
	commit();
    }
  }

  virtual void commit() final
  {
    if (!_receiver)
    {
      _commands.clear();
      return;
      //throw std::runtime_error("no receiver provided"); // is that better than checking _receiver repeatedly?
    }

    if (_level != 0 && !_commands.empty())
      _commands.pop_back();

    if (_commands.empty())
      return;

    bool start = true;
    for (const auto& v : _commands)
    {
      for (const auto& c : v)
      {
	if (!start)
	  _receiver->receive(", ");
	else
	{
	  _receiver->receive("bulk: ");
	  start = false;
	}
	_receiver->receive(c.value.c_str());
      }
    }
    if (!start)
      _receiver->flush();
    _commands.clear();
  }

  std::vector<std::vector<Command>> _commands{};
  size_t _level = 0;
  size_t _N = 1;
  std::shared_ptr<IResultReceiver> _receiver;
};

// ----------------------------------------------------------------------------

