CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

donut: math.c
	$(CC) $(CFLAGS) -o donut math.c $(LDFLAGS)

clean:
	rm -f donut