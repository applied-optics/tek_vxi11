include ../../config.mk

.PHONY:	all clean install

CFLAGS:=$(CFLAGS) -I../../library

all:	tek_afg_upload_arb

tek_afg_upload_arb: tek_afg_upload_arb.o
	$(CXX) -o $@ $^ ../../library/libtek_vxi11.so.0 $(LDFLAGS)

tek_afg_upload_arb.o: tek_afg_upload_arb.cc
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_afg_upload_arb *.arb

install : all
	$(INSTALL) tek_afg_upload_arb $(DESTDIR)$(prefix)/bin/

