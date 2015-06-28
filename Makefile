# used by Linux kernel
obj-y = mymain.o myinterrupt.o

# the generic parts
LINUX_URL = https://www.kernel.org/pub/linux/kernel/v4.x
KERNEL = linux-4.1.tar.xz
LINUX_FILE = downloads/$(KERNEL)
LINUX_MD5 = "fe9dc0f6729f36400ea81aa41d614c37"
PATCH_FILE = linux-4_1-mykernel.patch

ALL = stamps/downloads stamps/setup stamps/build
.PHONY: all
all: $(ALL)

TMPFILE := $(shell mktemp)

stamps:
	@mkdir -p stamps

to-md5 = $1 $(addsuffix .md5,$1)
%.md5: FORCE
	@$(if $(filter-out $(shell cat $@ 2>/dev/null),$(shell md5sum $*)),md5sum $* > $@)
FORCE:

$(LINUX_FILE):
	mkdir -p downloads
	(cd downloads; wget -c $(LINUX_URL)/$(KERNEL))

stamps/downloads: $(call to-md5,$(LINUX_FILE)) stamps
	@echo $(LINUX_MD5) > $(TMPFILE)
	@cmp -n 32 $(TMPFILE) $<.md5 >/dev/null || (echo "File checksum mismatch!"; exit 1)
	@touch $@

stamps/setup: stamps stamps/extract stamps/patch stamps/config
	@touch $@

stamps/extract: downloads/linux-4.1.tar.xz
	tar Jxf $<
	@touch $@

stamps/patch:
	(cd linux-4.1; \
	 patch --dry-run -f -p1 < ../patches/$(PATCH_FILE) >/dev/null && \
	 patch -p1 < ../patches/$(PATCH_FILE)) || touch $@

stamps/config:
	(cd linux-4.1; \
	 cp -f ../configs/mini-x86.config .config; \
	 make oldconfig)
	@touch $@

# number of CPUs
ifndef CPUS
CPUS := $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null || \
          sysctl -n hw.ncpu)                                          
endif

stamps/build: linux-4.1/Makefile \
              src/myinterrupt.c src/mymain.c src/mypcb.h
	(cd linux-4.1; $(MAKE) -j $(CPUS))
	@touch $@

run: linux-4.1/arch/x86/boot/bzImage
	qemu-system-i386 -kernel $<

clean:
	$(MAKE) -C linux-4.1 clean
	# FIXME: clean directory src as well
	rm -f src/built-in.o src/.built-in.o.cmd \
	      src/modules.order \
	      src/myinterrupt.o src/.myinterrupt.o.cmd \
	      src/mymain.o src/.mymain.o.cmd
	rm -f stamps/build

distclean: clean
	rm -rf stamps
	rm -rf linux-4.1
