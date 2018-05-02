## Description

The code inside **rpl-br-json**, **sensors-coap** and **sensors-sender** folders is a firmware, based on [contiki-os](https://github.com/contiki-os/contiki), which compiles by msp430-gcc for z1 and sky platforms and can be uploaded to the real hardware devices or run inside cooja simulator.  
Provided [simulation](sensors-sender/example-sender.csc) already has preset structure of all types of nodes. There is four types - UDP, MQTT, bulklog senders (source code in **sensors-sender** folder) and CoApp server (in **sensors-coap**).  
UDP and MQTT senders is just an examples and send data in human readable form to the listeners you have.  
bulklog senders do it in bulklog format. There is three bulklog nodes, two are in range of border router and one is out of coverage, this node is always connecting (unsuccessfully) — just for demonstration. (You can move it closer to the node#1 and it will work too).  
One more note — bulklog nodes use their IPv6 addresses as a nodeids (in terms of bulklog).  

After making changes in source files, do "make TARGET=z1", then delete old node from  cooja simulation (if needed) and add new one (Motes->Add motes->Create new mote type->Z1 mote->Browse) then select the new firmware file, e.g.: udp.z1. Or just select corresponding source file and cooja will compile it.

## Host Prerequisites

### Automatic environment bootstrap

- Install [Vagrant](https://www.vagrantup.com/downloads.html)
- Run `vagrant up` inside the sensors directory
- Run `vagrant ssh` after the setup is complete to enter the linux machine.

### Manual installation

- You will need a 32bit linux host with basic xorg setup.  
Run following command, if you don't already have it.

```
apt-get install xorg openbox 
```

- Install required packages

```
sudo apt-get install build-essential ant libncurses5-dev doxygen git mosquitto mosquitto-clients netcat6 
```

- Install msp-gcc 4.7

```
wget -O /tmp/mspgcc-4.7.0-compiled.tar.bz2 http://adamdunkels.github.io/contiki-fork/mspgcc-4.7.0-compiled.tar.bz2
cd /tmp
tar jxvf mspgcc-4.7.0-compiled.tar.bz2
sudo cp -rf msp430/* /usr/local
```

- Install latest Oracle JDK. Do steps from this [guide](http://stackoverflow.com/a/23645482)

- Install bulklog. You can get compiled one from the provision/resources.

- Get contiki source

```
git clone --recursive git://github.com/contiki-os/contiki.git
```

- Copy sensors-sender, rpl-br-json and sensors-coap folders from this repo to the `contiki/examples`

## Client Prerequisites
If your host is a different machine (OS X, Win, other Linux) you will probably need an X11 client.  
For OSX install [XQuartz](https://xquartz.macosforge.org/trac).

## Run simulation

There is four types of motes: UDP, MQTT, bulklog senders and CoApp server. Run listeners for senders motes:

On the __host__ machine:

- Run UDP listener

```
nc6 --recv-only -lu -p 4000
```

- MQTT. Start subscriber process 

```
mosquitto_sub -t sensors
```

- For manual installation, run bulklog. For Vagrant — it has already been started.

On the __client__ machine.

- Run X11 forwarder  
For manual installation — `ssh -X root@hostmachine xterm`  
For Vagrant installation — `vagrant ssh -- -X xterm`

In the xterm window:

```
cd contiki/examples/sensors-sender
make cooja
```

Ignore compilation warnings. After cooja has started, do following commands in the separated terminal (no matter xterm or plain ssh):

```
cd contiki/examples/ipv6/sky-websense
make connect-router-cooja
```

Now press __start__ button in cooja simulation and you should see that all listeners receiving data.

### CoApp Motes
There is two CoApp motes, which have addresses in this simulation aaaa::c30c:0:0:8 and aaaa::c30c:0:0:9.

To discover available CoApp resources use following command:

```
coap-client -m get coap://[aaaa::c30c:0:0:8]:5683/.well-known/core
```

It will return:

```
</.well-known/core>;ct=40,</sensors/sht11>;titleors/sht11>;title="Temperature and Humidity";rt="Sht11",</sensors/battery>;title="Battery status";rt="Battery",</sensors/potent>;title="Potentiometer sensor";rt="Potentiometer"
```

To get for example battery sensor data run:

```
coap-client -A "application/json" -m get coap://[aaaa::c30c:0:0:8]:5683/sensors/battery
```
### UDP Motes
For exchange of data over UDP, the default port used will be 4000. 

Start a listener using the command 
```
nc6 --recv-only -lu -p 4000
```
In the xterm window:
```
cd contiki/examples/sensors-sender
make cooja
```
Ignore compilation warnings. After cooja has started, do following commands in the separated terminal (no matter xterm or plain ssh):
```
cd contiki/examples/ipv6/sky-websense
make connect-router-cooja
```
Now separate out all the motes in the simulation env so that they will not be able to discover anyone else.
Press the start button in cooja simulation and bring one of the UDP motes close to the border router.

You should see the listener in the xterm window start printing out incoming data.
You can now move the UDP mote away from the border router and see the incoming data stop.

As far as the port of communication is concerned, you can change the listener to whatever you want and change that inside ```sensors-sender/udp.c```.

### bulklog Motes
Let us for the sake of testing open another terminal and go to the folder of sensors-sender
We can now start another terminal of the same machine by using 
```
vagrant ssh -- -X xterm
```
Now check if anything is already running on port 2667 and kill it
```
ps aux | grep 2667
kill -9 <pid>
```
Now that 2667 is free, go to 
```
/home/vagrant/virtual-data-center/sensors/provision/resources/bulklog
```
and start bulklog using
```
 ./bulklog-32bit -v=4 -alsologtostderr
```
In the xterm window:
```
cd contiki/examples/sensors-sender
make cooja
```
Ignore compilation warnings. After cooja has started, do following commands in the separated terminal (no matter xterm or plain ssh):
```
cd contiki/examples/ipv6/sky-websense
make connect-router-cooja
```
Now separate out all the motes in the simulation env so that they will not be able to discover anyone else.
Press the start button in cooja simulation and bring one of the bulklog motes close to the border router.

You should see bulklog create a new directory called ```primary```. If you see the contents you should be able to see bulklog record incoming data into bulklog
You can now move the mote away from the border router and see the incoming data stop. 


### Nodes discovery
To automatically discover new motes, added to the simulation, use border router, which always has address aaaa::212:7401:1:101 (in this simulation). To get available routes as a json object run:

```
curl -g http://[aaaa::212:7401:1:101]
```

#Refer to wiki (contiki) to find further explanation of how the software works
