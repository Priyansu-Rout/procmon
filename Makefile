CC = gcc
CFLAGS = -Wall -Wextra -O2
OBJS = main.o proc.o

procmon: $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c proc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o procmon