include ../../config.mk

.PHONY:	all clean install

all:	tek_afg

tek_afg: tek_afg.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o
	g++ $(CFLAGS) -o tek_afg tek_afg.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o

tek_afg.o: tek_afg.c ../../library/tek_user.c ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	g++ $(CFLAGS) -c tek_afg.c -o tek_afg.o

tek_user.o:	../../library/tek_user.c
	g++ $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

vxi11_user.o: ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_user.cc -o vxi11_user.o 

vxi11_clnt.o: ../../../vxi11/vxi11_clnt.c
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_clnt.c -o vxi11_clnt.o

vxi11_xdr.o: ../../../vxi11/vxi11_xdr.c
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_xdr.c -o vxi11_xdr.o

clean:
	rm -f *.o tek_afg

install:
	cp -f tek_afg /usr/local/bin

