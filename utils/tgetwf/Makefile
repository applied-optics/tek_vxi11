include ../../config.mk

.PHONY:	all clean install

CFLAGS:=$(CFLAGS) -I../../library

all:	tgetwf

tgetwf: tgetwf.o
	$(CXX) -o $@ $^ ../../library/libtek_vxi11.so.0 $(LDFLAGS)

tgetwf.o: tgetwf.cc
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o test.* tgetwf

install : all
	$(INSTALL) tgetwf $(DESTDIR)$(prefix)/bin/

