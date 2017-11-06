
#include "hidapi.h"

#include "Packet.h"
#include "UsbSwitch.h"

UsbSwitch::UsbSwitch()
:_handle(nullptr),
_hid_init_ok(false)
{
	if (hid_init() == 0)
	{
		_hid_init_ok = true;
	}
}

UsbSwitch::~UsbSwitch()
{
	close();

	if (_hid_init_ok)
	{
		hid_exit();
	}
}

int UsbSwitch::open()
{
	if (!_hid_init_ok)
	{
		return -1;
	}

	if (_handle != nullptr)
	{
		return 0;
	}

	_handle = hid_open(sVID, sPID, NULL);
	if (_handle == nullptr)
	{
		return -1;
	}

	return 0;
}

int UsbSwitch::close()
{
	if (_handle != nullptr)
	{
		hid_close(_handle);
		_handle = nullptr;
	}

	return 0;
}

int UsbSwitch::switch_mainboard(int port)
{
	if (_handle == nullptr)
	{
		return -1;
	}

	unsigned char buf[32];
	int size = Packet::build_switch_mainboard_packet(buf, sizeof(buf), port);
	if (size <= 0)
	{
		return -1;
	}

	return hid_write(_handle, buf, size);
}

int UsbSwitch::disable_mainboard()
{
	if (_handle == nullptr)
	{
		return -1;
	}

	unsigned char buf[32];
	int size = Packet::build_disable_mainboard_packet(buf, sizeof(buf));
	if (size <= 0)
	{
		return -1;
	}

	return hid_write(_handle, buf, size);
}

int UsbSwitch::switch_slaveboard(int main_port, int slave_port)
{
	if (_handle == nullptr)
	{
		return -1;
	}

	unsigned char buf[32];
	int size = Packet::build_switch_slaveboard_packet(buf, sizeof(buf), main_port, slave_port);
	if (size <= 0)
	{
		return -1;
	}

	return hid_write(_handle, buf, size);
}

int UsbSwitch::get_switch_response(unsigned char *buf, int size)
{
	hid_set_nonblocking(_handle, 0);

	return hid_read(_handle, buf, size);
}