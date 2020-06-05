# icmp2ip

[whatsmyip via ping](http://systemadmin.es/2013/02/whatsmyip-con-ping)

## Demo

Please notice how the IP is returned as the TTL of the ping response:

```
[root@qapla ~]# ping whatsmyip.systemadmin.es
PING whatsmyip.systemadmin.es (54.38.10.67) 56(84) bytes of data.
64 bytes from mail.thebohogroup.com (54.38.10.67): icmp_seq=1 ttl=91 time=9.43 ms
64 bytes from mail.thebohogroup.com (54.38.10.67): icmp_seq=2 ttl=121 time=9.65 ms
64 bytes from mail.thebohogroup.com (54.38.10.67): icmp_seq=3 ttl=142 time=9.41 ms
64 bytes from mail.thebohogroup.com (54.38.10.67): icmp_seq=4 ttl=67 time=9.39 ms
^C
--- whatsmyip.systemadmin.es ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3003ms
rtt min/avg/max/mdev = 9.398/9.475/9.655/0.158 ms
[root@qapla ~]# curl icanhazip.com
91.121.142.67
```

## Install

```
make all
insmod ./icmp2ip.ko
```
