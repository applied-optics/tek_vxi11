include ../../config.mk

.PHONY:	all clean install

all:	tek_afg_upload_arb

tek_afg_upload_arb: tek_afg_upload_arb.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o
	$(CXX) $(CFLAGS) -o tek_afg_upload_arb tek_afg_upload_arb.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o

tek_afg_upload_arb.o: tek_afg_upload_arb.c ../../library/tek_user.c ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	$(CXX) $(CFLAGS) -c tek_afg_upload_arb.c -o tek_afg_upload_arb.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

vxi11_user.o: ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_user.cc -o vxi11_user.o 

vxi11_clnt.o: ../../../vxi11/vxi11_clnt.c
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_clnt.c -o vxi11_clnt.o

vxi11_xdr.o: ../../../vxi11/vxi11_xdr.c
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_xdr.c -o vxi11_xdr.o

clean:
	rm -f *.o tek_afg_upload_arb *.arb

install:
	$(INSTALL) tek_afg_upload_arb $(DESTDIR)$(prefix)/bin

