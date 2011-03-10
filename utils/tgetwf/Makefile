CFLAGS = -g

all:	tgetwf
.PHONY:	all clean install

tgetwf: tgetwf.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o
	g++ $(CFLAGS) -o tgetwf tgetwf.o tek_user.o vxi11_user.o vxi11_clnt.o vxi11_xdr.o

tgetwf.o: tgetwf.c ../../library/tek_user.c ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	g++ $(CFLAGS) -c tgetwf.c -o tgetwf.o

tek_user.o:	../../library/tek_user.c
	g++ $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

vxi11_user.o: ../../../vxi11/vxi11_user.cc ../../../vxi11/vxi11.h
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_user.cc -o vxi11_user.o 

vxi11_clnt.o: ../../../vxi11/vxi11_clnt.c
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_clnt.c -o vxi11_clnt.o

vxi11_xdr.o: ../../../vxi11/vxi11_xdr.c
	g++ $(CLFAGS) -c ../../../vxi11/vxi11_xdr.c -o vxi11_xdr.o

clean:
	rm -f *.o test.* tgetwf

install:
	cp -f tgetwf /usr/local/bin

