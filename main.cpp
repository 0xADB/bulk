
#include "reader.h"
#include "accumulator.h"
#include "file_printer.h"
#include "console_printer.h"

#include <chrono>
#include <iostream>
#include <cstring>
#include <memory>

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
  reader.attach(
      std::make_shared<Accumulator>(
	N
	, std::vector<std::shared_ptr<IProcessor>>{
	    std::make_shared<FilePrinter>()
	    , std::make_shared<ConsolePrinter>()
	  }
	)
      );
  reader.read(std::cin);
  return 0;
}

// ----------------------------------------------------------------------------

