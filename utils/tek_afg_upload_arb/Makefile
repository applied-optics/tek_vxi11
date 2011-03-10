include ../../config.mk

.PHONY:	all clean install

all:	tek_afg_upload_arb

tek_afg_upload_arb: tek_afg_upload_arb.o
	$(CXX) -o $@ $^ ../../library/libtek_user.so.0 $(LDFLAGS)

tek_afg_upload_arb.o: tek_afg_upload_arb.cc
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_afg_upload_arb *.arb

install:
	$(INSTALL) tek_afg_upload_arb $(DESTDIR)$(prefix)/bin

