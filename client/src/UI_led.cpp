#include "UI_led.h"

#define GPIO_led1 11
#define GPIO_led2 9

UIled::UIled(){
    chip_fd = open("/dev/gpiochip0", O_WRONLY); // open the io chip in write mode
    if (chip_fd < 0) {                          // check for errors
        std::cerr << "Failed to open /dev/gpiochip0: " << strerror(errno)
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&req_out, 0, sizeof(req_out)); // zeros out the req struct

    req_out.lines = 2; // ask for 2 gpio lines
    req_out.lineoffsets[0] = GPIO_led1;
    req_out.lineoffsets[1] = GPIO_led2;

    // turn off initially
    req_out.default_values[0] = 0; 
    req_out.default_values[1] = 0; 

    memset(&data_out, 0, sizeof(data_out));

    // set flags
    req_out.flags = GPIOHANDLE_REQUEST_OUTPUT;

    if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req_out) < 0) {
        std::cerr << "GPIO_GET_LINEHANDLE_IOCTL (led) failed: "
                << strerror(errno) << std::endl;
        close(chip_fd);
        exit(EXIT_FAILURE);
    }
}

UIled::~UIled(){
    // turn off at destruction
    led1_off();
    if(led2_status == true){
        toggle_LED2();
    }
    led1_status = false;
    led2_status = false;
    close(req_out.fd);  
    close(chip_fd);
}

void UIled::toggle_LED2() {
    led2_status = !led2_status;
    data_out.values[1] = led2_status;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
}

void UIled::led1_on() {
    led1_status = true;
    data_out.values[0] = 1;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
}

void UIled::led1_off() {
    led1_status = false;
    data_out.values[0] = 0;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
}

void UIled::led1_blink() {
    data_out.values[0] = 1;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
    usleep(200000);
    data_out.values[0] = 0;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
    usleep(200000);
}

bool UIled::get_led1(){
    return led1_status;
}

bool UIled::get_led2(){
    return led2_status;
}