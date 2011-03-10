include ../../config.mk

.PHONY:	all clean install

all:	tgetwf

tgetwf: tgetwf.o tek_user.o
	$(CXX) $(CFLAGS) -o tgetwf tgetwf.o tek_user.o -lvxi11

tgetwf.o: tgetwf.c ../../library/tek_user.c
	$(CXX) $(CFLAGS) -c tgetwf.c -o tgetwf.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

clean:
	rm -f *.o test.* tgetwf

install:
	$(INSTALL) tgetwf $(DESTDIR)$(prefix)/bin

