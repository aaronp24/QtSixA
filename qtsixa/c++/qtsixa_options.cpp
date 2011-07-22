#include <QtGui>

#include "qtsixa_options.h"

QtSixA_Options::QtSixA_Options(QDialog *parent)
    : QDialog(parent)
{
    ui_options.setupUi(this);

//#RE-Read preferences file
//config_file = commands.getoutput('cat "$HOME/.qtsixa"').split()
//config_enable_systray = config_file[1]
//config_start_minimized = config_file[3]
//config_close_to_tray = config_file[5]
//config_show_warnings = config_file[7]
//
//self.i_saw_the_warning = 0
//
//#Notification stuff
//if (commands.getoutput("ls -a /tmp | grep sixa-notify") != ""):
//    self.box_notify.setChecked(1)
//    self.box_notify_start.setEnabled(1)
//    self.alreadyStartedNotify = 1
//else:
//    self.alreadyStartedNotify = 0
//if (commands.getoutput("ls $HOME/.config/autostart/ | grep sixa-notify") != ""):
//    self.box_notify.setChecked(1)
//    self.box_notify_start.setEnabled(1)
//    self.box_notify_start.setChecked(1)
//
//if (commands.getoutput("ls $HOME/.config/autostart/ | grep qtsixa.desktop") != ""):
//    self.box_startup.setChecked(1)
//
//if config_enable_systray == "yes":  self.box_systray.setChecked(1)
//if config_enable_systray == "yes" and config_start_minimized == "yes":  self.box_min.setChecked(1)
//if config_enable_systray == "yes" and config_close_to_tray == "yes":  self.box_close.setChecked(1)
//if config_show_warnings == "yes":  self.box_warn.setChecked(1)
//
//if self.box_systray.isChecked():
//  self.box_min.setEnabled(1)
//  self.box_close.setEnabled(1)
//else:
//  self.box_min.setChecked(0)
//  self.box_min.setEnabled(0)
//  self.box_close.setEnabled(0)
//  self.box_close.setChecked(0)
//
//self.connect(self.b_apply, QtCore.SIGNAL('clicked()'), self.func_Apply)
//self.connect(self.b_ok, QtCore.SIGNAL('clicked()'), self.func_OK)
//self.connect(self.box_startup, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
//self.connect(self.box_min, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
//self.connect(self.box_warn, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
//self.connect(self.box_close, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
//self.connect(self.box_systray, QtCore.SIGNAL('clicked()'), self.func_Apply_Enable)
//self.connect(self.box_systray, QtCore.SIGNAL('clicked()'), self.func_Systray)
//
//self.connect(self.box_notify, QtCore.SIGNAL("clicked()"), self.func_NotifyBox)
//self.connect(self.box_notify_start, QtCore.SIGNAL("clicked()"), self.func_Apply_Enable)
//
}

//
//def func_Apply_Enable(self):
//      self.b_apply.setEnabled(1)
//      self.b_cancel.setEnabled(1)
//
//def func_NotifyBox(self):
//    if self.box_notify.isChecked():
//        self.box_notify_start.setEnabled(1)
//    else:
//        self.box_notify_start.setEnabled(0)
//        self.box_notify_start.setChecked(0)
//    self.func_Apply_Enable()
//
//def func_Systray(self):
//    if self.box_systray.isChecked():
//      self.box_min.setEnabled(1)
//      self.box_close.setEnabled(1)
//    else:
//      self.box_min.setChecked(0)
//      self.box_min.setEnabled(0)
//      self.box_close.setEnabled(0)
//      self.box_close.setChecked(0)
//
//def func_OK(self):
//    if self.b_apply.isEnabled(): self.func_Apply()
//    self.close()
//
//def func_Apply(self):
//
//    if self.box_systray.isChecked(): self.conf_systray = "yes"
//    else: self.conf_systray = "no"
//    if self.box_min.isChecked(): self.conf_min = "yes"
//    else: self.conf_min = "no"
//    if self.box_close.isChecked(): self.conf_close = "yes"
//    else: self.conf_close = "no"
//    if self.box_warn.isChecked(): self.conf_warn = "yes"
//    else: self.conf_warn = "no"
//
//    self.finalFile = (""
//    "enable_systray	"+self.conf_systray+"\n"
//    "start_minimized	"+self.conf_min+"\n"
//    "close_to_tray	"+self.conf_close+"\n"
//    "show_warnings	"+self.conf_warn+"\n"
//    "")
//
//    if self.box_notify.isChecked():
//        if (self.alreadyStartedNotify == 0):
//            os.system("rm -rf "+"/tmp/.sixa-notify")
//            os.system("sixa-notify &")
//        if self.box_notify_start.isChecked():
//            os.system("cp /usr/share/qtsixa/sixa-notify.desktop $HOME/.config/autostart/sixa-notify.desktop")
//        else:
//            os.system("rm -rf $HOME/.config/autostart/sixa-notify.desktop")
//        self.alreadyStartedNotify = 1
//    else:
//        os.system("rm -rf /tmp/.sixa-notify")
//        os.system("rm -rf $HOME/.config/autostart/sixa-notify.desktop")
//        self.alreadyStartedNotify = 0
//
//    if self.box_startup.isChecked(): os.system("cp /usr/share/applications/qtsixa.desktop $HOME/.config/autostart/qtsixa.desktop")
//    else: os.system("rm -rf $HOME/.config/autostart/qtsixa.desktop")
//
//    saveFile = QtCore.QFile(commands.getoutput("ls $HOME/.qtsixa").split()[0])
//
//    if not saveFile.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
//      QtGui.QMessageBox.warning(self, self.tr("QtSixA - Error!"), self.tr("Cannot write to configuration file.\nPlease check if your media is not read-only, enough space left on disk and permissions."))
//    else:
//      os.system("echo '"+str(self.finalFile)+"' > $HOME/.qtsixa")
//      if self.i_saw_the_warning == 0:
//        QtGui.QMessageBox.information(self, self.tr("QtSixA - Done"), self.tr(""
//        "Don't forget you'll have to restart QtSixA to apply this changes."))
//
//    self.i_saw_the_warning = 1
//    self.b_cancel.setEnabled(0)
//    self.b_apply.setEnabled(0)
