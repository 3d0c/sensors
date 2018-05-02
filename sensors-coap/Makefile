CONTIKI_PROJECT = coap-server

CONTIKI = ../..

RF_CHANNEL?=26
WITH_RPL?=1

# WITH_UIP6=1
# UIP_CONF_IPV6=1

CONTIKI_WITH_IPV6 = 1

APPS += er-coap 
APPS += rest-engine

CFLAGS += -DUIP_CONF_IPV6=1
CFLAGS +=-DRF_CHANNEL=$(RF_CHANNEL)
CFLAGS += -DUDPCLIENT=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECTDIRS += ./resources
PROJECT_SOURCEFILES += res-sht11.c res-battery.c res-potent.c

CONTIKI_SOURCEFILES += sht11.c potentiometer-sensor.c

SMALL=1

all: $(CONTIKI_PROJECT) 

include $(CONTIKI)/Makefile.include
