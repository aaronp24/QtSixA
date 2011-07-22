#!/bin/bash
#Small script to automatically create a deb file for qtsixa
VERSION="1.1.1-falktx1"

#Make needed directories
mkdir ./qtsixa_deb
mkdir ./qtsixa_deb/DEBIAN
mkdir ./qtsixa_deb/usr
mkdir ./qtsixa_deb/usr/bin
mkdir ./qtsixa_deb/usr/share
mkdir ./qtsixa_deb/usr/share/applications
mkdir ./qtsixa_deb/usr/share/doc/
mkdir ./qtsixa_deb/usr/share/doc/qtsixa
mkdir ./qtsixa_deb/usr/share/doc/qtsixa/manual
#mkdir ./qtsixa_deb/usr/share/menu
mkdir ./qtsixa_deb/usr/share/pixmaps
mkdir ./qtsixa_deb/usr/share/qtsixa
mkdir ./qtsixa_deb/usr/share/qtsixa/game-profiles
mkdir ./qtsixa_deb/usr/share/qtsixa/gui
mkdir ./qtsixa_deb/usr/share/qtsixa/icons
mkdir ./qtsixa_deb/usr/share/qtsixa/pics
mkdir ./qtsixa_deb/usr/share/qtsixa/sixaxis-profiles

#Copy folders
cp DEBIAN/* ./qtsixa_deb/DEBIAN/
cp ../../qtsixa-*/qtsixa/doc/* ./qtsixa_deb/usr/share/doc/qtsixa/
cp ../../qtsixa-*/qtsixa/manual/* ./qtsixa_deb/usr/share/doc/qtsixa/manual/
cp ../../qtsixa-*/qtsixa/game-profiles/* ./qtsixa_deb/usr/share/qtsixa/game-profiles/
cp ../../qtsixa-*/qtsixa/gui/* ./qtsixa_deb/usr/share/qtsixa/gui/
cp ../../qtsixa-*/qtsixa/pics/* ./qtsixa_deb/usr/share/qtsixa/pics/
cp ../../qtsixa-*/qtsixa/icons/* ./qtsixa_deb/usr/share/qtsixa/icons/
cp ../../qtsixa-*/qtsixa/sixaxis-profiles/* ./qtsixa_deb/usr/share/qtsixa/sixaxis-profiles/

#Copy individual files
cp ../../qtsixa-*/qtsixa/sixa ./qtsixa_deb/usr/bin/
cp ../../qtsixa-*/qtsixa/sixa-lq ./qtsixa_deb/usr/bin/
cp ../../qtsixa-*/qtsixa/sixa-notify ./qtsixa_deb/usr/bin/
cp ../../qtsixa-*/qtsixa/qtsixa ./qtsixa_deb/usr/bin/
cp ../../qtsixa-*/qtsixa/qtsixa.conf ./qtsixa_deb/usr/share/qtsixa/
cp ../../qtsixa-*/qtsixa/qtsixa.desktop ./qtsixa_deb/usr/share/applications/
# cp ../../qtsixa-*/qtsixa/qtsixa.menu ./qtsixa_deb/usr/share/menu/qtsixa
cp ../../qtsixa-*/qtsixa/qtsixa.xpm ./qtsixa_deb/usr/share/pixmaps/
cp ../../qtsixa-*/qtsixa/*.bu ./qtsixa_deb/usr/share/qtsixa/
cp ../../qtsixa-*/qtsixa/features.html ./qtsixa_deb/usr/share/qtsixa/
cp ../../qtsixa-*/qtsixa/sixa-notify.desktop ./qtsixa_deb/usr/share/qtsixa/

sed "s/---VERSION---/$VERSION/" -i ./qtsixa_deb/DEBIAN/control

#Create the deb
dpkg -b ./qtsixa_deb

#Check if file was created sucessfully, then prints info accordingly (also deletes old files)
if [ -f ./qtsixa_deb.deb ]; then
rm -r ./qtsixa_deb
mv qtsixa_deb.deb qtsixa_"$VERSION"_all.zip.deb
echo "Done!"
else
rm -r ./qtsixa_deb
echo "Error building the package"
fi
