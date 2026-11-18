# Resources

## AI conversation
- [My Claude exploration chat](https://claude.ai/share/338cbbcf-fb07-4bab-bdf3-54f4acbf3892): the main way I learned this topic. Covers IP addresses, masks, routing, contiguous masks, and the `/0` shift bug.

## Reading
- [Cloudflare: What is my IP address?](https://www.cloudflare.com/learning/dns/glossary/what-is-my-ip-address/): what an IP address is, IPv4 vs IPv6, and how devices get one.
- [Cloudflare: What is the Internet Protocol?](https://www.cloudflare.com/learning/network-layer/internet-protocol/): how IP labels packets and gets them to the right device.
- [Cloudflare: What is the network layer?](https://www.cloudflare.com/learning/network-layer/what-is-the-network-layer/): layer 3 of the OSI model, where IP and routing live.
- [Cloudflare: What is a subnet? How subnetting works](https://www.cloudflare.com/learning/network-layer/what-is-a-subnet/): a short, beginner-friendly overview of subnets and subnet masks.
- [Cloudflare: What is routing?](https://www.cloudflare.com/learning/network-layer/what-is-routing/): how routers use routing tables to pick a path for each packet.

## Tools
- `gcc -fsanitize=undefined`: caught the shift-by-32 undefined behavior for `/0`.
