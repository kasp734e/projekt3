/* This unit test tests the two UI leds. 

Following files are tested:
- UI_led.h
- UI_led.cpp
*/

#include "UI_led.h"
#include <chrono>
#include <thread>
#include <iostream>

int main(){
    UIled myLeds;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // test led 1 can turn on and off
    for(int i = 0; i < 3; i++){
        myLeds.led1_on();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        myLeds.led1_off();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // test led 1 can blink
    for(int i = 0; i < 5; i++){
        myLeds.led1_blink();
    }

    // test led 2 can toggle
    for(int i = 0; i < 3; i++){
        myLeds.toggle_LED2();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}