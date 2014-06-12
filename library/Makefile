include ../config.mk

.PHONY : all install clean

all : $(full_libname)

$(full_libname) : tek_vxi11.o
	$(CXX) ${LDFLAGS} -shared -Wl,-soname,$(full_libname) $^ -o $@ -lvxi11

tek_vxi11.o: tek_vxi11.cc tek_vxi11.h
	$(CXX) -fPIC $(CFLAGS) -c $< -o $@

TAGS: $(wildcard *.cc) $(wildcard *.h)
	etags $^

clean:
	rm -f *.o $(full_libname) TAGS

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	$(INSTALL) $(full_libname) $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	ln -sf $(full_libname) $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/$(libname)
	$(INSTALL) -d $(DESTDIR)$(prefix)/include/
	$(INSTALL) tek_vxi11.h $(DESTDIR)$(prefix)/include/

