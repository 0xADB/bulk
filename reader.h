#pragma once

#include "processor.h"
#include <memory>
#include <iostream>
#include <chrono>

// ----------------------------------------------------------------------------

struct Reader
{
  void attach(std::shared_ptr<IProcessor> processor)
  {
    _processors.push_back(processor);
  }

  void read(std::istream& stream)
  {
    std::string cmd;
    int level = 0;
    std::vector<Command> cmds;
    auto cmdtime = std::chrono::system_clock::time_point();
    while(std::getline(stream, cmd))
    {
      if (cmd == "{")
      {
	level++;
      }
      else if (cmd == "}")
      {
	level--;
	if (level == 0 && !cmds.empty())
	{
	  for (auto p : _processors)
	    p->push(cmdtime, cmds);
	  cmds.clear();
	  cmdtime = std::chrono::system_clock::time_point();
	}
      }
      else if (level)
      {
	if (cmdtime == std::chrono::system_clock::time_point())
	  cmdtime = std::chrono::system_clock::now();
	cmds.push_back(Command{cmd});
      }
      else
      {
	for (auto p : _processors)
	  p->push_one(std::chrono::system_clock::now(), Command{cmd});
      }
    }
    for (auto p : _processors)
      p->commit();
  }

  std::vector<std::shared_ptr<IProcessor>> _processors{};
};

// ----------------------------------------------------------------------------

