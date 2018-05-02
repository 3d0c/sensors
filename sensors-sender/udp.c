#include "common.h"

PROCESS(udp_sensors_process, "Udp Sensors Sender");
AUTOSTART_PROCESSES(&udp_sensors_process);

static struct uip_udp_conn *client_conn = NULL;
static uip_ipaddr_t dest_addr;

static void udp_sender(char *data) {
    if (client_conn == NULL) {
        if ((client_conn = udp_new(&dest_addr, UIP_HTONS(4000), NULL)) == NULL) {
            printf("Unable to connect\n");
            return;
        }
    }

    uip_udp_packet_send(client_conn, data, strlen(data));
}

PROCESS_THREAD(udp_sensors_process, ev, data){
    static struct etimer et;
    static char local_addr[MAX_SIZE];
    clock_time_t send_interval = SEND_INTERVAL;

    PROCESS_BEGIN();

    ipaddr_sprintf(set_global_address(), local_addr);

    memset(&dest_addr, 0, sizeof(uip_ipaddr_t));
    if (uiplib_ipaddrconv(DST_GLOBAL, &dest_addr) == 0) {
        printf("uiplib_ipaddrconv error\n");        
    }

    SENSORS_ACTIVATE(battery_sensor);
    SENSORS_ACTIVATE(potentiometer_sensor);
    sht11_init();

    etimer_set(&et, send_interval);
    
    static unsigned rh;
    static float mv;
    static uint16_t battery;
    static uint16_t potent;
    char cmd[128];
    
    while(1) {
        PROCESS_YIELD();
        if(etimer_expired(&et)) {
            battery = battery_sensor.value(0);           
            mv = (battery * 2.500 * 2) / 4096;
            rh = sht11_humidity();
            potent = potentiometer_sensor.value(0);

            snprintf(cmd, 128, "%s, Battery: %d.%03ld mV, Temp: %u, Hum: %u%%, Potent: %i\n",
                local_addr,
                battery, 
                (long) mv, 
                // this math is given from dev/sht/sht11.h
                (unsigned) (-39.60 + 0.01 * sht11_temp()), 
                (unsigned) (-4 + 0.0405*rh - 2.8e-6*(rh*rh)),
                potent
            );
            
            udp_sender(cmd);
            
            etimer_set(&et, send_interval);
        }
    }

    printf("The end\n");

    SENSORS_DEACTIVATE(battery_sensor);
    SENSORS_DEACTIVATE(potentiometer_sensor);

    PROCESS_END();
}
