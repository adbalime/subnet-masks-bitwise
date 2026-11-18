# subnet-masks-bitwise'
# Subnet Masks, in Bitwise Terms

COMP 211 Checkoff 3: Applications of Bitwise Operators, Network IP Addresses.

Every time your laptop sends a packet, it has to decide one thing: is this destination on my local network, or do I hand it to the router? That decision is a single bitwise AND. This repo is my exploration of how that works.

## The short version

- An **IPv4 address** is just a 32-bit unsigned integer. `192.168.1.100` is `0xC0A80164`. Dotted decimal writes each of the 4 bytes in base 10.
- A **subnet mask** is a run of 1s followed by 0s. The 1s mark the **network** part of the address, and the 0s mark the **host** (device) part.
- **CIDR notation** `/n` means the first `n` bits are 1s. `/24` = `255.255.255.0`, `/27` = `255.255.255.224`.

Everything else is masking:

```c
mask      = 0xFFFFFFFFu << (32 - n);   // n = 0 needs its own case (see below)
network   = ip & mask;                 // clear the host bits
broadcast = network | ~mask;           // set all the host bits
same_net  = (a & mask) == (b & mask);  // same network?
hosts     = 2^(32 - n) - 2;            // minus network + broadcast addresses
```

## What I explored

1. What an IP address actually is (a device's "mailing address" on a network) and how it differs from a pointer in C.
2. How a mask splits an address into network and host bits, and how ANDing two addresses with the same mask tells you if they're on the same network.
3. How routers use masks to pick the next hop (longest prefix match).
4. Why masks have to be contiguous 1s.
5. Edge cases in C: shifting by 32, and the `/31` and `/32` prefixes.

## Interesting discoveries

### 1. Numbers that look close can be on different networks

With a `/27` mask, `172.16.5.94` and `172.16.5.97` are only 3 apart, but they're on **different** networks:

```
mask:  224 = 1110 0000
94  = 0101 1110  & mask = 0100 0000 = 64
97  = 0110 0001  & mask = 0110 0000 = 96
```

A `/27` cuts the last byte into blocks of 32 (`.0`, `.32`, `.64`, `.96`, ...). 94 is the end of one block and 97 is the start of the next. You can't eyeball it. You have to do the AND.

### 2. Routers run the same AND on every packet

A router keeps a table of `network / mask -> where to send`. For each packet it checks `(dest & mask) == network` on every rule, and when several match, the one with the **longest mask wins** because it's the most specific. A packet to `8.8.8.8` only matches the `/0` default route, so it goes to the next router, which does the same thing. That's how every hop across the internet works.

This only works because masks are contiguous. With a weird mask like `255.255.0.255`, two rules can partially overlap and "longest prefix" stops meaning anything.

### 3. `/0` breaks the obvious C code

The natural way to build a mask is `0xFFFFFFFFu << (32 - n)`. For `/0`, that's a shift by 32, which is **undefined behavior** in C. You can't assume it gives 0.

The first version of my program (written with AI) guarded the mask line but still did `1u << (32 - n)` in the host count, so `/0` hit the same bug in a second place. I caught it by compiling with `-fsanitize=undefined` and running `./subnet 0.0.0.0/0`, then fixed it.

Also, the `- 2` in the host count isn't always right. `/31` (point-to-point links, RFC 3021) has 2 usable addresses and `/32` is a single host, so the program handles both separately.

## The code

`src/subnet.c` takes an address in CIDR form and prints the mask, network, broadcast, usable host range and host count. It also shows the AND bit by bit. Give it a second IP and it tells you whether the two are on the same network.

### Build and run

```bash
make
./subnet                                # defaults to 192.168.1.100/26
./subnet 10.0.0.5/8                     # any a.b.c.d/n
./subnet 172.16.5.94/27 172.16.5.97     # same-network check
```

To check for undefined behavior:

```bash
gcc -Wall -fsanitize=undefined -o subnet src/subnet.c
./subnet 0.0.0.0/0
```

### Example output

```
$ ./subnet 172.16.5.94/27 172.16.5.97
Input:     172.16.5.94/27
Mask:      255.255.255.224
Network:   172.16.5.64
Broadcast: 172.16.5.95
First:     172.16.5.65
Last:      172.16.5.94
Hosts:     30

Bit view (1s in the mask keep a bit, 0s clear it):
  IP             10101100.00010000.00000101.01011110
  Mask           11111111.11111111.11111111.11100000
  IP & Mask      10101100.00010000.00000101.01000000

Same network check (each IP AND'd with the /27 mask):
  172.16.5.94/27 10101100.00010000.00000101.01000000
  172.16.5.97    10101100.00010000.00000101.01100000
Net 1:     172.16.5.64
Net 2:     172.16.5.96
DIFFERENT networks: send to the router
```

More sample runs are in [`examples/`](examples/).

## Repo layout

```
├── README.md        # this file
├── src/subnet.c     # the subnet calculator
├── Makefile
├── examples/        # sample output
├── slides/          # presentation slides (PDF)
└── RESOURCES.md     # references
```

## Takeaway

Routing on the internet comes down to the same `x & mask` we used in class: keep some bits, clear the rest, compare.

## AI use

I used Claude as a tutor to learn this topic. The initial version of `subnet.c` was generated in that conversation, and Claude also helped add the bit view and same-network check. I tested it, found the `/0` bug with the sanitizer and fixed it myself. Full conversation: https://claude.ai/share/338cbbcf-fb07-4bab-bdf3-54f4acbf3892

## Resources

See [`RESOURCES.md`](RESOURCES.md).