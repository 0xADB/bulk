#pragma once

struct IResultReceiver
{
  virtual void receive(const char *) = 0;
  virtual void flush() = 0;
  virtual ~IResultReceiver(){}
};
