SOURCES = $(shell find . -maxdepth 1 -name '*.c')
OBJS      = $(subst .c,.o,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

all: $(OBJS)

%.o: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS)
