include ../../config.mk

.PHONY:	all clean install

all:	tek_afg

tek_afg: tek_afg.o tek_user.o
	$(CXX) $(CFLAGS) -o tek_afg tek_afg.o tek_user.o -lvxi11

tek_afg.o: tek_afg.c ../../library/tek_user.c
	$(CXX) $(CFLAGS) -c tek_afg.c -o tek_afg.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

clean:
	rm -f *.o tek_afg

install:
	$(INSTALL) tek_afg $(DESTDIR)$(prefix)/bin

