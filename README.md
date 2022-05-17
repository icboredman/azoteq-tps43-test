# touch-test
Test application to demonstrate interfacing with Azoteq TPS43 touch pad I2C sensor in polling mode (without using RDY pin).
Polling of touch pad sensor is offloaded to a helper thread.
Main application receives events and dumps info to console.
### Compiling
* `cd build`
* `cmake ..`
* `make`
