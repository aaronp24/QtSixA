build:
	make -C sixad

clean:
	make -C sixad clean

install:
	@echo "Run 'make install-system to really install sixad"

install-system:
	make -C sixad install-system
	make -C qtsixa install

uninstall:
	make -C sixad uninstall
	make -C qtsixa uninstall
