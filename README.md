My experiments with ch32v208 ethernet.

It is **crucial** to set the correct values if `WCHNET_NUM_UDP`, `WCHNET_NUM_TCP`, and `WCHNET_NUM_TCP_LISTEN` in `include/net_config.h`!

* Set `WCHNET_NUM_UDP` to 1 if you use DHCP
* Set `WCHNET_NUM_TCP` to 1 for each server or client (for example, 2 clients mean `WCHNET_NUM_TCP = 2`)
* Set `WCHNET_NUM_TCP_LISTEN` to 1 for each server