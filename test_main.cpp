#include <string>
#include <sstream>

#include "processor.h"
#include "accumulator.h"
#include "reader.h"
#include "stream_printer.h"

using namespace std::string_literals;

struct Resulter : public StreamPrinter
{
  virtual ~Resulter() final = default;
  virtual void open(std::chrono::system_clock::time_point) final {}

  virtual void print(const std::string& text) final
  {
    _result << text.c_str();
  }

  virtual void close() final
  {
    _result << "\n";
  }

  std::ostringstream _result{};
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
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});

    p.push_one(std::chrono::system_clock::now(), cmds[0]); // 1
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push_one(std::chrono::system_clock::now(), cmds[1]); // 2
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[2]); // 3
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[3]); // 4
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);

    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_plain_commands_packed)
  {
    std::vector<Command> cmds
    {
        Command{"1"}
      , Command{"2"}
      , Command{"3"}
      , Command{"4"}
    };
    auto resulter = std::make_shared<Resulter>();
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});
    p.push(std::chrono::system_clock::now(), cmds);
    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2, 3, 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_plain_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("1\n2\n3\n4");
    r.attach(std::make_shared<Accumulator>(2, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_empty_commands)
  {
    std::vector<Command> cmds
    {
        Command{"1"}
      , Command{""}
      , Command{""}
      , Command{"4"}
    };

    auto resulter = std::make_shared<Resulter>();
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});

    p.push_one(std::chrono::system_clock::now(), cmds[0]); // 1
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push_one(std::chrono::system_clock::now(), cmds[1]); // 2
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, \n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[2]); // 3
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, \n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[3]); // 4
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, \nbulk: , 4\n"s);

    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, \nbulk: , 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_empty_commands_packed)
  {
    std::vector<Command> cmds
    {
        Command{"1"}
      , Command{""}
      , Command{""}
      , Command{"4"}
    };

    auto resulter = std::make_shared<Resulter>();
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});
    p.push(std::chrono::system_clock::now(), cmds);
    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, , , 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_empty_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("1\n\n\n4");
    r.attach(std::make_shared<Accumulator>(2, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, \nbulk: , 4\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_nested_commands)
  {
    std::vector<std::vector<Command>> cmds
    {
        {Command{"1"}}
      , {Command{"2"}}
      , {Command{"3"}, Command{"4"}}
      , {Command{"5"}}
    };

    auto resulter = std::make_shared<Resulter>();
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});

    p.push_one(std::chrono::system_clock::now(), cmds[0][0]); // 1
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push_one(std::chrono::system_clock::now(), cmds[1][0]); // 2
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\n"s);

    p.push(std::chrono::system_clock::now(), cmds[2]); // {3, 4}
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[3][0]);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\n"s);

    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2\nbulk: 3, 4\nbulk: 5\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_empty_nested_commands)
  {
    std::vector<std::vector<Command>> cmds
    {
	{}
      , {Command{"3"}, Command{"4"}}
      , {Command{"5"}}
    };

    auto resulter = std::make_shared<Resulter>();
    Accumulator p(2, std::vector<std::shared_ptr<IProcessor>>{resulter});

    p.push(std::chrono::system_clock::now(), cmds[0]); // {}
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push(std::chrono::system_clock::now(), cmds[1]); // {3,4}
    BOOST_CHECK(resulter->_result.str() == "bulk: 3, 4\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[2][0]); // 5
    BOOST_CHECK(resulter->_result.str() == "bulk: 3, 4\n"s);

    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 3, 4\nbulk: 5\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_empty_nested_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("{\n}\n{\n3\n4\n}\n5\n");
    r.attach(std::make_shared<Accumulator>(2, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 3, 4\nbulk: 5\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_open_nested_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("{\n1\n}\n{\n3\n4");
    r.attach(std::make_shared<Accumulator>(2, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_deeper_nested_commands)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("{\n{\n1\n2\n}\n3\n}\n4\n5");
    r.attach(std::make_shared<Accumulator>(2, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 1, 2, 3\nbulk: 4, 5\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_processor_with_nested_commands_in_the_middle)
  {
    //std::cout << "test_processor_with_nested_commands_in_the_middle" << std::endl;
    std::vector<std::vector<Command>> cmds
    {
	{Command{"00"}}
      , {Command{"01"}}
      , {Command{"02"}}
      , {Command{"03"}}
      , {Command{"04"}} // {}
      , {Command{"05"}}
    };

    auto resulter = std::make_shared<Resulter>();
    Accumulator p(3, std::vector<std::shared_ptr<IProcessor>>{resulter});

    p.push_one(std::chrono::system_clock::now(), cmds[0][0]); // 00
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push_one(std::chrono::system_clock::now(), cmds[1][0]); // 01
    BOOST_CHECK(resulter->_result.str() == ""s);

    p.push_one(std::chrono::system_clock::now(), cmds[2][0]); // 02
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[3][0]); // 03
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\n"s);

    p.push(std::chrono::system_clock::now(), cmds[4]); // 04
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\nbulk: 03\nbulk: 04\n"s);

    p.push_one(std::chrono::system_clock::now(), cmds[5][0]); // 05
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\nbulk: 03\nbulk: 04\n"s);

    p.commit();
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\nbulk: 03\nbulk: 04\nbulk: 05\n"s);
  }

  BOOST_AUTO_TEST_CASE(test_reader_and_processor_with_nested_commands_in_the_middle)
  {
    Reader r;
    auto resulter = std::make_shared<Resulter>();
    std::istringstream in("00\n01\n02\n03\n{\n04\n}\n05");
    r.attach(std::make_shared<Accumulator>(3, std::vector<std::shared_ptr<IProcessor>>{resulter}));
    r.read(in);
    BOOST_CHECK(resulter->_result.str() == "bulk: 00, 01, 02\nbulk: 03\nbulk: 04\nbulk: 05\n"s);
  }

BOOST_AUTO_TEST_SUITE_END()
