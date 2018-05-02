#include "common.h"

void ipaddr_sprintf(const uip_ipaddr_t *addr, char *local_addr) {
    if(addr == NULL || addr->u8 == NULL) {
        snprintf(local_addr, MAX_SIZE, "no ip defined");
        return;
    }

    uint16_t a, pos=0;
    unsigned int i;
    int f;

    for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
        a = (addr->u8[i] << 8) + addr->u8[i + 1];
        if(a == 0 && f >= 0) {
            if(f++ == 0) {
                pos += snprintf(&local_addr[pos], MAX_SIZE - pos, "::");
            }
        } else {
            if(f > 0) {
                f = -1;
            } else if(i > 0) {
                pos += snprintf(&local_addr[pos], MAX_SIZE - pos, ":");
            }
            
            pos += snprintf(&local_addr[pos], MAX_SIZE - pos, "%x", a);
        }
    }

    free((void *)addr);
    printf("LocalIP: %s\n", local_addr);
}

uip_ipaddr_t *set_global_address(void) {
    uip_ipaddr_t *ipaddr = malloc(sizeof(uip_ipaddr_t));

    uip_ip6addr(ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(ipaddr, &uip_lladdr);
    uip_ds6_addr_add(ipaddr, 0, ADDR_AUTOCONF);

    return ipaddr;
}
