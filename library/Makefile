include ../config.mk

.PHONY : all install clean

all : libtek_user.so.0

libtek_user.so.0 : tek_user.o
	$(CXX) ${LDFLAGS} -shared -Wl,-soname,libtek_user.so.0 $^ -o $@ -lvxi11

tek_user.o: tek_user.cc tek_user.h
	$(CXX) -fPIC $(CFLAGS) -c $< -o $@

TAGS: $(wildcard *.cc) $(wildcard *.h)
	etags $^

clean:
	rm -f *.o libtek_user.so.0 TAGS

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	$(INSTALL) libtek_user.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	ln -sf libtek_user.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/libtek_user.so
	$(INSTALL) -d $(DESTDIR)$(prefix)/include/
	$(INSTALL) tek_user.h $(DESTDIR)$(prefix)/include/

