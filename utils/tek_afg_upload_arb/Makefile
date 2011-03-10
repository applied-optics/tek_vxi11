include ../../config.mk

.PHONY:	all clean install

all:	tek_afg_upload_arb

tek_afg_upload_arb: tek_afg_upload_arb.o tek_user.o 
	$(CXX) $(CFLAGS) -o tek_afg_upload_arb tek_afg_upload_arb.o tek_user.o -lvxi11

tek_afg_upload_arb.o: tek_afg_upload_arb.c ../../library/tek_user.c
	$(CXX) $(CFLAGS) -c tek_afg_upload_arb.c -o tek_afg_upload_arb.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

clean:
	rm -f *.o tek_afg_upload_arb *.arb

install:
	$(INSTALL) tek_afg_upload_arb $(DESTDIR)$(prefix)/bin

