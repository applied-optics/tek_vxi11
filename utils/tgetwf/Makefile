include ../../config.mk

.PHONY:	all clean install

all:	tgetwf

tgetwf: tgetwf.o tek_user.o
	$(CXX) -o $@ $^ -lvxi11 $(LDFLAGS)

tgetwf.o: tgetwf.c
	$(CXX) $(CFLAGS) -c $^ -o $@

tek_user.o:	../../library/tek_user.c
	$(CXX) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o test.* tgetwf

install:
	$(INSTALL) tgetwf $(DESTDIR)$(prefix)/bin

