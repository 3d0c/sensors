#include "contiki.h"

#include <string.h>
#include "rest-engine.h"
#include "dev/potentiometer-sensor.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_potent,
        "title=\"Potentiometer sensor\";rt=\"Potentiometer\"",
        res_get_handler,
        NULL,
        NULL,
        NULL);

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
    uint16_t potent = potentiometer_sensor.value(0);

    unsigned int accept = -1;
    REST.get_header_accept(request, &accept);

    if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%i", potent);

        REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
    } else if(accept == REST.type.APPLICATION_JSON) {
        REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'Potentiometer':%i}", potent);

        REST.set_response_payload(response, buffer, strlen((char *)buffer));
    } else {
        REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
        const char *msg = "Supporting content-types text/plain and application/json";
        REST.set_response_payload(response, msg, strlen(msg));
    }
}
