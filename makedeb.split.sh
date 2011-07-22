#!/bin/bash
#Small script to automatically create the deb files for sixad
VERSION="0.9-falktx1"

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
  cp DEBIAN.split/postinst.all ./sixad_deb_$ARCH/DEBIAN/postinst
}

copy_32 () {
mkdir ./sixad_deb_$ARCH/usr/lib32
cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib32/libbluetooth.so.2
}

copy_64 () {
mkdir ./sixad_deb_$ARCH/usr/lib64
cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib64/libbluetooth.so.2
}

ARCH="amd64"; make_dirs; copy_files; copy_64
ARCH="i386"; make_dirs; copy_files; copy_32
ARCH="powerpc32"; make_dirs; copy_files; copy_32
ARCH="powerpc64"; make_dirs; copy_files; copy_64

#Create the debs
dpkg -b ./sixad_deb_amd64
dpkg -b ./sixad_deb_i386
dpkg -b ./sixad_deb_powerpc32
dpkg -b ./sixad_deb_powerpc64

#Check if file was created sucessfully, then prints info accordingly (also deletes old files)
if [ -f ./sixad_deb_amd64.deb ]; then
  {
  if [ -f ./sixad_deb_i386.deb ]; then
    {
      if [ -f ./sixad_deb_powerpc32.deb ]; then
      {
	if [ -f ./sixad_deb_powerpc64.deb ]; then
	{
	  rm -r ./sixad_deb_amd64
	  rm -r ./sixad_deb_i386
	  rm -r ./sixad_deb_powerpc32
	  rm -r ./sixad_deb_powerpc64
	  mv sixad_deb_amd64.deb Debs/sixad_"$VERSION"_amd64.deb
	  mv sixad_deb_i386.deb Debs/sixad_"$VERSION"_i386.deb
	  mv sixad_deb_powerpc32.deb Debs/sixad_"$VERSION"_powerpc32.deb
	  mv sixad_deb_powerpc64.deb Debs/sixad_"$VERSION"_powerpc64.deb
	  echo "Done!"
	}
	else
	{
	  rm -r ./sixad_deb_amd64
	  rm -r ./sixad_deb_i386
	  rm -r ./sixad_deb_powerpc32
	  rm -r ./sixad_deb_powerpc64
	  echo "Error building the powerpc64 package"
	}
	fi
	}
      else
      {
	rm -r ./sixad_deb_amd64
	rm -r ./sixad_deb_i386
	rm -r ./sixad_deb_powerpc32
	rm -r ./sixad_deb_powerpc64
	echo "Error building the powerpc32 package"
      }
      fi
      }
    else
    {
      rm -r ./sixad_deb_amd64
      rm -r ./sixad_deb_i386
      rm -r ./sixad_deb_powerpc32
      rm -r ./sixad_deb_powerpc64
      echo "Error building the i386 package"
    }
    fi
    }
  else
  {
    rm -r ./sixad_deb_amd64
    rm -r ./sixad_deb_i386
    rm -r ./sixad_deb_powerpc32
    rm -r ./sixad_deb_powerpc64
    echo "Error building the amd64 package"
  }
fi
