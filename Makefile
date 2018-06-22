SUBDIRS = common cloud_connector cloud_structures #cli
SOURCES = $(shell find . -maxdepth 1 -name '*.c')
IINCLUDE  = -I. -I$(GANESHASRC)/include -I$(GANESHASRC)/libntirpc/ntirpc/ -Icloud_connector/ -Icloud_structures/ -Icommon/
TARGET    = libfsalcloud.so
DEST      = dest/
OBJS      = $(subst .c,.o,$(SOURCES))
CC        = gcc -g3 -Wall -Wno-pointer-sign
TESTDIRS = $(SUBDIRS:%=test-%)
INSTALLDIRS = $(SUBDIRS:%=install-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)
BUILDDIRS = $(SUBDIRS:%=build-%)

all: $(BUILDDIRS) $(TARGET)

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

ifndef GANESHASRC
    $(error export GANESHASRC=".. nfs-ganesha/src")
endif

$(TARGET): $(OBJS)
	$(CC) $(OBJS) common/common.o cloud_structures/cloud_structures.o cloud_connector/cloud_connector.o -shared -o $(TARGET)
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

