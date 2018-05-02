CONTIKI_PROJECT = udp mqtt-sender bulklog-sender

CONTIKI = ../..
COOJADIR=../../../tools/cooja

RF_CHANNEL?=26
WITH_RPL?=1

# WITH_UIP6=1
# UIP_CONF_IPV6=1

CONTIKI_WITH_IPV6 = 1

APPS = mqtt

CFLAGS += -DUIP_CONF_IPV6=1
CFLAGS +=-DRF_CHANNEL=$(RF_CHANNEL)
CFLAGS += -DUDPCLIENT=1

# CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += common.c

CONTIKI_SOURCEFILES += sht11.c potentiometer-sensor.c

ifeq ($(TARGET),sky)
MSP430_20BITS=0
endif

all: $(CONTIKI_PROJECT) 

cooja: all rpl-br-json build-cooja launch-cooja

build-cooja:
	cd ${CONTIKI}/tools/cooja; ant jar; cd -

launch-cooja:
	mkdir tmp; cd tmp; java -jar ${COOJADIR}/dist/cooja.jar -quickstart=../example-sender.csc; cd -; rm -rf tmp

rpl-br-json:
	cd ../rpl-br-json;make;cd -

include $(CONTIKI)/Makefile.include
