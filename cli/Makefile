SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I../ 
OBJS      = $(subst .c,,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif
all: $(OBJS)

%: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) $< -o $@

clean: 
	rm -f $(OBJS)
