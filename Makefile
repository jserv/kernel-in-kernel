# used by Linux kernel
obj-y = mymain.o myinterrupt.o

# the generic parts
LINUX_URL = https://www.kernel.org/pub/linux/kernel/v4.x
KERNEL = linux-4.1
LINUX_FILE = downloads/$(KERNEL).tar.xz
LINUX_MD5 = "fe9dc0f6729f36400ea81aa41d614c37"
PATCH_FILE = linux-4_1-mykernel.patch
OUT = $(PWD)/.out

ALL = .stamps/downloads .stamps/setup .stamps/build
.PHONY: all
all: $(ALL)

TMPFILE := $(shell mktemp)

.stamps:
	@mkdir -p $@

to-md5 = $1 $(addsuffix .md5,$1)
%.md5: FORCE
	@$(if $(filter-out $(shell cat $@ 2>/dev/null),$(shell md5sum $*)),md5sum $* > $@)
FORCE:

$(LINUX_FILE):
	mkdir -p downloads
	(cd downloads; wget -c $(LINUX_URL)/$(KERNEL).tar.xz)

.stamps/downloads: $(call to-md5,$(LINUX_FILE)) .stamps
	@echo $(LINUX_MD5) > $(TMPFILE)
	@cmp -n 32 $(TMPFILE) $<.md5 >/dev/null || (echo "File checksum mismatch!"; exit 1)
	@touch $@

.stamps/setup: .stamps/extract .stamps/patch .stamps/config
	@touch $@

.stamps/extract: $(LINUX_FILE)
	tar Jxf $<
	(cd $(KERNEL); ln -s ../src mysrc)
	@touch $@

.stamps/patch:
	(cd $(KERNEL); \
	 patch --dry-run -f -p1 < ../patches/$(PATCH_FILE) >/dev/null && \
	 patch -p1 < ../patches/$(PATCH_FILE)) || touch $@

.stamps/config:
	@mkdir -p $(OUT)
	(cd $(KERNEL); \
	 cp -f ../configs/mini-x86.config $(OUT)/.config; \
	 make O=$(OUT) oldconfig)
	@touch $@

# number of CPUs
ifndef CPUS
CPUS := $(shell grep -c ^processor /proc/cpuinfo 2>/dev/null || \
          sysctl -n hw.ncpu)                                          
endif

.stamps/build: $(KERNEL)/Makefile \
              src/scheduler.c src/main.c src/mypcb.h
	(cd $(KERNEL); $(MAKE) O=$(OUT) -j $(CPUS))
	@touch $@

run: $(OUT)/arch/x86/boot/bzImage
	qemu-system-i386 -kernel $<

clean:
	$(MAKE) -C $(KERNEL) O=$(OUT) clean
	rm -f .stamps/build

distclean: clean
	rm -rf .stamps $(OUT)
	rm -rf $(KERNEL)
