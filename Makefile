SUBDIRS = s3_connector
SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I. -I$(GANESHASRC)/include -I$(GANESHASRC)/libntirpc/ntirpc/
TARGET    = libfsals3.so
DEST      = dest/
OBJS      = $(subst .c,.o,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign
TESTDIRS = $(SUBDIRS:%=test-%)
INSTALLDIRS = $(SUBDIRS:%=install-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)
BUILDDIRS = $(SUBDIRS:%=build-%)

all: $(TARGET) $(BUILDDIRS)

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -shared -o $(TARGET)
	cp $(TARGET) $(DEST)

%.o: %.c $(HEADERS) 
	$(CC) $(IINCLUDE) -fPIC $(CCFLAGS) -c $< -o $@

clean: $(CLEANDIRS) 
	rm -f $(OBJS) $(TARGET) $(DEST)$(TARGET)

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean
test: all $(TESTDIRS)

$(TESTDIRS):
	$(MAKE) -C $(@:test-%=%) test

