# Makefile

unexport CFLAGS CPPFLAGS CXXFLAGS LDFLAGS

PATH=/opt/poky/3.3.3/sysroots/x86_64-pokysdk-linux/usr/bin:/opt/poky/3.3.3/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi:/bin:/usr/bin:/sbin:/usr/sbin

KERNEL_SRC ?= "/opt/poky/3.3.3/sysroots/cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi/usr/src/kernel"
CROSS_COMPILE ?= "arm-poky-linux-gnueabi-"
ARCH ?= arm

mods := rpi2exp-leds rpi2exp-btns rpi2exp-adc rpi2exp-pot rpi2exp-temp rpi2exp-lcd
dts := rpi2exp-overlay.dts

obj-m += $(mods:%=%.o)

SRC := $(shell pwd)

all: scripts prepare modules rpi2exp-overlay.dtb
	

scripts:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) scripts

prepare:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) prepare

%.dtb: %.dts
	$(KERNEL_SRC)/scripts/dtc/dtc -@ -I dts -O dtb -o $@ $<

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
	rm *.dtb || true

modules:
	$(MAKE) -C $(KERNEL_SRC) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
CC="arm-poky-linux-gnueabi-gcc  -mno-thumb-interwork -marm -fuse-ld=bfd" \
LD="arm-poky-linux-gnueabi-ld.bfd " \
AR="arm-poky-linux-gnueabi-ar " \
M=$(SRC) modules
