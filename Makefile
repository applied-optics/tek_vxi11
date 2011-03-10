MAKE=make
DIRS=utils

.PHONY : all clean install

all :
	for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean:
	for d in ${DIRS}; do $(MAKE) -C $${d} clean; done

install:
	for d in ${DIRS}; do $(MAKE) -C $${d} install; done
