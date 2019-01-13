CC=gcc
CFLAGS=-I. -g
DEPS = utils.h
OBJS = main.o utils.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

my_ls: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

