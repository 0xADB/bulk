
#include "reader.h"
#include "processor.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <chrono>
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>

// ----------------------------------------------------------------------------

struct Printer : public IResultReceiver
{
  void open(time_t time)
  {
    std::vector<char> name(4 + std::numeric_limits<uint64_t>::digits10 + 4 + 1); // "bulk1517223860.log"
    std::snprintf(
	name.data()
	, name.size()
	, "bulk%" PRIu64 ".log"
	, static_cast<uint64_t>(time)
	);
    name.back() = 0;
    out.open(name.data());
    if (!out.is_open())
      throw std::runtime_error(std::string("unable to open") + " " + name.data());
  }

  virtual void receive(time_t time, const char * result) final
  {
    if (!out.is_open())
      open(time);
    std::cout << result;
    out << result;
  }

  virtual void flush() final
  {
    std::cout << std::endl;
    if (out.is_open())
    {
      out << std::endl;
      out.close();
    }
  }

  virtual ~Printer()
  {
    if (out.is_open())
      out.close();
  }
  std::ofstream out;
};

// ----------------------------------------------------------------------------

void printUsage(const char * self)
{
  std::cerr << "\nUsage: " << self << " <block_size>"
	    << "\n    <block_size>\t\tA positive number of commands to be processed as a block"
	    << std::endl;
}

// ----------------------------------------------------------------------------

int main(int argc, char ** argv)
{
  if (argc != 2 || strncmp(argv[1], "--help", 6) == 0 || strncmp(argv[1], "-h", 2) == 0)
  {
    printUsage(argv[0]);
    return 0;
  }

  char * nEnd = nullptr;
  unsigned long N = std::strtoul(argv[1], &nEnd, 10);
  if (errno == ERANGE)
  {
    std::cerr << "Error: the <block_size> argument overflow. Please, be reasonable." << std::endl;
    return -1;
  }
  else if (nEnd && *nEnd != '\0')
  {
    std::cerr << "Error: couldn't understand the <block_size> argument at: " << nEnd << std::endl;
    return -1;
  }

  Reader reader;
  reader.attach(std::make_shared<Processor>(N, std::make_shared<Printer>()));
  reader.read(std::cin);
  return 0;
}

// ----------------------------------------------------------------------------

