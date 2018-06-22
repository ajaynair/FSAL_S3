SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I. -I../../../common/ -I../../ #-I$(GANESHASRC)/include -I$(GANESHASRC)libntirpc/ntirpc/
OBJS      = $(subst .c,.o,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif
all: $(OBJS)

%.o: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) -c $< -o $@
	$(CC) s3_connector.o -shared -o libs3connector.so -ls3

test: $(OBJS)
	$(CC) $(IINCLUDE) $(CCFLAGS) libs3connector.so unittest/basic.c -o basic
	
clean: 
	rm -f $(OBJS) basic
