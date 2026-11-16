CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

subnet: src/subnet.c
	$(CC) $(CFLAGS) -o subnet src/subnet.c

clean:
	rm -f subnet