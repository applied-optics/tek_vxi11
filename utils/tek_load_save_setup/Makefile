include ../../config.mk

.PHONY:	all clean install

all:	tek_load_setup tek_save_setup

tek_load_setup: tek_load_setup.o tek_user.o
	$(CXX) $(CFLAGS) -o tek_load_setup tek_load_setup.o tek_user.o -lvxi11

tek_load_setup.o: tek_load_setup.c ../../library/tek_user.c 
	$(CXX) $(CFLAGS) -c tek_load_setup.c -o tek_load_setup.o

tek_save_setup: tek_save_setup.o tek_user.o 
	$(CXX) $(CFLAGS) -o tek_save_setup tek_save_setup.o tek_user.o -lvxi11

tek_save_setup.o: tek_save_setup.c ../../library/tek_user.c
	$(CXX) $(CFLAGS) -c tek_save_setup.c -o tek_save_setup.o

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c ../../library/tek_user.c -o tek_user.o

clean:
	rm -f *.o tek_load_setup tek_save_setup *.tss

install:
	$(INSTALL) tek_load_setup $(DESTDIR)$(prefix)/bin
	$(INSTALL) tek_save_setup $(DESTDIR)$(prefix)/bin

