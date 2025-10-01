# ESP32 PWM Project

This project demonstrates using ESP32 PWM to control an ESC and a servo on a RC car.
## Prerequisites

- ESP-IDF and its dependencies installed (see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/))  
- ESP32 development board (everything is configured to the ESP32 DEVKITV1, but reconfiguring for other boards should be easy)

## Building the Project
Make sure to source the ESP-IDF environment before idf commands are available:

    . $HOME/esp/esp-idf/export.sh

Then build

    idf.py build

And flash to the device. Write permissions are need and can be added if need with `sudo chmod 666 /dev/<ttyUSB>`.


    idf.py -p /dev/<ttyUSB> flash monitor 
