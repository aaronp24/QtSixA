# You know, there are pre-compile DEBs of this...

CXX ?= g++
CXXFLAGS ?= -O2 -Wall
LDFLAGS += -Wl,-Bsymbolic-functions
CC := $(CXX)

all: sixad_bins

bluez_cflags := $(shell pkg-config --cflags bluez)
bluez_libs := $(shell pkg-config --libs bluez)

targets := sixad-bin sixad-sixaxis sixad-remote sixad-raw sixad-3in1
sixad_bins: $(targets)

sixad-bin: bluetooth.o shared.o textfile.o
sixad-bin: CFLAGS += $(bluez_cflags)
sixad-bin: LDLIBS += $(bluez_libs)

sixad-sixaxis: sixaxis.o shared.o uinput.o textfile.o
sixad-sixaxis: LDLIBS += -lpthread -lrt

sixad-remote: remote.o shared.o uinput.o textfile.o
sixad-remote: LDLIBS += -lrt

sixad-raw: sixaxis.o shared.o uinput.o textfile.o

sixad-3in1: sixaxis.o shared.o uinput.o textfile.o

clean:
	$(RM) *~ $(targets) *.o

install: $(targets)
	install -d $(DESTDIR)/etc/default/
	install -d $(DESTDIR)/etc/init.d/
	install -d $(DESTDIR)/etc/logrotate.d/
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/sbin/
	install -d $(DESTDIR)/var/lib/sixad/
	install -d $(DESTDIR)/var/lib/sixad/profiles/
	install -m 644 sixad.default $(DESTDIR)/etc/default/sixad
	install -m 755 sixad.init $(DESTDIR)/etc/init.d/sixad
	install -m 644 sixad.log $(DESTDIR)/etc/logrotate.d/sixad
	install -m 755 sixad $(DESTDIR)/usr/bin/
	install -m 755 sixad-bin $(DESTDIR)/usr/sbin/
	install -m 755 sixad-sixaxis $(DESTDIR)/usr/sbin/
	install -m 755 sixad-remote $(DESTDIR)/usr/sbin/
	install -m 755 sixad-3in1 $(DESTDIR)/usr/sbin/
	install -m 755 sixad-raw $(DESTDIR)/usr/sbin/
	install -m 755 sixad-dbus-blocker $(DESTDIR)/usr/sbin/
	@chmod 777 -R $(DESTDIR)/var/lib/sixad/
	@echo "Installation is Complete!"

uninstall:
	$(RM) $(DESTDIR)/etc/default/sixad
	$(RM) $(DESTDIR)/etc/init.d/sixad
	$(RM) $(DESTDIR)/etc/logrotate.d/sixad
	$(RM) $(DESTDIR)/usr/bin/sixad
	$(RM) $(DESTDIR)/usr/sbin/sixad-bin
	$(RM) $(DESTDIR)/usr/sbin/sixad-sixaxis
	$(RM) $(DESTDIR)/usr/sbin/sixad-remote
	$(RM) $(DESTDIR)/usr/sbin/sixad-raw
	$(RM) $(DESTDIR)/usr/sbin/sixad-dbus-blocker
	$(RM) -r $(DESTDIR)/var/lib/sixad/
