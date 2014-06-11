MAKE=make
DIRS=tgetwf tek_load_save_setup tek_afg_upload_arb tek_afg

.PHONY : all clean install

all :
	for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean:
	for d in ${DIRS}; do $(MAKE) -C $${d} clean; done

install:
	$(INSTALL) -d $(DESTDIR)$(prefix)/usr/bin
	for d in ${DIRS}; do $(MAKE) -C $${d} install; done

