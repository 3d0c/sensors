#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "dev/battery-sensor.h"
#include "dev/potentiometer-sensor.h"
#include "dev/sht11/sht11-sensor.h"
#include "dev/sht11/sht11.h"
#include "lib/sensors.h"

#include "rest-engine.h"

extern resource_t res_battery, res_sht11, res_potent;

PROCESS(er_process, "Erbium Server");
AUTOSTART_PROCESSES(&er_process);

uip_ipaddr_t *set_global_address(void) {
    uip_ipaddr_t *ipaddr = malloc(sizeof(uip_ipaddr_t));

    uip_ip6addr(ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(ipaddr, &uip_lladdr);
    uip_ds6_addr_add(ipaddr, 0, ADDR_AUTOCONF);

    return ipaddr;
}

PROCESS_THREAD(er_process, ev, data) {
    PROCESS_BEGIN();
    PROCESS_PAUSE();

    set_global_address();
    
    rest_init_engine();
    
    rest_activate_resource(&res_sht11, "sensors/sht11");  
    SENSORS_ACTIVATE(sht11_sensor);  

    rest_activate_resource(&res_battery, "sensors/battery");  
    SENSORS_ACTIVATE(battery_sensor);  

    rest_activate_resource(&res_potent, "sensors/potent");  
    SENSORS_ACTIVATE(potentiometer_sensor);

    while(1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
