include ../../config.mk

.PHONY:	all clean install

CFLAGS:=$(CFLAGS) -I../../library

all:	tek_load_setup tek_save_setup

tek_load_setup: tek_load_setup.o
	$(CXX) -o $@ $^ ../../library/$(full_libname) $(LDFLAGS)

tek_load_setup.o: tek_load_setup.cc
	$(CXX) $(CFLAGS) -c $^ -o $@

tek_save_setup: tek_save_setup.o
	$(CXX) -o $@ $^ ../../library/$(full_libname) $(LDFLAGS)

tek_save_setup.o: tek_save_setup.cc
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_load_setup tek_save_setup *.tss

install : all
	$(INSTALL) tek_load_setup $(DESTDIR)$(prefix)/bin/
	$(INSTALL) tek_save_setup $(DESTDIR)$(prefix)/bin/

