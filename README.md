# icmp2ip — Encode IP Address in ICMP Echo Reply TTL

A Linux kernel module that hooks netfilter to embed the source IP address of an incoming ICMP Echo request into the TTL field of the corresponding ICMP Echo reply. This allows a remote host to learn its own public IP by simply running `ping` against a server running this module — without needing any application-layer service like HTTP or DNS.

## How It Works

When the server receives an ICMP Echo request, the module stores the source IP address and the original TTL of the incoming packet. When the corresponding Echo reply is about to leave the server, the module overwrites the TTL field with one octet of the stored source IP (encoded as a value between 0 and 255) plus an offset derived from the original TTL.

Because standard `ping` output displays the TTL of each reply, the client can reconstruct its own IP address over a series of ping packets. The module cycles through the four octets of the IP address (from the least significant to the most significant), sending one octet per reply. After the fourth octet it resets to the first octet.

The offset accounts for the difference between the assumed initial TTL (64, the default in both Linux and Windows) and the actual TTL observed in the incoming request, under the assumption that the round-trip path is symmetric.

## Limitations

- **8-bit TTL field**: The TTL is only 8 bits, so IP addresses with octets larger than 255 minus the hop-count offset cannot be represented correctly. This is typically not an issue on the same LAN, but on WAN links some high IP octets may be truncated.
- **Concurrent pings**: Multiple concurrent pings from the same source IP will intermix octets across connections, producing garbled results.
- **Fixed-size tracking array**: The module uses a fixed array of 30 slots to track ongoing connections. If the limit is exceeded, the least recently used slot is reused.
- **State persistence**: If an IP address is not fully transmitted (e.g., the client stops pinging after fewer than 4 replies), the next ping from the same source will resume from the next octet, provided the slot hasn't been recycled by another connection.
- **Localhost (127.0.0.1)**: Works locally, though the first reply will show TTL 127 (the Linux loopback default) followed by the encoded octets.

## Usage

```
make all
insmod ./icmp2ip.ko
```

Once loaded, any ICMP Echo request received by the host will cause the reply to carry the source IP encoded in the TTL field. No additional configuration is needed.

## Example

Client pings the server (IP 172.16.1.1) from source IP 10.25.11.1:

```
# ping 172.16.1.1
PING 172.16.0.100 (172.16.1.1) 56(84) bytes of data.
64 bytes from 172.16.1.1: icmp_seq=1 ttl=10 time=2.61 ms
64 bytes from 172.16.1.1: icmp_seq=2 ttl=25 time=1.86 ms
64 bytes from 172.16.1.1: icmp_seq=3 ttl=11 time=2.27 ms
64 bytes from 172.16.1.1: icmp_seq=4 ttl=1  time=2.31 ms
```

The TTL values 10, 25, 11, 1 correspond to the four octets of the source IP (10.25.11.1).

## Demo Server

A public test server is not available.

## Build Requirements

- Linux kernel headers matching the running kernel
- `make` and a C compiler

## Build

```
make
```

## License

GPL — see [LICENSE](LICENSE).
