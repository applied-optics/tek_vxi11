include ../../config.mk

.PHONY:	all clean install

all:	tek_afg

tek_afg: tek_afg.o
	$(CXX) -o $@ $^ ../../library/libtek_user.so.0 $(LDFLAGS)

tek_afg.o: tek_afg.c
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o tek_afg

install:
	$(INSTALL) tek_afg $(DESTDIR)$(prefix)/bin

