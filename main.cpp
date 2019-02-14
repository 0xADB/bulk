#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <memory>

// ----------------------------------------------------------------------------

using command_t = std::string;

// ----------------------------------------------------------------------------

void printUsage(const char * self)
{
  std::cerr << "\nUsage: " << self << " <block_size>"
	    << "\n    <block_size>\t\tA positive number of commands to be processed as a block"
	    << std::endl;
}

// ----------------------------------------------------------------------------

struct IProcessor
{
  virtual void process(const command_t&) = 0;
  virtual void flush() = 0;
  virtual ~IProcessor(){}
};

// ----------------------------------------------------------------------------

struct Processor : public IProcessor
{
  Processor() = default;
  explicit Processor (size_t N) : _N(N) {_commands.reserve(_N);}
  virtual ~Processor() final {}

  virtual void process(const command_t& cmd) final
  {
    _commands.push_back(cmd);
    if (_commands.size() >= _N)
      flush();
  }

  virtual void flush() final
  {
    if (_commands.empty()) return;
    std::cout << "bulk: ";
    for (auto cIt = std::begin(_commands)
	, cEndIt = std::end(_commands)
	; cIt != cEndIt
	; ++cIt
	)
    {
      if (cIt != std::begin(_commands))
	  std::cout << ", ";
      std::cout << cIt->c_str();
    }
    std::cout << std::endl;
    _commands.clear();
  }

  std::vector<command_t> _commands{};
  size_t _N = 1;
};

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
      for (auto p : _processors)
	p->process(cmd);
    }
    for (auto p : _processors)
      p->flush();
  }

  std::vector<std::shared_ptr<IProcessor>> _processors{};
};

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
  reader.attach(std::make_shared<Processor>(N));
  reader.read(std::cin);
  return 0;
}

// ----------------------------------------------------------------------------
