# icmp2ip

[whatsmyip via ping](http://systemadmin.es/2013/02/whatsmyip-con-ping)

## Install

```
make all
insmod ./icmp2ip.ko
```

## Demo

```
$ ping localhost
PING picamoixons (127.0.0.1) 56(84) bytes of data.
64 bytes from picamoixons (127.0.0.1): icmp_req=1 ttl=127 time=0.066 ms
64 bytes from picamoixons (127.0.0.1): icmp_req=2 ttl=0 time=0.059 ms
64 bytes from picamoixons (127.0.0.1): icmp_req=3 ttl=0 time=0.060 ms
64 bytes from picamoixons (127.0.0.1): icmp_req=4 ttl=1 time=0.060 ms
^C
--- picamoixons ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 2999ms
rtt min/avg/max/mdev = 0.059/0.061/0.066/0.006 ms
```
