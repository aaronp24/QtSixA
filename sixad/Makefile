# You know, there are pre-compile DEBs and RPMs of this...

all: sixad-bin tools hcid-bin

sixad-bin:
	gcc -Wall -O2 sdp.c textfile.c sixad.c -o bins/sixad-bin -lbluetooth
	gcc -Wall -O2 sdp.c textfile.c sixad-uinput-sixaxis.c -o bins/sixad-uinput-sixaxis -lbluetooth

tools:
	gcc -Wall -O2 sixpair.c -o bins/sixpair -lusb
	gcc -Wall -O2 sixpair_kbd.c -o bins/sixpair-kbd -lusb
	gcc -Wall -O2 sixad-raw.c -o bins/sixad-raw
	gcc -Wall -O2 hidraw-dump.c -o bins/hidraw-dump

hcid-bin:
	make all -C hcid

clean:
	rm -f bins/*
	make clean -C hcid

install:
	@echo "Run 'make install-system to really install sixad"

install-system:
	#Creating needed directories
	mkdir -p /lib/udev/rules.d/
	mkdir -p /var/lib/sixad/

	#Installing binaries
	cp bins/sixad /usr/bin/
	cp hcid/hcid /usr/sbin/
	cp bins/hidraw-dump /usr/sbin/
	cp bins/sixad-bin /usr/sbin/
	cp bins/sixad-raw /usr/sbin/
	cp bins/sixad-uinput-sixaxis /usr/sbin/
	cp bins/sixpair /usr/sbin/
	cp bins/sixpair-kbd /usr/sbin/

	#Installing misc files
	cp 98-sixad.rules /lib/udev/rules.d/
	cp default/sixad /etc/default/
	cp init/sixad /etc/init.d/
	cp x11-sony-keypad.fdi /usr/share/hal/fdi/policy/20thirdparty/
	cp mans/* usr/share/man/man1/
	ln -s /etc/init.d/sixad /etc/rc0.d/K10sixad
	ln -s /etc/init.d/sixad /etc/rc1.d/K10sixad
	ln -s /etc/init.d/sixad /etc/rc6.d/K10sixad


uninstall:
	rm -f /etc/default/sixad
	rm -f /etc/init.d/sixad
	rm -f /etc/rc0.d/K10sixad
	rm -f /etc/rc1.d/K10sixad
	rm -f /etc/rc6.d/K10sixad
	rm -f /lib/udev/rules.d/98-sixad.rules
	rm -f /usr/bin/sixad
	rm -f /usr/sbin/hcid
	rm -f /usr/sbin/hidraw-dump
	rm -f /usr/sbin/sixad-bin
	rm -f /usr/sbin/sixad-raw
	rm -f /usr/sbin/sixad-uinput-sixaxis
	rm -f /usr/sbin/sixpair
	rm -f /usr/sbin/sixpair-kbd
	rm -f /usr/share/hal/fdi/policy/20thirdparty/x11-sony-keypad.fdi
	rm -f /usr/share/man/man1/qtsixa.1
	rm -f /usr/share/man/man1/sixa.1
	rm -f /usr/share/man/man1/sixad.1
	rm -f /usr/share/man/man1/sixad-bin.1
	rm -f /usr/share/man/man1/sixad-raw.1
	rm -f /usr/share/man/man1/sixad-uinput-sixaxis.1
	rm -rf /usr/lib/sixad/
	rm -rf /var/lib/sixad/

