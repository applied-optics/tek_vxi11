include ../../config.mk

.PHONY:	all clean install

all:	tek_load_setup tek_save_setup

tek_load_setup: tek_load_setup.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o
	$(CXX) $(CFLAGS) -o tek_load_setup tek_load_setup.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o

tek_load_setup.o: tek_load_setup.c ../../library/tek_user.c ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	$(CXX) $(CFLAGS) -c tek_load_setup.c -o tek_load_setup.o

tek_save_setup: tek_save_setup.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o
	$(CXX) $(CFLAGS) -o tek_save_setup tek_save_setup.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o

tek_save_setup.o: tek_save_setup.c ../../library/tek_user.c ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	$(CXX) $(CFLAGS) -c tek_save_setup.c -o tek_save_setup.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

vxi11_user.o: ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_user.cc -o vxi11_user.o 

vxi11_clnt.o: ../../../vxi11/vxi11_clnt.c
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_clnt.c -o vxi11_clnt.o

vxi11_xdr.o: ../../../vxi11/vxi11_xdr.c
	$(CXX) $(CLFAGS) -c ../../../vxi11/vxi11_xdr.c -o vxi11_xdr.o

clean:
	rm -f *.o tek_load_setup tek_save_setup *.tss

install:
	$(INSTALL) tek_load_setup $(DESTDIR)$(prefix)/bin
	$(INSTALL) tek_save_setup $(DESTDIR)$(prefix)/bin

