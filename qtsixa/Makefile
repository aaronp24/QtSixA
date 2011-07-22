build:
	@echo "QtSixA doesn't need to be compiled :)"

clean:
	@echo "QtSixA doesn't need to be compiled :)"

install:
	cp ./doc/* /usr/share/doc/qtsixa/
	cp ./manual/* /usr/share/doc/qtsixa/manual/
	cp ./game-profiles/* /usr/share/qtsixa/game-profiles/
	cp ./gui/* /usr/share/qtsixa/gui/
	cp ./pics/* /usr/share/qtsixa/pics/
	cp ./lang/* /usr/share/qtsixa/lang/
	cp ./icons/* /usr/share/qtsixa/icons/
	cp ./sixaxis-profiles/* /usr/share/qtsixa/sixaxis-profiles/
	cp ./sixa /usr/bin/
	cp ./sixa-lq /usr/bin/
	cp ./sixa-notify /usr/bin/
	cp ./qtsixa /usr/bin/
	cp ./qtsixa.conf /usr/share/qtsixa/
	cp ./qtsixa.desktop /usr/share/applications/
	cp ./qtsixa.xpm /usr/share/pixmaps/
	cp ./profiles.list.bu /usr/share/qtsixa/
	cp ./features.html /usr/share/qtsixa/
	cp ./sixa-notify.desktop /usr/share/qtsixa/
	if [ ! -f /usr/share/qtsixa/profiles.list.bu ]; then cp ./profiles.list.bu /usr/share/qtsixa/profiles.list; fi

### Note to packagers:
#  The real "profiles.list" should not be installed by default, only if it is not found.
#  This avoids overritting a possibly customized file


uninstall:
	rm -rf /usr/share/doc/qtsixa/
	rm -rf /usr/share/qtsixa/
	rm -rf /usr/bin/sixa
	rm -rf /usr/bin/sixa-lq
	rm -rf /usr/bin/sixa-notify
	rm -rf /usr/bin/qtsixa
	rm -rf /usr/share/applications/qtsixa.desktop
	rm -rf /usr/share/pixmaps/qtsixa.xpm
	rm -rf /etc/hal/fdi/policy/*sixa*.fdi
