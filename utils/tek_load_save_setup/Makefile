include ../../config.mk

.PHONY:	all clean install

all:	tek_load_setup tek_save_setup

tek_load_setup: tek_load_setup.o tek_user.o
	$(CXX) -o $@ $^ -lvxi11 $(LDFLAGS)

tek_load_setup.o: tek_load_setup.c
	$(CXX) $(CFLAGS) -c $^ -o $@

tek_save_setup: tek_save_setup.o tek_user.o 
	$(CXX) -o $@ $^ -lvxi11 $(LDFLAGS)

tek_save_setup.o: tek_save_setup.c
	$(CXX) $(CFLAGS) -c $^ -o $@

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_load_setup tek_save_setup *.tss

install:
	$(INSTALL) tek_load_setup $(DESTDIR)$(prefix)/bin
	$(INSTALL) tek_save_setup $(DESTDIR)$(prefix)/bin

