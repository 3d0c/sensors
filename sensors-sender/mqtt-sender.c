#include "common.h"
#include "mqtt.h"

PROCESS(mqtt_sensors_process, "Mqtt Sensors Sender");
AUTOSTART_PROCESSES(&mqtt_sensors_process);

static int state = 0;

static void mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data) {
    switch(event) {
        case MQTT_EVENT_CONNECTED: {
            printf("MQTT Connected\n");
            state = 2;
            break;
        }

        case MQTT_EVENT_PUBLISH: {
            printf("Publishing\n");
            break;
        }
        
        // dumb handlers to prevent -Wswitch, msp-gcc ignores -Wno-switch
        case MQTT_EVENT_DISCONNECTED:
            state = -1;
            break;

        case MQTT_EVENT_SUBACK:
            state = -1;
            break;

        case MQTT_EVENT_UNSUBACK:
            state = -1;
            break;

        case MQTT_EVENT_PUBACK:
            state = -1;
            break;

        case MQTT_EVENT_ERROR:
            state = -1;
            break;

        case MQTT_EVENT_PROTOCOL_ERROR:
            state = -1;
            break;

        case MQTT_EVENT_CONNECTION_REFUSED_ERROR:
            state = -1;
            break;
 
        case MQTT_EVENT_NOT_IMPLEMENTED_ERROR:
            state = -1;
            break;

        case MQTT_EVENT_DNS_ERROR:
            state = -1;
            break;
    }
}

PROCESS_THREAD(mqtt_sensors_process, ev, data){
    static struct etimer et;
    static struct etimer ct;
    static struct mqtt_connection conn;
    static char local_addr[MAX_SIZE];
    clock_time_t send_interval = SEND_INTERVAL;

    PROCESS_BEGIN();
    
    ipaddr_sprintf(set_global_address(), local_addr);

    //no need to allocate memory
    
    state = 0;

    //connect func
    printf("connecting to MQTT broker\n");
    state = 1;
    mqtt_register(&conn, &mqtt_sensors_process, local_addr, mqtt_event, MAX_TCP_SEGMENT_SIZE);
    mqtt_connect(&conn, DST_GLOBAL, 1883, 60);

    
    SENSORS_ACTIVATE(battery_sensor);
    SENSORS_ACTIVATE(sht11_sensor);
    SENSORS_ACTIVATE(potentiometer_sensor);

    etimer_set(&et, send_interval);
    etimer_set(&ct, CLOCK_SECOND);
    
    static unsigned rh;
    static float mv;
    static uint16_t battery;
    static uint16_t potent;
    char cmd[128];

    while(1) {
        PROCESS_YIELD();
        printf("MQTT STATE = %d \n", state);

        if(etimer_expired(&ct) && state == 1) {
            printf("entered condition 1 - NOTconnected \n");
            etimer_set(&ct, CLOCK_SECOND);
        }

        if(etimer_expired(&ct) && state == 2) {
            //connected, send header
            state = 3;
        }

        if (etimer_expired(&et) && state == 3) {
            printf("entered condition 3 - connected \n");
            battery = battery_sensor.value(0);
            mv = (battery * 2.500 * 2) / 4096;
            rh = sht11_humidity();
            potent = potentiometer_sensor.value(0);
            
            snprintf(cmd, 128, "%s, Battery: %d.%03ld mV, Temp: %u, Hum: %u%%, Potent: %i",
                local_addr,
                battery, 
                (long) mv, 
                // this math is given from dev/sht/sht11.h
                (unsigned) (-39.60 + 0.01 * sht11_temp()), 
                (unsigned) (-4 + 0.0405*rh - 2.8e-6*(rh*rh)),
                potent
            );

            printf("MQTT VALUES NEED TO BE SENT NOW \n");

            mqtt_publish(&conn, NULL, "sensors", (uint8_t *)cmd, strlen(cmd), MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);

            etimer_set(&et, send_interval);
        }

        if (etimer_expired(&ct) && state == -1) {
            printf("ALERT !!! \n");
            state = 0;
            etimer_set(&ct, CLOCK_SECOND);
            state = 1;
            mqtt_register(&conn, &mqtt_sensors_process, local_addr, mqtt_event, MAX_TCP_SEGMENT_SIZE);
            mqtt_connect(&conn, DST_GLOBAL, 1883, 60);
        }
    }

    printf("The end\n");

    SENSORS_DEACTIVATE(battery_sensor);
    SENSORS_DEACTIVATE(sht11_sensor);
    SENSORS_DEACTIVATE(potentiometer_sensor);

    PROCESS_END();
}
