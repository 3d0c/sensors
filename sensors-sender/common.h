#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "dev/battery-sensor.h"
#include "dev/sht11/sht11-sensor.h"
#include "dev/sht11/sht11.h"
#include "lib/sensors.h"
#include "net/ip/uip-debug.h"
#include "dev/potentiometer-sensor.h"

#define SEND_INTERVAL 10 * CLOCK_SECOND;
#define MAX_TCP_SEGMENT_SIZE    32

#define MAX_SIZE 45

#define DST_GLOBAL "aaaa::0000:0000:0000:0000:0000:00001"

extern void get_local_addresses(char *local_addr);
extern uip_ipaddr_t *set_global_address(void);
extern void ipaddr_sprintf(const uip_ipaddr_t *addr, char *local_addr);
