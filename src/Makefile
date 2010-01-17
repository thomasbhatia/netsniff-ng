#
# Makefile for netsniff-ng
#

#
# Note: 
#   check_packets is not a main program, it is just an 
#   example implementation for a unix domain socket (uds) client.
#   Therefore, you explicitly need to build it for usage.
#   Possibly with newer version the uds API might have changed,
#   so a few manual adoptions could be possible.
#

CC               = gcc
# Use this for code analysis
#CC               = nccgen -ncgcc -ncld -ncfabs
LIBS             = -lpthread -lrt
INCLUDE          = -Iinclude
CFLAGS           = -Wall -O2

BUILD_FOLDERS    = lib

NAME             = netsniff-ng
OBJECTS          = netsniff-ng.o

BINDIR           = usr/sbin
ETCDIR           = etc
MANDIR           = usr/share/man/man8
MANDIR_LOCAL     = doc

all: clean netsniff-ng

netsniff-ng: $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(shell find $(BUILD_FOLDERS) -name "*.o") $(LIBS)

%.o: %.c
	-for d in $(BUILD_FOLDERS); do (cd $$d; $(MAKE) $(MFLAGS) all); done
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

install:
	install -D $(NAME) $(DESTDIR)/$(BINDIR)/$(NAME)
	install -d $(DESTDIR)/$(ETCDIR)/$(NAME)
	cp -r rules/ $(DESTDIR)/$(ETCDIR)/$(NAME)/
	cat $(MANDIR_LOCAL)/$(NAME).8 | gzip --best > $(MANDIR_LOCAL)/$(NAME).8.gz
	install -D $(MANDIR_LOCAL)/$(NAME).8.gz $(DESTDIR)/$(MANDIR)/$(NAME).8.gz

clean:
	-for d in $(BUILD_FOLDERS); do (cd $$d; $(MAKE) $(MFLAGS) clean); done
	rm *.o *.nccout Code.map $(NAME) $(MANDIR_LOCAL)/$(NAME).8.gz || true

uninstall:
	rm $(DESTDIR)/$(BINDIR)/$(NAME) || true
	rm -rf $(DESTDIR)/$(ETCDIR)/$(NAME) || true
	rm $(DESTDIR)/$(MANDIR)/$(NAME).8.gz || true

indent:
	$(PWD)/indent_code