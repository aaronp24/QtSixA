# You know, there are pre-compile DEBs of this...

DESTDIR =
CXXFLAGS := -Wall -O2
CC := $(CXX)

all: sixad_bins

targets := sixad-bin sixad-sixaxis sixad-raw
sixad_bins: $(targets)

sixad-bin: bluetooth.o shared.o textfile.o
sixad-bin: LDLIBS += -lbluetooth

sixad-sixaxis: sixaxis.o shared.o uinput.o textfile.o
sixad-sixaxis: LDLIBS += -lpthread

sixad-raw: sixaxis.o shared.o uinput.o textfile.o

clean:
	rm -f *~ $(targets) *.o

extra_files := \
    sixad.default \
    sixad.init \
    sixad.log \
    sixad \
    98-sixad.rules

install: $(targets) $(extra_files)
	install -d $(DESTDIR)/etc/default/
	install -d $(DESTDIR)/etc/init.d/
	install -d $(DESTDIR)/etc/logrotate.d/
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/sbin/
	install -d $(DESTDIR)/lib/udev/rules.d/
	install -d $(DESTDIR)/var/lib/sixad/
	install -m 644 sixad.default $(DESTDIR)/etc/default/sixad
	install -m 755 sixad.init $(DESTDIR)/etc/init.d/sixad
	install -m 644 sixad.log $(DESTDIR)/etc/logrotate.d/sixad
	install -m 755 sixad $(DESTDIR)/usr/bin/
	install -m 755 sixad-bin $(DESTDIR)/usr/sbin/
	install -m 755 sixad-sixaxis $(DESTDIR)/usr/sbin/
	install -m 755 sixad-raw $(DESTDIR)/usr/sbin/
	install -m 644 98-sixad.rules $(DESTDIR)/lib/udev/rules.d/
	@echo "Please run '[sudo] make install-system' now if you're installing from source"

install-system:
	@addgroup --system sixad || true
	@chown :sixad $(DESTDIR)/var/lib/sixad/
	@chmod 775 -R $(DESTDIR)/var/lib/sixad/
	@echo "Installation is Complete!"
	@echo "You may want to add yourself to the sixad group now:"
	@echo "$ [sudo] adduser YOUR-USER sixad"

uninstall:
	rm -f $(DESTDIR)/etc/default/sixad
	rm -f $(DESTDIR)/etc/init.d/sixad
	rm -f $(DESTDIR)/etc/logrotate.d/sixad
	rm -f $(DESTDIR)/usr/bin/sixad
	rm -f $(DESTDIR)/usr/sbin/sixad-bin
	rm -f $(DESTDIR)/usr/sbin/sixad-sixaxis
	rm -f $(DESTDIR)/usr/sbin/sixad-raw
	rm -f $(DESTDIR)/lib/udev/rules.d/98-sixad.rules
	rm -rf $(DESTDIR)/var/lib/sixad/
