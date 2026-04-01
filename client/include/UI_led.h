#pragma once
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Class for a future feature
class UIled {
  public:
	UIled();
	~UIled();
	void toggle_LED2();

	void led1_on();
	void led1_off();
	void led1_blink();

	bool get_led1();
	bool get_led2();

  private:
	int chip_fd = -1;
	struct gpiohandle_request req_out;
	struct gpiohandle_data data_out;
	bool led1_status = false;
	bool led2_status = false;
};
