#include <string>
#include <sstream>

#include "processor.h"
#include "reader.h"

using namespace std::string_literals;

struct Resulter : public IResultReceiver
{
  virtual void receive(const char * result) final
  {
    _result << result;
  }
  virtual void flush() final
  {
    _result << std::endl;
  }
  virtual ~Resulter(){}

  std::ostringstream _result;
};

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_main)

  BOOST_AUTO_TEST_CASE(test_processor_with_plain_commands)
  {
    std::vector<Command> cmds
    {
        Command{"1"}
      , Command{"2"}
      , Command{"3"}
      , Command{"4"}
    };

    auto resulter = std::make_shared<Resulter>();
    Processor p(2, resulter);

    p.push(cmds[0]); // 1
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push(cmds[1]); // 2
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[2]); // 3
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[3]); // 4
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_plain_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("1\n2\n3\n4");
    r.attach(std::make_shared<Processor>(2, resulter));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_nested_commands)
  {
    std::vector<Command> cmds
    {
        Command{"1"}
      , Command{"2"}
      , Command{"{"}
      , Command{"3"}
      , Command{"4"}
      , Command{"}"}
      , Command{"5"}
    };

    auto resulter = std::make_shared<Resulter>();
    Processor p(2, resulter);

    p.push(cmds[0]); // 1
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push(cmds[1]); // 2
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[2]); // {
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[3]); // 3
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[4]); // 4
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[5]); // }
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(cmds[6]); // 5
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4, 5\n"s);
  }

BOOST_AUTO_TEST_SUITE_END()
