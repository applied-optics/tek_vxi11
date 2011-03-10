include ../../config.mk

.PHONY:	all clean install

all:	tek_afg_upload_arb

tek_afg_upload_arb: tek_afg_upload_arb.o tek_user.o 
	$(CXX) -o $@ $^ -lvxi11 $(LDFLAGS)

tek_afg_upload_arb.o: tek_afg_upload_arb.c
	$(CXX) $(CFLAGS) -c $^ -o $@

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_afg_upload_arb *.arb

install:
	$(INSTALL) tek_afg_upload_arb $(DESTDIR)$(prefix)/bin

