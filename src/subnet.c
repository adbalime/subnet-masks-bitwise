#include <stdint.h>
#include <stdio.h>

static void print_ip(const char* label, uint32_t ip) {
    printf("%-10s %u.%u.%u.%u\n", label, (ip >> 24) & 0xFF, (ip >> 16) & 0xFF,
           (ip >> 8) & 0xFF, ip & 0xFF);
}

/* Print all 32 bits, with a dot between each byte, e.g. 11000000.10101000... */
static void print_bin(const char* label, uint32_t x) {
    printf("  %-14s ", label);
    for (int i = 31; i >= 0; i--) {
        putchar(((x >> i) & 1u) ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar('.');
    }
    putchar('\n');
}

/* Parse "a.b.c.d" into a uint32_t. Returns 1 on success, 0 on bad input. */
static int parse_ip(const char* s, uint32_t* out) {
    unsigned a, b, c, d;
    char extra;
    if (sscanf(s, "%u.%u.%u.%u%c", &a, &b, &c, &d, &extra) != 4 || a > 255 ||
        b > 255 || c > 255 || d > 255) {
        return 0;
    }
    *out = (a << 24) | (b << 16) | (c << 8) | d;
    return 1;
}

int main(int argc, char* argv[]) {
    const char* input = (argc > 1) ? argv[1] : "192.168.1.100/26";
    unsigned a, b, c, d, n;

    if (sscanf(input, "%u.%u.%u.%u/%u", &a, &b, &c, &d, &n) != 5 || a > 255 ||
        b > 255 || c > 255 || d > 255 || n > 32) {
        fprintf(stderr, "usage: %s a.b.c.d/n [other.ip]\n", argv[0]);
        return 1;
    }

    uint32_t ip = (a << 24) | (b << 16) | (c << 8) | d;
    uint32_t mask =
        (n == 0) ? 0 : 0xFFFFFFFFu << (32 - n); /* shift by 32 is UB */
    uint32_t net = ip & mask;
    uint32_t bcast = net | ~mask;

    printf("Input:     %s\n", input);
    print_ip("Mask:", mask);
    print_ip("Network:", net);
    print_ip("Broadcast:", bcast);

    if (n >= 31) {
        /* /31 and /32 have no separate network/broadcast addresses */
        print_ip("First:", net);
        print_ip("Last:", bcast);
        printf("Hosts:     %u\n", (n == 32) ? 1u : 2u);
    } else {
        print_ip("First:", net + 1);
        print_ip("Last:", bcast - 1);
        printf("Hosts:     %u\n", ~mask - 1);
    }

    /* Demo 1: show the AND bit by bit */
    printf("\nBit view (1s in the mask keep a bit, 0s clear it):\n");
    print_bin("IP", ip);
    print_bin("Mask", mask);
    print_bin("IP & Mask", net);

    /* Demo 2: optional second IP -> same network or not? */
    if (argc > 2) {
        uint32_t other;
        if (!parse_ip(argv[2], &other)) {
            fprintf(stderr, "bad second IP: %s\n", argv[2]);
            return 1;
        }
        uint32_t other_net = other & mask;

        printf("\nSame network check (each IP AND'd with the /%u mask):\n", n);
        print_bin(argv[1], net);
        print_bin(argv[2], other_net);
        print_ip("Net 1:", net);
        print_ip("Net 2:", other_net);

        if (net == other_net) {
            printf("SAME network: send directly\n");
        } else {
            printf("DIFFERENT networks: send to the router\n");
        }
    }
    return 0;
}
