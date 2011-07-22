#!/bin/bash

# You need to install ppu-gcc before running this script
if [ -f /usr/lib/cell/sysroot/usr/include/a.out.h ]; then

sudo mkdir -p /usr/lib/cell/sysroot/usr/include/bluetooth
sudo cp /usr/include/bluetooth/* /usr/lib/cell/sysroot/usr/include/bluetooth/
sudo cp /usr/include/usb.h /usr/lib/cell/sysroot/usr/include/

sudo cp lib/* /usr/lib/cell/sysroot/usr/lib64/
sudo cp lib/* /usr/lib/cell/sysroot/lib64/

else

echo "ppu-sysroot and ppu-gcc are not installed!
please install them first"

fi
