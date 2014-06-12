include ../config.mk

.PHONY : all install clean

all : libtek_vxi11.so.0

libtek_vxi11.so.0 : tek_vxi11.o
	$(CXX) ${LDFLAGS} -shared -Wl,-soname,libtek_vxi11.so.0 $^ -o $@ -lvxi11

tek_vxi11.o: tek_vxi11.cc tek_vxi11.h
	$(CXX) -fPIC $(CFLAGS) -c $< -o $@

TAGS: $(wildcard *.cc) $(wildcard *.h)
	etags $^

clean:
	rm -f *.o libtek_vxi11.so.0 TAGS

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	$(INSTALL) libtek_vxi11.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	ln -sf libtek_vxi11.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/libtek_vxi11.so
	$(INSTALL) -d $(DESTDIR)$(prefix)/include/
	$(INSTALL) tek_vxi11.h $(DESTDIR)$(prefix)/include/

