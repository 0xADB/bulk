#pragma once

#include <time.h>

struct IResultReceiver
{
  virtual void receive(time_t, const char *) = 0;
  virtual void flush() = 0;
  virtual ~IResultReceiver(){}
};
