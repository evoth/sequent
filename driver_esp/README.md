# ESP32 Driver
This will be the driver program for ESP32 (and maybe ESP8266) microcontrollers for use with two Canon cameras (one via CCAPI and one via [reverse-engineered PTP/IP](https://julianschroden.com/post/2023-05-10-pairing-and-initializing-a-ptp-ip-connection-with-a-canon-eos-camera)), multiple sensors, servo motors, and perhaps some GoPros as well.

Right now it's just some code to experiment with controlling my Canon EOS Rebel T6 (which doesn't have an officially documented API) using the [PTP/IP protocol as reverse-engineered for Canon cameras by Julian Schroden](https://julianschroden.com/post/2023-05-10-pairing-and-initializing-a-ptp-ip-connection-with-a-canon-eos-camera).

## TODO:
- Organize it and isolate repeated logic (this is just some quick-and-dirty experimentation with PTP/IP camera communication at the moment)