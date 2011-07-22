#include <QtGui>

#include "qtsixa_addprofile.h"

QtSixA_AddProfile::QtSixA_AddProfile(QDialog *parent)
    : QDialog(parent)
{
    ui_addprofile.setupUi(this);

    connect(ui_addprofile.b_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_addprofile.b_loc, SIGNAL(clicked()), this, SLOT(func_Add()));
    connect(ui_addprofile.b_png, SIGNAL(clicked()), this, SLOT(func_PNG_file()));
    connect(ui_addprofile.b_add, SIGNAL(clicked()), this, SLOT(func_Add()));
}


void QtSixA_AddProfile::func_Add()
{
    return;
    //    if (self.line_loc.displayText() == ""):
    //        self.func_Warning()
    //    elif (self.line_png.displayText() == ""):
    //        self.func_Warning()
    //    elif (self.line_short.displayText() == ""):
    //        self.func_Warning()
    //    elif (self.line_full.displayText() == ""):
    //        self.func_Warning()
    //    else:
    //        if look4Root():
    //            os.system(ROOT+" cp "+str(self.location)+" "+"/usr/share/qtsixa/sixaxis-profiles/sixa_"+str(self.line_short.displayText())+".fdi")
    //            os.system(ROOT+" cp "+str(self.png_file)+" "+"/usr/share/qtsixa/pics/sixa_"+str(self.line_short.displayText())+".png")
    //            self.profileFileW = open(("/tmp/qtsixa.profile"), "w")
    //            self.profileFileW.write(";\n"+str(self.line_short.displayText())+";\n"+str(self.line_full.displayText()))
    //            self.profileFileW.close()
    //            os.system("cp /usr/share/qtsixa/profiles.list /tmp/qtsixa.profile.mod")
    //            os.system("cat /tmp/qtsixa.profile >> /tmp/qtsixa.profile.mod")
    //            os.system(ROOT+" cp /tmp/qtsixa.profile.mod /usr/share/qtsixa/profiles.list")
    //            QtGui.QMessageBox.information(self, self.tr("QtSixA - Done!"), self.tr("Your custom profile \"%1\" has been added.\nYou will need to restart QtSixA to see changes.").arg(self.line_full.displayText()))
    //            self.close()
}

void QtSixA_AddProfile::func_Location()
{
    return;
    //    self.location = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open FDI File"), "", "QtSixA FDI Files (*fdi *FDI *Fdi)")
    //    self.line_loc.setText(str(self.location))
}

void QtSixA_AddProfile::func_PNG_file()
{
    return;
    //    self.png_file = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open PNG File"), "", "PNG Image Files (*png *PNG *Png *PnG)")
    //    self.line_png.setText(str(self.png_file))
}

void QtSixA_AddProfile::func_Warning()
{
    return;
    //    QtGui.QMessageBox.warning(self, self.tr("QtSixA - Incomplete!"), self.tr("Something is missing...\n(Empty slot is not possible!)"))

}
