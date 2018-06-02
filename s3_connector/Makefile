SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I. -I../common/ #-I$(GANESHASRC)/include -I$(GANESHASRC)libntirpc/ntirpc/
OBJS      = $(subst .c,.o,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif
all: $(OBJS)

%.o: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) -c $< -o $@

test: $(OBJS)
	$(CC) $(IINCLUDE) $(CCFLAGS) $(OBJS) unittest/basic.c -o basic -ls3	
	
clean: 
	rm -f $(OBJS) basic
