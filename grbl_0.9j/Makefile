GRBL = grbl-master
WORK_DIR = $(GRBL)

TARFILE = $(GRBL).tar.bz2

PATCHFILES = $(sort $(wildcard patches/*.patch))

PATCH_CMD = \
        for f in $(PATCHFILES); do\
            echo $$f; \
            patch -s -d $(WORK_DIR) -p1 -b -z .original < $$f; \
        done

all:
	tar jxf $(TARFILE)
	$(PATCH_CMD)
	cd files; tar -c -f - * | (cd ../$(WORK_DIR) ; tar xfp -)

clean:
	-rm -rf $(WORK_DIR)
