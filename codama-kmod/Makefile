obj-m += snd-soc-codama-soundcard.o snd-soc-codama-codec.o
KDIR ?= /lib/modules/`uname -r`/build
PWD := $(shell pwd)

all: devicetree
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install: devicetree
	cp snd-soc-codama-codec.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/bcm/
	cp snd-soc-codama-soundcard.ko /lib/modules/$(shell uname -r)/kernel/sound/soc/bcm/
	cp codama-soundcard.dtbo /boot/overlays/
	depmod -a

clean: clean-local
	$(MAKE) -C $(KDIR) M=$(PWD) clean

clean-local:
	rm codama-soundcard.dtbo || true

codama-soundcard.dtbo:
	dtc -@ -I dts -O dtb -o codama-soundcard.dtbo codama-soundcard-overlay.dts

devicetree: codama-soundcard.dtbo
