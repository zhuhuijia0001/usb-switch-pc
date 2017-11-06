
#include <string.h>

#include "Packet.h"

//packet size, 1 byte report id + 8 bytes report
#define PACKET_SIZE                9

//report id
#define REPORT_ID                  0x00

int Packet::build_switch_mainboard_packet(unsigned char *buf, int max_size, int port)
{
	if (max_size < PACKET_SIZE)
	{
		return -1;
	}

	memset(buf, 0x00, PACKET_SIZE);
	buf[0] = REPORT_ID;
	buf[1] = CMD_SWITCH_MAINBOARD;
	buf[2] = (unsigned char)(port & 0xff);

	return PACKET_SIZE;
}

int Packet::build_disable_mainboard_packet(unsigned char *buf, int max_size)
{
	if (max_size < PACKET_SIZE)
	{
		return -1;
	}

	memset(buf, 0x00, PACKET_SIZE);
	buf[0] = REPORT_ID;
	buf[1] = CMD_DISABLE_MAINBOARD;

	return PACKET_SIZE;
}

int Packet::build_switch_slaveboard_packet(unsigned char *buf, int max_size, int main_port, int slave_port)
{
	if (max_size < PACKET_SIZE)
	{
		return -1;
	}

	memset(buf, 0x00, PACKET_SIZE);
	buf[0] = REPORT_ID;
	buf[1] = CMD_SWITCH_SLAVEBOARD;
	buf[2] = (unsigned char)(main_port & 0xff);
	buf[3] = (unsigned char)(slave_port & 0xff);

	return PACKET_SIZE;
}

bool Packet::parse_response_packet(unsigned char *buf, int size, unsigned char &cmd, unsigned char &result)
{
	bool ret = true;

	switch (buf[0])
	{
	case CMD_SWITCH_MAINBOARD:
		if (buf[2] == RESULT_OK 
			|| buf[2] == RESULT_FAILED)
		{
			cmd = buf[0];
			result = buf[2];
		}
		else
		{
			ret = false;
		}

		break;

	case CMD_SWITCH_SLAVEBOARD:
		if (buf[3] == RESULT_OK
			|| buf[3] == RESULT_FAILED)
		{
			cmd = buf[0];
			result = buf[3];
		}
		else
		{
			ret = false;
		}

		break;

	case CMD_DISABLE_MAINBOARD:
		if (buf[1] == RESULT_OK
			|| buf[1] == RESULT_FAILED)
		{
			cmd = buf[0];
			result = buf[1];
		}
		else
		{
			ret = false;
		}

		break;

	default:
		ret = false;

		break;
	}

	return ret;
}