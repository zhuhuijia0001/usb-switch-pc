
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#include <string>

#include "Packet.h"
#include "UsbSwitch.h"

//helper
class CountDownLatch
{
public:
	CountDownLatch(int count)
	:_count(count)
	{
	}

	void count_down()
	{
		{
			std::lock_guard<std::mutex> locker(_mutex);
			_count--;
		}

		if (_count == 0)
		{
			_cond.notify_all();
		}
	}

	void wait()
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_cond.wait(locker, [this] { return _count == 0; });
	}

private:
	int _count;

	std::mutex _mutex;

	std::condition_variable _cond;
};

class Test
{
public:
	Test()
	:_latch(1)
	{
	}

	int test_switch_mainboard(int port)
	{
		if (port >= Packet::MAINBOARD_PORT_NUM)
		{
			std::cout << "invalid port\n";

			return -1;
		}

		if (_switch.open() < 0)
		{
			std::cout << "open switch failed\n";

			return -1;
		}

		std::cout << "start switching main port " << port << "\n";

		_thread = std::make_shared<std::thread>(&Test::run_in_thread, this);

		_switch.switch_mainboard(port);
		_latch.wait();

		_thread->join();

		_switch.close();

		return 0;
	}

	int test_switch_slaveboard(int main_port, int slave_port)
	{
		if (main_port >= Packet::MAINBOARD_PORT_NUM
			|| slave_port >= Packet::SLAVEBOARD_PORT_NUM)
		{
			std::cout << "invalid port\n";

			return -1;
		}

		if (_switch.open() < 0)
		{
			std::cout << "open switch failed\n";

			return -1;
		}

		std::cout << "start switching slave port " << slave_port << " on main port " << main_port << "\n";

		_thread = std::make_shared<std::thread>(&Test::run_in_thread, this);

		_switch.switch_slaveboard(main_port, slave_port);
		_latch.wait();

		_thread->join();

		_switch.close();

		return 0;
	}

	int test_disable_mainboard()
	{
		if (_switch.open() < 0)
		{
			std::cout << "open switch failed\n";

			return -1;
		}

		std::cout << "start disabling mainboard\n";

		_thread = std::make_shared<std::thread>(&Test::run_in_thread, this);

		_switch.disable_mainboard();
		_latch.wait();

		_thread->join();

		_switch.close();

		return 0;
	}

private:
	Test(const Test &) = delete;  //disable assign constructor

	Test & operator = (const Test &) = delete;  //disable copy constructor

	void run_in_thread()
	{
		unsigned char buf[32];
		int len = _switch.get_switch_response(buf, sizeof(buf));
		if (len > 0)
		{
			unsigned char cmd, result;
			if (Packet::parse_response_packet(buf, len, cmd, result))
			{
				if (result == Packet::RESULT_OK)
				{
					std::cout << "Operation OK\n";
				}
				else
				{
					std::cout << "Operation failed\n";
				}
			}
			else
			{
				std::cout << "Response error\n";
			}
		}

		_latch.count_down();
	}

private:
	UsbSwitch _switch;

	std::shared_ptr<std::thread> _thread;

	CountDownLatch _latch;
};

//usage: 
//1: switch main board
//   usb_switch -m port

//2: switch slave board
//   usb_switch -s main_port slave_port

//3: disable main board
//   usb_switch -d

//invalid cmd message
static const std::string invalid_cmd_msg("Invalid command. Must be like this: usb_switch -m|-s|-d ...\n");

static const std::string invalid_switch_mainboard_cmd_msg("Invalid command. Must be like this: usb_switch -m port\n");
static const std::string invalid_switch_slaveboard_cmd_msg("Invalid command. Must be like this: usb_switch -s main_port slave_port\n");
static const std::string invalid_disable_mainboard_cmd_msg("Invalid command. Must be like this: usb_switch -d\n");

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << invalid_cmd_msg;

		return -1;
	}

	std::string cmd(argv[1]);
	if (cmd.compare("-m") == 0)
	{
		if (argc != 3)
		{
			std::cout << invalid_switch_mainboard_cmd_msg;

			return -1;
		}

		int port = std::atoi(argv[2]);
		if (port < 0)
		{
			std::cout << invalid_switch_mainboard_cmd_msg;

			return -1;
		}

		Test test;
		test.test_switch_mainboard(port);
	}
	else if (cmd.compare("-s") == 0)
	{
		if (argc != 4)
		{
			std::cout << invalid_switch_slaveboard_cmd_msg;

			return -1;
		}

		int main_port = std::atoi(argv[2]);
		int slave_port = std::atoi(argv[3]);
		if (main_port < 0 || slave_port < 0)
		{
			std::cout << invalid_switch_slaveboard_cmd_msg;

			return -1;
		}

		Test test;
		test.test_switch_slaveboard(main_port, slave_port);
	}
	else if (cmd.compare("-d") == 0)
	{
		if (argc != 2)
		{
			std::cout << invalid_disable_mainboard_cmd_msg;

			return -1;
		}

		Test test;
		test.test_disable_mainboard();
	}

    return 0;
}
