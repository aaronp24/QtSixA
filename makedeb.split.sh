#!/bin/bash
#Small script to automatically create the deb files for sixad
VERSION="0.7-falktx1"

#Make needed directories
make_dirs () {
  mkdir ./sixad_deb_$ARCH
  mkdir ./sixad_deb_$ARCH/DEBIAN
  mkdir ./sixad_deb_$ARCH/etc
  mkdir ./sixad_deb_$ARCH/etc/default
  mkdir ./sixad_deb_$ARCH/etc/hal
  mkdir ./sixad_deb_$ARCH/etc/hal/fdi
  mkdir ./sixad_deb_$ARCH/etc/hal/fdi/policy
  mkdir ./sixad_deb_$ARCH/etc/init.d
  mkdir ./sixad_deb_$ARCH/lib
  mkdir ./sixad_deb_$ARCH/lib/udev
  mkdir ./sixad_deb_$ARCH/lib/udev/rules.d
  mkdir ./sixad_deb_$ARCH/usr
  mkdir ./sixad_deb_$ARCH/usr/bin
  mkdir ./sixad_deb_$ARCH/usr/lib
  mkdir ./sixad_deb_$ARCH/usr/lib/sixad
  mkdir ./sixad_deb_$ARCH/usr/sbin
}

copy_files () {
  cp DEBIAN.split/control.$ARCH ./sixad_deb_$ARCH/DEBIAN/control
  cp DEBIAN.split/conffiles.all ./sixad_deb_$ARCH/DEBIAN/conffiles
  cp DEBIAN.split/postrm.all ./sixad_deb_$ARCH/DEBIAN/postrm
  cp DEBIAN.split/preinst.all ./sixad_deb_$ARCH/DEBIAN/preinst
  cp sixad ./sixad_deb_$ARCH/usr/bin/
  cp sixad.default ./sixad_deb_$ARCH/etc/default/sixad
  cp sixad.default ./sixad_deb_$ARCH/etc/default/sixad.bak
  cp sixad.init ./sixad_deb_$ARCH/etc/init.d/sixad
  cp 98-sixad.rules ./sixad_deb_$ARCH/lib/udev/rules.d/
  cp DEBIAN.split/control.$ARCH ./sixad_deb_$ARCH/DEBIAN/control
  cp bins/hidraw-dump_$ARCH ./sixad_deb_$ARCH/usr/sbin/hidraw-dump
  cp bins/sixpair_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixpair
  cp bins/sixpair-kbd_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixpair-kbd
  cp bins/sixad-raw_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-raw
  cp bins/sixad-bin_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-bin
  cp bins/sixad-uinput-sixaxis_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-uinput-sixaxis
  cp compat/hcid_$ARCH ./sixad_deb_$ARCH/usr/sbin/hcid
  cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib/libbluetooth.so.2
  cp x11-sony-keypad.fdi ./sixad_deb_$ARCH/etc/hal/fdi/policy/
}

copy_lib () { #Only for PowerPC
  cp compat/libbluetooth.so.3_powerpc ./sixad_deb_$ARCH/usr/lib/sixad/libbluetooth.so.3
}

ARCH="amd64"; make_dirs; copy_files; cp DEBIAN.split/postinst.outro ./sixad_deb_$ARCH/DEBIAN/postinst; mkdir ./sixad_deb_$ARCH/usr/lib64
ARCH="i386"; make_dirs; copy_files; cp DEBIAN.split/postinst.outro ./sixad_deb_$ARCH/DEBIAN/postinst; mkdir ./sixad_deb_$ARCH/usr/lib32
ARCH="powerpc"; make_dirs; copy_files; copy_lib; cp DEBIAN.split/postinst.powerpc ./sixad_deb_$ARCH/DEBIAN/postinst; mkdir ./sixad_deb_$ARCH/usr/lib32

#Special dirs for compat with RPM distros
cp compat/libbluetooth.so.2_amd64 ./sixad_deb_amd64/usr/lib64/libbluetooth.so.2
cp compat/libbluetooth.so.2_i386 ./sixad_deb_i386/usr/lib32/libbluetooth.so.2
cp compat/libbluetooth.so.2_powerpc ./sixad_deb_powerpc/usr/lib32/libbluetooth.so.2

#Create the debs
dpkg -b ./sixad_deb_amd64
dpkg -b ./sixad_deb_i386
dpkg -b ./sixad_deb_powerpc

#Check if file was created sucessfully, then prints info accordingly (also deletes old files)
if [ -f ./sixad_deb_amd64.deb ]; then
  {
  if [ -f ./sixad_deb_i386.deb ]; then
    {
      if [ -f ./sixad_deb_powerpc.deb ]; then
	rm -r ./sixad_deb_amd64
	rm -r ./sixad_deb_i386
	rm -r ./sixad_deb_powerpc
	mv sixad_deb_amd64.deb Debs/sixad_"$VERSION"_amd64.deb
	mv sixad_deb_i386.deb Debs/sixad_"$VERSION"_i386.deb
	mv sixad_deb_powerpc.deb Debs/sixad_"$VERSION"_powerpc.deb
	echo "Done!"
      else
	rm -r ./sixad_deb_amd64
	rm -r ./sixad_deb_i386
	rm -r ./sixad_deb_powerpc
	echo "Error building the powerpc package"
      fi
      }
  else
    rm -r ./sixad_deb_amd64
    rm -r ./sixad_deb_i386
    rm -r ./sixad_deb_powerpc
    echo "Error building the i386 package"
  fi
  }
else
  rm -r ./sixad_deb_amd64
  rm -r ./sixad_deb_i386
  rm -r ./sixad_deb_powerpc
  echo "Error building the amd64 package"
fi
