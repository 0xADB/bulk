
#include "reader.h"
#include "processor.h"

#include <iostream>
#include <cstdlib>
#include <cstring>

// ----------------------------------------------------------------------------

struct Printer : public IResultReceiver
{
  virtual void receive(const char * result) final
  {
    std::cout << result;
  }
  virtual void flush() final
  {
    std::cout << std::endl;
  }
  virtual ~Printer(){}
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

