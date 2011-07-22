#!/bin/bash
#Small script to automatically create the deb files for sixad
VERSION="1.0.3-0falktx1"

cd ../../qtsixa-*/sixad
make -f ../../qtsixa-XTE/sixad/Makefile.all

cd ../../qtsixa-XTE/sixad/

#Make needed directories
make_dirs () {
  mkdir ./sixad_deb_$ARCH
  mkdir ./sixad_deb_$ARCH/DEBIAN
  mkdir ./sixad_deb_$ARCH/etc
  mkdir ./sixad_deb_$ARCH/etc/default
  mkdir ./sixad_deb_$ARCH/etc/init.d
  mkdir ./sixad_deb_$ARCH/lib
  mkdir ./sixad_deb_$ARCH/lib/udev
  mkdir ./sixad_deb_$ARCH/lib/udev/rules.d
  mkdir ./sixad_deb_$ARCH/usr
  mkdir ./sixad_deb_$ARCH/usr/bin
  mkdir ./sixad_deb_$ARCH/usr/lib
  mkdir ./sixad_deb_$ARCH/usr/sbin
  mkdir ./sixad_deb_$ARCH/usr/share
  mkdir ./sixad_deb_$ARCH/usr/share/man
  mkdir ./sixad_deb_$ARCH/usr/share/man/man1
  mkdir ./sixad_deb_$ARCH/usr/share/hal
  mkdir ./sixad_deb_$ARCH/usr/share/hal/fdi
  mkdir ./sixad_deb_$ARCH/usr/share/hal/fdi/policy
  mkdir ./sixad_deb_$ARCH/usr/share/hal/fdi/policy/20thirdparty
  mkdir ./sixad_deb_$ARCH/usr/share/sixad
}

copy_files () {
  cp DEBIAN/conffiles ./sixad_deb_$ARCH/DEBIAN/
  cp DEBIAN/postrm ./sixad_deb_$ARCH/DEBIAN/
  cp DEBIAN/postinst ./sixad_deb_$ARCH/DEBIAN/
  cp DEBIAN/preinst ./sixad_deb_$ARCH/DEBIAN/
  cp DEBIAN/control ./sixad_deb_$ARCH/DEBIAN/

  cp ../../qtsixa-*/sixad/sixad ./sixad_deb_$ARCH/usr/bin/
  cp ../../qtsixa-*/sixad/default/sixad ./sixad_deb_$ARCH/etc/default/
  cp ../../qtsixa-*/sixad/init/sixad ./sixad_deb_$ARCH/etc/init.d/
  cp ../../qtsixa-*/sixad/98-sixad.rules ./sixad_deb_$ARCH/lib/udev/rules.d/
  cp ../../qtsixa-*/sixad/bins/hidraw-dump_$ARCH ./sixad_deb_$ARCH/usr/sbin/hidraw-dump
  cp ../../qtsixa-*/sixad/bins/sixpair_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixpair
  cp ../../qtsixa-*/sixad/bins/sixpair-kbd_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixpair-kbd
  cp ../../qtsixa-*/sixad/bins/sixad-raw_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-raw
  cp ../../qtsixa-*/sixad/bins/sixad-bin_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-bin
  cp ../../qtsixa-*/sixad/bins/sixad-uinput-sixaxis_$ARCH ./sixad_deb_$ARCH/usr/sbin/sixad-uinput-sixaxis
  cp ../../qtsixa-*/sixad/x11-sony-keypad.fdi ./sixad_deb_$ARCH/usr/share/hal/fdi/policy/20thirdparty
  cp ../../qtsixa-*/sixad/mans/* ./sixad_deb_$ARCH/usr/share/man/man1/
  gzip -9 ./sixad_deb_$ARCH/usr/share/man/man1/*

  cp compat/hcid_$ARCH ./sixad_deb_$ARCH/usr/sbin/hcid
  cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib/libbluetooth.so.2
}

copy_32 () {
mkdir ./sixad_deb_$ARCH/usr/lib32
cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib32/libbluetooth.so.2
}

copy_64 () {
mkdir ./sixad_deb_$ARCH/usr/lib64
cp compat/libbluetooth.so.2_$ARCH ./sixad_deb_$ARCH/usr/lib64/libbluetooth.so.2
}

make_arch() {
sed "s/---ARCH---/$ARCH/" -i ./sixad_deb_*/DEBIAN/control
}

ARCH="amd64"; make_dirs; copy_files; make_arch; copy_64
ARCH="i386"; make_dirs; copy_files; make_arch; copy_32
ARCH="powerpc32"; make_dirs; copy_files; make_arch; copy_32
ARCH="powerpc64"; make_dirs; copy_files; make_arch; copy_64

sed "s/---VERSION---/$VERSION/" -i ./sixad_deb_*/DEBIAN/control

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
