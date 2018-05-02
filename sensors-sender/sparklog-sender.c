#include "common.h"
#include "tcp-socket.h"

#define SOCKET_INPUTBUFSIZE  UIP_TCP_MSS
#define SOCKET_OUTPUTBUFSIZE MAX(UIP_TCP_MSS, 256)

PROCESS(bulkglog_process, "bulkglog Sensors Sender");
AUTOSTART_PROCESSES(&bulkglog_process);

struct spaarklog_conn {
    struct spaarklog_conn *next;
    struct tcp_socket socket;
    int state;
    
    int64_t pos;
    uint64_t length;
    
    void *callbackptr;
    int did_tcp_connect;
    
    uip_ipaddr_t server_ip;
    uint16_t server_port;

    uint8_t in_buffer[SOCKET_INPUTBUFSIZE];
    uint8_t out_buffer[SOCKET_OUTPUTBUFSIZE];

    struct etimer timeout_timer;
    uint8_t timeout_timer_started;
};

static int tcp_input(struct tcp_socket *s, void *ptr, const uint8_t *input_data_ptr, int input_data_len) {
    // nothing to process here, just a mock
    return 0;
}

static void tcp_event(struct tcp_socket *s, void *ptr, tcp_socket_event_t event) {
    struct spaarklog_conn *conn = ptr;
    
    switch(event) {
        case TCP_SOCKET_CONNECTED: {
            printf("Connected to bulkglog\n");
            conn->state = 2;
            break;

        case TCP_SOCKET_CLOSED: {
            printf("Socket closed\n");
            conn->state = -1;
            break;
        }

        case TCP_SOCKET_TIMEDOUT: {
            printf("Connection timeout\n");
            conn->state = -1;
            break;
        }

        case TCP_SOCKET_ABORTED: {
            printf("Connection aborted\n");
            conn->state = -1;
            break;
        }

        case TCP_SOCKET_DATA_SENT:
            break;
        }
    }
}

static void connect_tcp(struct spaarklog_conn *conn) {
    printf("Connecting to bulkglog\n");
    conn->state = 1;

    tcp_socket_register(&(conn->socket), conn, conn->in_buffer, SOCKET_INPUTBUFSIZE, conn->out_buffer, SOCKET_OUTPUTBUFSIZE, tcp_input, tcp_event);

    tcp_socket_connect(&(conn->socket), &(conn->server_ip), conn->server_port);
}

PROCESS_THREAD(bulkglog_process, ev, data){
    static struct etimer et;
    static struct etimer ct;
    static char local_addr[MAX_SIZE];
    static struct spaarklog_conn conn;
    clock_time_t send_interval = SEND_INTERVAL;

    PROCESS_BEGIN();

    ipaddr_sprintf(set_global_address(), local_addr);

    memset(&conn.server_ip, 0, sizeof(uip_ipaddr_t));
    if (uiplib_ipaddrconv(DST_GLOBAL, &conn.server_ip) == 0) {
        printf("uiplib_ipaddrconv error\n");
        PROCESS_EXIT();
    }

    conn.server_port = 2667;
    conn.state = 0;

    connect_tcp(&conn);

    SENSORS_ACTIVATE(battery_sensor);
    SENSORS_ACTIVATE(potentiometer_sensor);
    sht11_init();

    etimer_set(&et, send_interval);
    etimer_set(&ct, CLOCK_SECOND);

    static unsigned rh;
    static float mv;
    static uint16_t battery;
    static uint16_t potent;
    char cmd[128];

    while(1) {
        PROCESS_YIELD();

        if (etimer_expired(&ct) && conn.state == 1) {
            // connecting
            etimer_set(&ct, CLOCK_SECOND);            
        }

        if (etimer_expired(&ct) && conn.state == 2) {
            // connected, send header
            printf("Sending header\n");
            snprintf(cmd, 256, ":=:header sensors %s 100 #timestamp Battery Temp Hum Potent\n", local_addr);
            tcp_socket_send(&(conn.socket), (const uint8_t *)cmd, strlen(cmd));
            conn.state = 3;
        }

        if (etimer_expired(&et) && conn.state == 3) {
            // connected and header sent, sending data
            battery = battery_sensor.value(0);           
            mv = (battery * 2.500 * 2) / 4096;
            rh = sht11_humidity();
            potent = potentiometer_sensor.value(0);

            snprintf(cmd, 256, ":=:sensors %s 100 %u %d.%03ld %u %u %i\n",
                local_addr,
                (unsigned) clock_seconds(),
                battery, 
                (long) mv, 
                // this math is given from dev/sht/sht11.h
                (unsigned) (-39.60 + 0.01 * sht11_temp()), 
                (unsigned) (-4 + 0.0405*rh - 2.8e-6*(rh*rh)),
                potent
            );
            
            tcp_socket_send(&(conn.socket), (const uint8_t *)cmd, strlen(cmd));
            
            etimer_set(&et, send_interval);
        }

        if(etimer_expired(&ct) && conn.state == -1) {
            printf("time expired and state == -1\n");
            etimer_set(&ct, CLOCK_SECOND);
            conn.state = 0;
            connect_tcp(&conn);
        }
    }

    printf("The end\n");

    SENSORS_DEACTIVATE(battery_sensor);
    SENSORS_DEACTIVATE(potentiometer_sensor);

    PROCESS_END();
}
