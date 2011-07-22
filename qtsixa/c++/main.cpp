#include <QApplication>
#include <stdio.h>

#include "qtsixa_main.h"
#include "main.h"

const char *ROOT;

int look4Root()
{
//    if not "kdesudo" in ROOT and "kdesu" in ROOT: #Fix for openSUSE's kdesu not echoing to terminal (opens separate session for sudo)
//	return 1
//    elif "YES" in commands.getoutput(ROOT+" echo YES"):
//	return 1
//    else:
//        QtGui.QMessageBox.critical(QtSixA, QtGui.QApplication.translate("MainPYW", "QtSixA - Error"), QtGui.QApplication.translate("MainPYW", "Operation not permitted - Not enough rights"))
    return 0;
}

//def func_Check_BTs():
//    if (commands.getoutput("hcitool dev") == "Devices:"):
//	QtGui.QMessageBox.warning(QtSixA, QtGui.QApplication.translate("MainPYW", "QtSixA - Warning"), QtGui.QApplication.translate("MainPYW", "No bluetooth dongles detected.\nConnect over bluetooth will not be possible"))


int main(int argc, char *argv[])
{
    printf("Main Qt version: %s\n", QT_VERSION_STR);

//#Create configuration files if they don't exist
//os.system("if [ -d $HOME/.config/autostart ]; then true; else mkdir -p $HOME/.config; mkdir -p $HOME/.config/autostart; fi")
//os.system("if [ -f $HOME/.qtsixa ]; then true; else cp /usr/share/qtsixa/qtsixa.conf $HOME/.qtsixa; fi")
//
//#Read preferences file
//config_file = commands.getoutput('cat "$HOME/.qtsixa"').split()
//config_enable_systray = config_file[1]
//config_start_minimized = config_file[3]
//config_close_to_tray = config_file[5]
//config_show_warnings = config_file[7]
//
//#List Of Sixaxis profiles
//if os.path.exists('/usr/share/qtsixa/profiles.list'):
//    listOfSixaxisProfiles = open('/usr/share/qtsixa/profiles.list').read().split(";\n")
//else:
//    listOfSixaxisProfiles = "".split()
//    print "Faild to load list of Sixaxis profiles"
//
//#Use dbus for disconnect devices (no root required)
//bus = dbus.SystemBus()
//
//#Check for root tool

    ROOT = "sudo";
    //ROOT = commands.getoutput("sixa "+"get-root");
    printf("Will use '%s' for root actions\n", ROOT);
    //printf("Will use '%s' for root actions\n", ROOT.split()[0]);

    QApplication app(argc, argv);
    QtSixA_Main qtsixa;
    qtsixa.show();
    return app.exec();
}
