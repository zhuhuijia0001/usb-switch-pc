#pragma once

namespace Packet
{
	//cmd type
	const unsigned char CMD_SWITCH_MAINBOARD = 0x01;
	const unsigned char CMD_SWITCH_SLAVEBOARD = 0x02;
	const unsigned char CMD_DISABLE_MAINBOARD = 0x03;

	//result
	const unsigned char RESULT_OK = 0;
	const unsigned char RESULT_FAILED = 1;

	static const int MAINBOARD_PORT_NUM = 16;

	static const int SLAVEBOARD_PORT_NUM = 8;

	int build_switch_mainboard_packet(unsigned char *buf, int max_size, int port);

	int build_disable_mainboard_packet(unsigned char *buf, int max_size);

	int build_switch_slaveboard_packet(unsigned char *buf, int max_size, int main_port, int slave_port);

	bool parse_response_packet(unsigned char *buf, int size, unsigned char &cmd, unsigned char &result);
}
