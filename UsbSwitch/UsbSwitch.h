#pragma once

struct hid_device_;

class UsbSwitch
{
public:
	UsbSwitch();

	~UsbSwitch();

	int open();

	int close();

	int switch_mainboard(int port);

	int disable_mainboard();

	int switch_slaveboard(int main_port, int slave_port);

	int get_switch_response(unsigned char *buf, int size);

private:
	static const unsigned short sVID = 0x0483; //vendor id
	static const unsigned short sPID = 0x5710; //product id

	bool _hid_init_ok;

	hid_device_ *_handle;
};

