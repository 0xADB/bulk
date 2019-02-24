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
    while(std::getline(stream, cmd))
    {
      auto cmdtime = std::chrono::system_clock::now();
      for (auto p : _processors)
	p->push(cmdtime, Command{cmd});
    }
    for (auto p : _processors)
      p->commit();
  }

  std::vector<std::shared_ptr<IProcessor>> _processors{};
};

// ----------------------------------------------------------------------------

