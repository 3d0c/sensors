#!/bin/sh

WGET="wget --continue --tries=20 --waitretry=10 --retry-connrefused --no-dns-cache --timeout 300"

echo "Bootstrap environment"
sudo apt-get -y update
sudo apt-get -y install xorg openbox build-essential ant libncurses5-dev doxygen git mosquitto mosquitto-clients netcat6 autoconf curl

# Install JDK
$WGET --no-cookies --no-check-certificate --header "Cookie: gpw_e24=http%3A%2F%2Fwww.oracle.com%2F; oraclelicense=accept-securebackup-cookie" -O  /tmp/jdk-8u65-linux-i586.tar.gz http://download.oracle.com/otn-pub/java/jdk/8u65-b17/jdk-8u65-linux-i586.tar.gz

sudo mkdir /usr/java
sudo tar zxvf /tmp/jdk-8u65-linux-i586.tar.gz -C /usr/java
rm /tmp/jdk-8u65-linux-i586.tar.gz
JAVA_HOME=/usr/java/jdk1.8.0_65/
sudo update-alternatives --install /usr/bin/java java ${JAVA_HOME%*/}/bin/java 20000
sudo update-alternatives --install /usr/bin/javac javac ${JAVA_HOME%*/}/bin/javac 20000
java -version

# Install msp-gcc 4.7
$WGET -O /tmp/mspgcc-4.7.0-compiled.tar.bz2 http://adamdunkels.github.io/contiki-fork/mspgcc-4.7.0-compiled.tar.bz2
cd /tmp
tar jxvf mspgcc-4.7.0-compiled.tar.bz2
sudo cp -rf msp430/* /usr/local
rm -rf msp430
rm mspgcc-4.7.0-compiled.tar.bz2
msp430-gcc --version

# Install and run bulklog
cp -r /var/sensors/provision/resources/bulklog ~/
cd ~/bulklog
nohup ./bulklog-32bit -v=4 2>&1 >> /dev/null &

# Install contiki
cd ~
git clone --recursive git://github.com/contiki-os/contiki.git

# Copy repo folder
cp -r /var/sensors/sensors-sender ~/contiki/examples
cp -r /var/sensors/rpl-br-json ~/contiki/examples
cp -r /var/sensors/sensors-coap ~/contiki/examples

# Install libcoap
git clone https://github.com/authmillenon/libcoap.git
cd libcoap
autoconf && ./configure --prefix=/usr/local && make CFLAGS=-DNDEBUG && sudo make install && sudo cp examples/coap-client /usr/local/bin && sudo cp examples/coap-server /usr/local/bin
cd ~ && rm -rf libcoap
