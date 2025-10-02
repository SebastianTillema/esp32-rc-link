# ESP32 PWM Project

This project demonstrates using ESP32 PWM to control an ESC and a servo on a RC car.
## Prerequisites

- ESP-IDF and its dependencies installed (see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/))  
- ESP32 development board (everything is configured to the ESP32 DEVKITV1, but reconfiguring for other boards should be easy)

## Building, Flashing, and Monitoring
Before building, ensure the ESP-IDF environment is sourced:
```bash
. $HOME/esp/esp-idf/export.sh
```
Also verify that your user has write access to the serial device. For example:
```bash
sudo chmod 666 /dev/<ttyUSB>
```

### Selecting the Application Variant
This project supports multiple build targets:

receiver — runs PWM control logic for servos/ESCs.

transmitter — sends control data via ESPNOW.

You can choose which application to build by defining the DEFAULT_APP CMake variable.

### Example:
The following command will build the selected application, flash it to the ESP32, and open a serial monitor:
```bash
idf.py -p /dev/<ttyUSB> -DDEFAULT_APP=receiver build flash monitor
```

To build and flash the transmitter instead:
```bash
idf.py -p /dev/<ttyUSB> -DDEFAULT_APP=transmitter build flash monitor
```

Replace `/dev/<ttyUSB>` with the actual device path on your system (e.g., /dev/ttyUSB0).