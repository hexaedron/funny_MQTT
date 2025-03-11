# Ethernet and MQTT Library for ch32v208

This library provides support for Ethernet and MQTT on the **ch32v208** microcontroller. It is based on MQTT examples from WCH and depends on the following libraries:

- [ch32fun](https://github.com/cnlohr/ch32fun)
- [funny_libs](https://github.com/hexaedron/funny_libs)

Currently, this library is in **beta version**.

## Usage Example

A usage example can be found in the file `src/example.cpp`. Please refer to the comments in this file for more detailed information.

## Network Configuration

It is **crucial** to set the correct values in the `include/net_config.h` file for the library to work properly. Pay special attention to the following parameters:

- **`WCHNET_NUM_UDP`**: Set this to 1 if you are using DHCP.
- **`WCHNET_NUM_TCP`**: Set this to the number of TCP connections (for example, for two clients, set `WCHNET_NUM_TCP = 2`).
- **`WCHNET_NUM_TCP_LISTEN`**: Set this to the number of server listeners (for example, for one server, set `WCHNET_NUM_TCP_LISTEN = 1`).