#include <QtGui>
#include <QTimer>

#include "qtsixa_main.h"
#include "qtsixa_about.h"
#include "qtsixa_keylist.h"
#include "qtsixa_options.h"
#include "qtsixa_reference.h"
#include "qtsixa_sixaxis.h"
#include "qtsixa_sixpair.h"

QtSixA_Main::QtSixA_Main(QMainWindow *parent)
    : QMainWindow(parent)
{
    ui_main.setupUi(this);

    QTimer *autoListRefresh = new QTimer(this);
//    QTimer *autoLQRefresh = new QTimer(this);

    connect(ui_main.b_disconnect, SIGNAL(clicked()), this, SLOT(func_Disconnect()));
    connect(ui_main.b_battery, SIGNAL(clicked()), this, SLOT(func_Battery()));
    connect(ui_main.actionDiscAllSixaxis, SIGNAL(triggered()), this, SLOT(func_DiscAllSixaxis()));
    connect(ui_main.actionDiscEver, SIGNAL(triggered()), this, SLOT(func_DiscEverything()));
    connect(ui_main.actionDiscOne, SIGNAL(triggered()), this, SLOT(func_DiscSelected())); //1 FIXME
    connect(ui_main.actionDiscTwo, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscThree, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscFour, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscFive, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscSix, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscSeven, SIGNAL(triggered()), this, SLOT(func_DiscSelected()));
    connect(ui_main.actionDiscEight, SIGNAL(triggered()), this, SLOT(func_DiscSelected())); //8 FIXME
    connect(ui_main.actionBT_Start, SIGNAL(triggered()), this, SLOT(func_BT_Start()));
    connect(ui_main.actionBT_Stop, SIGNAL(triggered()), this, SLOT(func_BT_Stop()));
    connect(ui_main.actionSixpair, SIGNAL(triggered()), this, SLOT(func_Sixpair()));
    connect(ui_main.actionForce, SIGNAL(triggered()), this, SLOT(func_Force()));
    connect(ui_main.actionStop, SIGNAL(triggered()), this, SLOT(func_Stop()));
    connect(ui_main.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui_main.actionConfSixaxis, SIGNAL(triggered()), this, SLOT(func_ConfSixaxis()));
    connect(ui_main.actionConfQtSixA, SIGNAL(triggered()), this, SLOT(func_ConfQtSixA()));
    connect(ui_main.actionClearBT, SIGNAL(triggered()), this, SLOT(func_ClearBT()));
    connect(ui_main.actionRestoreDef, SIGNAL(triggered()), this, SLOT(func_RestoreDef()));
    connect(ui_main.actionSixaxisResPro, SIGNAL(triggered()), this, SLOT(func_RestoreProfiles()));
    connect(ui_main.actionManual, SIGNAL(triggered()), this, SLOT(func_Manual()));
    connect(ui_main.actionSourceForge, SIGNAL(triggered()), this, SLOT(func_SourceForge()));
    connect(ui_main.actionUbuntu, SIGNAL(triggered()), this, SLOT(func_UbuntuForums()));
    connect(ui_main.actionDonate, SIGNAL(triggered()), this, SLOT(func_Donate()));
    connect(ui_main.actionReport_Bug, SIGNAL(triggered()), this, SLOT(func_Report_Bug()));
    connect(ui_main.actionIdea, SIGNAL(triggered()), this, SLOT(func_Idea()));
    connect(ui_main.actionAskSome, SIGNAL(triggered()), this, SLOT(func_Questions()));
    connect(ui_main.actionReference, SIGNAL(triggered()), this, SLOT(func_Reference()));
    connect(ui_main.actionList_of_Features, SIGNAL(triggered()), this, SLOT(func_Features()));
    connect(ui_main.actionAbout_QtSixA, SIGNAL(triggered()), this, SLOT(func_About_QtSixA()));
    connect(ui_main.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui_main.b_game_help, SIGNAL(clicked()), this, SLOT(func_HelpGame()));
    connect(ui_main.b_game_apply, SIGNAL(clicked()), this, SLOT(func_ApplyGame()));
    connect(ui_main.b_apply_signal, SIGNAL(clicked()), this, SLOT(func_Apply_signal()));
    connect(ui_main.b_apply_hidraw, SIGNAL(clicked()), this, SLOT(func_Apply_hidraw()));
    connect(ui_main.b_refresh_signal, SIGNAL(clicked()), this, SLOT(func_Refresh_signal()));
    connect(ui_main.b_refresh_hidraw, SIGNAL(clicked()), this, SLOT(func_Refresh_hidraw()));
    connect(ui_main.b_reset_signal, SIGNAL(clicked()), this, SLOT(func_Reset_signal()));
    connect(ui_main.b_tips_signal, SIGNAL(clicked()), this, SLOT(func_Tips_signal()));
    connect(ui_main.b_stop_sixadraw, SIGNAL(clicked()), this, SLOT(func_Kill_sixadraw()));

    connect(autoListRefresh, SIGNAL(timeout()), this, SLOT(func_UpdateListOfDevices()));
//    connect(autoLQRefresh, SIGNAL(timeout()), this, SLOT(func_UpdateDeviceLQ()));
    connect(ui_main.listOfDevices, SIGNAL(currentRowChanged(int)), this, SLOT(func_UpdateDeviceStats()));
    connect(ui_main.listOfGames, SIGNAL(currentIndexChanged(int)), this, SLOT(func_UpdateGames()));
    connect(ui_main.listOfGames, SIGNAL(currentIndexChanged(int)), this, SLOT(func_Game_bOff()));

    connect(ui_main.radio_etracer_axis, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_etracer_accel, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_etracer_full, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_stk_digital, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_stk_axis, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_stk_accel, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_stk_full, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_neverball_axis, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_neverball_accel, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_game_epsxe_1, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_game_epsxe_axis, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));
    connect(ui_main.radio_game_epsxe_drv, SIGNAL(clicked()), this, SLOT(func_Game_bOn()));

    hidd_number_1 = "";
    hidd_number_2 = "";
    hidd_number_3 = "";
    hidd_number_4 = "";
    hidd_number_5 = "";
    hidd_number_6 = "";
    hidd_number_7 = "";
    hidd_number_8 = "";
    usb_number_1 = "";
    usb_number_2 = "";
    usb_number_3 = "";
    usb_number_4 = "";
    trayTooltip = "Aaah!!";
    SixaxisProfile = "";
    trayIsActive = 0;
//    self.wEP.setVisible(1); //ePSXe
    ui_main.wET->setVisible(0); //Extreme Tux Racer
    ui_main.wNE->setVisible(0); //Neverball
    ui_main.wSTK->setVisible(0); //SuperTuxKart
    ui_main.b_game_apply->setEnabled(0);

    func_UpdateListOfDevices();
    func_UpdateDeviceStats();
    autoListRefresh->start(2000);
    func_What();

    //    if (config_enable_systray == "yes")
    //    {
    //        createTrayIcon();
    //        trayIsActive = 1;
    //    } else {
    //        trayIsActive = 0;
    //    }

    
}

void QtSixA_Main::func_What()
{
//    self.SixaxisProfileCheck = commands.getoutput("ls /etc/hal/fdi/policy | grep sixa | awk 'sub(\"sixa_\",\"\")' | awk 'sub(\".fdi\",\"\")'")
//    if (self.SixaxisProfileCheck == ""):
        ui_main.label_SixaxisProfile->setText(tr("You're not using a Sixaxis profile"));
//    else:
//        self.SixaxisProfile = self.SixaxisProfileCheck.split()[0]
//        if (self.SixaxisProfile in listOfSixaxisProfiles):
//            x = listOfSixaxisProfiles.index(self.SixaxisProfile)
//            self.SixaxisProfile = listOfSixaxisProfiles[x+1]
//            self.label_SixaxisProfile.setText(self.tr("Current Sixaxis profile is \"%1\"").arg(self.SixaxisProfile))
//        else: self.label_SixaxisProfile.setText(self.tr("You're not using a Sixaxis profile"))
}

void QtSixA_Main::func_UpdateListOfDevices()
{
//    self.Check4BluetoothDevices = commands.getoutput("hcitool con")
//    self.Check4USBDevices = commands.getoutput("lsusb")
//
//    if not "ACL" in self.Check4BluetoothDevices:
        ui_main.listOfDevices->item(1)->setHidden(1);
        ui_main.listOfDevices->item(2)->setHidden(1);
        ui_main.listOfDevices->item(3)->setHidden(1);
        ui_main.listOfDevices->item(4)->setHidden(1);
        ui_main.listOfDevices->item(5)->setHidden(1);
        ui_main.listOfDevices->item(6)->setHidden(1);
        ui_main.listOfDevices->item(7)->setHidden(1);
        ui_main.listOfDevices->item(8)->setHidden(1);
        ui_main.actionDiscOne->setVisible(0);
        ui_main.actionDiscTwo->setVisible(0);
        ui_main.actionDiscThree->setVisible(0);
        ui_main.actionDiscFour->setVisible(0);
        ui_main.actionDiscFive->setVisible(0);
        ui_main.actionDiscSix->setVisible(0);
        ui_main.actionDiscSeven->setVisible(0);
        ui_main.actionDiscEight->setVisible(0);
        ui_main.actionDiscNull->setVisible(1);
        hidd_number_1 = hidd_number_2 = hidd_number_3 = hidd_number_4 = hidd_number_5 = hidd_number_6 = hidd_number_7 = hidd_number_8 = "";
//        if (not "054c:03a0" in self.Check4USBDevices) and (not "054c:0306" in self.Check4USBDevices) and (not "0079:0006" in self.Check4USBDevices) and (not "054c:0268" in self.Check4USBDevices):
            ui_main.listOfDevices->item(9)->setHidden(1);
            ui_main.listOfDevices->item(10)->setHidden(1);
            ui_main.listOfDevices->item(11)->setHidden(1);
            ui_main.listOfDevices->item(12)->setHidden(1);
            ui_main.listOfDevices->item(0)->setHidden(0);
            ui_main.listOfDevices->setSelectionMode(QAbstractItemView::NoSelection); //0
            usb_number_1 = usb_number_2 = usb_number_3 = usb_number_4 = "";
//    else:
//        self.func_UpdateBluetoothNames()
//        self.listOfDevices.setSelectionMode(1)
//        self.listOfDevices.item(0).setHidden(1)
//        self.actionDiscNull.setVisible(0)
//        if (self.hidd_number_1 == ""): self.listOfDevices.item(1).setHidden(1), self.actionDiscOne.setVisible(0)
//        else:
//            self.listOfDevices.item(1).setText(self.hidd_number_1)
//            self.listOfDevices.item(1).setHidden(0)
//            self.actionDiscOne.setVisible(1)
//            self.actionDiscOne.setText(self.hidd_number_1)
//        if (self.hidd_number_2 == ""): self.listOfDevices.item(2).setHidden(1), self.actionDiscTwo.setVisible(0)
//        else:
//            self.listOfDevices.item(2).setText(self.hidd_number_2)
//            self.listOfDevices.item(2).setHidden(0)
//            self.actionDiscTwo.setVisible(1)
//            self.actionDiscTwo.setText(self.hidd_number_2)
//        if (self.hidd_number_3 == ""): self.listOfDevices.item(3).setHidden(1), self.actionDiscThree.setVisible(0)
//        else:
//            self.listOfDevices.item(3).setText(self.hidd_number_3)
//            self.listOfDevices.item(3).setHidden(0)
//            self.actionDiscThree.setVisible(1)
//            self.actionDiscThree.setText(self.hidd_number_3)
//        if (self.hidd_number_4 == ""): self.listOfDevices.item(4).setHidden(1), self.actionDiscFour.setVisible(0)
//        else:
//            self.listOfDevices.item(4).setText(self.hidd_number_4)
//            self.listOfDevices.item(4).setHidden(0)
//            self.actionDiscFour.setVisible(1)
//            self.actionDiscFour.setText(self.hidd_number_4)
//        if (self.hidd_number_5 == ""): self.listOfDevices.item(5).setHidden(1), self.actionDiscFive.setVisible(0)
//        else:
//            self.listOfDevices.item(5).setText(self.hidd_number_5)
//            self.listOfDevices.item(5).setHidden(0)
//            self.actionDiscFive.setVisible(1)
//            self.actionDiscFive.setText(self.hidd_number_5)
//        if (self.hidd_number_6 == ""): self.listOfDevices.item(6).setHidden(1), self.actionDiscSix.setVisible(0)
//        else:
//            self.listOfDevices.item(6).setText(self.hidd_number_6)
//            self.listOfDevices.item(6).setHidden(0)
//            self.actionDiscSix.setVisible(1)
//            self.actionDiscSix.setText(self.hidd_number_6)
//        if (self.hidd_number_7 == ""): self.listOfDevices.item(7).setHidden(1), self.actionDiscSeven.setVisible(0)
//        else:
//            self.listOfDevices.item(7).setText(self.hidd_number_7)
//            self.listOfDevices.item(7).setHidden(0)
//            self.actionDiscSeven.setVisible(1)
//            self.actionDiscSeven.setText(self.hidd_number_7)
//        if (self.hidd_number_8 == ""): self.listOfDevices.item(8).setHidden(1), self.actionDiscEight.setVisible(0)
//        else:
//            self.listOfDevices.item(8).setText(self.hidd_number_8)
//            self.listOfDevices.item(8).setHidden(0)
//            self.actionDiscEight.setVisible(1)
//            self.actionDiscEight.setText(self.hidd_number_8)
//
//    if (not "054c:03a0" in self.Check4USBDevices) and (not "054c:0306" in self.Check4USBDevices) and (not "0079:0006" in self.Check4USBDevices) and (not "054c:0268" in self.Check4USBDevices):
//        self.listOfDevices.item(9).setHidden(1)
//        self.listOfDevices.item(10).setHidden(1)
//        self.listOfDevices.item(11).setHidden(1)
//        self.listOfDevices.item(12).setHidden(1)
//        self.usb_number_1 = self.usb_number_2 = self.usb_number_3 = self.usb_number_4 = ""
//    else:
//        self.func_UpdateUSBNames()
//        self.listOfDevices.item(0).setHidden(1)
//        self.listOfDevices.item(9).setHidden(0)
//        self.listOfDevices.item(10).setHidden(0)
//        self.listOfDevices.item(11).setHidden(0)
//        self.listOfDevices.item(12).setHidden(0)
//        self.listOfDevices.setSelectionMode(1)
//        if (self.usb_number_1 == ""): self.listOfDevices.item(9).setHidden(1)
//        else: self.listOfDevices.item(9).setText(self.usb_number_1)
//        if (self.usb_number_2 == ""): self.listOfDevices.item(10).setHidden(1)
//        else: self.listOfDevices.item(10).setText(self.usb_number_2)
//        if (self.usb_number_3 == ""): self.listOfDevices.item(11).setHidden(1)
//        else: self.listOfDevices.item(11).setText(self.usb_number_3)
//        if (self.usb_number_4 == ""): self.listOfDevices.item(12).setHidden(1)
//        else: self.listOfDevices.item(12).setText(self.usb_number_4)
//
//    self.func_UpdateTrayTooltip()

}

void QtSixA_Main::func_UpdateDeviceStats()
{
//    if self.listOfDevices.currentRow() <= 0: #No device
        ui_main.b_disconnect->setEnabled(0);
        ui_main.b_battery->setEnabled(0);
        ui_main.groupDevice->setEnabled(0);
        ui_main.groupBluetooth->setEnabled(0);
        ui_main.lineName->setText("");
        ui_main.lineType->setText("");
        ui_main.lineID->setText("");
        ui_main.lineMode->setText("");
        ui_main.lineAddress->setText("");
        ui_main.barLQ->setTextVisible(0);
        ui_main.barLQ->setValue(0);
        ui_main.barBattery->setTextVisible(0);
        ui_main.barBattery->setValue(0);
//        autoLQRefresh->stop();
//    elif self.listOfDevices.currentRow() >= 9: #USB selected
//        self.parsedID = str(self.listOfDevices.item(self.listOfDevices.currentRow()).text()).split(":")
//        if (self.parsedID[2] == "054c" and self.parsedID[3] == "0268"):
//            self.devName = "PLAYSTATION(R)3 Controller"
//            self.devType = "Joystick"
//        elif (self.parsedID[2] == "054c" and self.parsedID[3] == "03a0"):
//            self.devName = "Wireless Keypad"
//            self.devType = "Keypad"
//        elif (self.parsedID[2] == "054c" and self.parsedID[3] == "0306"):
//            self.devName = "PLAYSTATION(R)3 Remote"
//            self.devType = "Remote"
//        elif (self.parsedID[2] == "0079" and self.parsedID[3] == "0006"):
//            self.devName = "Generic USB Joystick"
//            self.devType = "Joystick"
//        else:
//            self.devName = self.tr("Unknown")
//            self.devType = self.tr("Unknown")
//        self.b_disconnect.setEnabled(0)
//        self.b_battery.setEnabled(0)
//        self.groupDevice.setEnabled(1)
//        self.groupBluetooth.setEnabled(0)
//        self.lineName.setText(self.devName)
//        self.lineType.setText(self.devType)
//        self.lineID.setText(self.parsedID[2]+":"+self.parsedID[3])
//        self.lineMode.setText("USB")
//        self.lineAddress.setText("")
//        self.barLQ.setTextVisible(0)
//        self.barLQ.setValue(0)
//        self.barBattery.setTextVisible(0)
//        self.barBattery.setValue(0)
//        self.autoLQRefresh.stop()
//    else: #Bluetooth
//        self.devName = commands.getoutput("hcitool name "+str(self.listOfDevices.item(self.listOfDevices.currentRow()).text()))
//        if (self.devName == "PLAYSTATION(R)3 Controller"):
//            self.devType = "Joystick"
//            self.devID = "054c:0268"
//        elif (self.devName == "Wireless Keypad"):
//            self.devType = "Keypad"
//            self.devID = "054c:03a0"
//        elif (self.devName == "PLAYSTATION(R)3 Remote"):
//            self.devType = "Remote"
//            self.devID = "054c:0306"
//        else:
//            self.devType = self.tr("Unknown")
//            self.devID = self.tr("Unknown")
//        self.b_disconnect.setEnabled(1)
//        if self.devType == "Joystick": self.b_battery.setEnabled(1)
//        else: self.b_battery.setEnabled(0)
//        self.groupDevice.setEnabled(1)
//        self.groupBluetooth.setEnabled(1)
//        self.lineName.setText(self.devName)
//        self.lineType.setText(self.devType)
//        self.lineID.setText(self.devID)
//        self.lineMode.setText("Bluetooth")
//        self.lineAddress.setText(self.listOfDevices.item(self.listOfDevices.currentRow()).text())
//        self.barLQ.setTextVisible(1)
//        self.devLQ = commands.getoutput("hcitool lq "+str(self.listOfDevices.item(self.listOfDevices.currentRow()).text())+" | awk '{printf$3}'")
//        if not ("o" in self.devLQ): self.barLQ.setValue(int(self.devLQ))
//        self.label_bat.setEnabled(0)
//        self.barBattery.setEnabled(0)
//        self.barBattery.setTextVisible(0)
//        self.barBattery.setValue(0)
//        self.autoLQRefresh.start(5000)

}

void QtSixA_Main::func_About_QtSixA()
{
    QtSixA_About_Window().exec();
}

void QtSixA_Main::func_Disconnect()
{
    return;
}

void QtSixA_Main::func_Battery()
{
    return;
}

void QtSixA_Main::func_DiscAllSixaxis()
{
    return;
}

void QtSixA_Main::func_DiscEverything()
{
    return;
}

void QtSixA_Main::func_DiscSelected() //FIXME
{
    return;
}

void QtSixA_Main::func_BT_Start()
{
    return;
}

void QtSixA_Main::func_BT_Stop()
{
    return;
}

void QtSixA_Main::func_Sixpair()
{
    QtSixA_Sixpair().exec();
}

void QtSixA_Main::func_Force()
{
    return;
}

void QtSixA_Main::func_Stop()
{
    return;
}

void QtSixA_Main::func_ConfSixaxis()
{
    QtSixA_Configure_Sixaxis().exec();
}

void QtSixA_Main::func_ConfQtSixA()
{
    QtSixA_Options().exec();
}

void QtSixA_Main::func_ClearBT()
{
    return;
}

void QtSixA_Main::func_RestoreDef()
{
    return;
}

void QtSixA_Main::func_RestoreProfiles()
{
    return;
}

void QtSixA_Main::func_Manual()
{
    return;
}

void QtSixA_Main::func_SourceForge()
{
    return;
}

void QtSixA_Main::func_UbuntuForums()
{
    return;
}

void QtSixA_Main::func_Donate()
{
    return;
}

void QtSixA_Main::func_Report_Bug()
{
    return;
}

void QtSixA_Main::func_Idea()
{
    return;
}

void QtSixA_Main::func_Questions()
{
    return;
}

void QtSixA_Main::func_Reference()
{
    QtSixA_Reference().exec();
}

void QtSixA_Main::func_Features()
{
    return;
}

void QtSixA_Main::func_UpdateGames()
{
    ui_main.wEP->setVisible(0);
    ui_main.wET->setVisible(0);
    ui_main.wNE->setVisible(0);
    ui_main.wSTK->setVisible(0);
    if (ui_main.listOfGames->currentText() == "ePSXe (Wine)") {
        ui_main.wEP->setVisible(1);
    } else if (ui_main.listOfGames->currentText() == "Extreme Tux Racer") {
        ui_main.wET->setVisible(1);
    } else if (ui_main.listOfGames->currentText() == "Neverball / Neverputt") {
        ui_main.wNE->setVisible(1);
    } else if (ui_main.listOfGames->currentText() == "Super Tux Kart") {
        ui_main.wSTK->setVisible(1);
    }
}

void QtSixA_Main::func_Game_bOff()
{
    return;
}

void QtSixA_Main::func_Game_bOn()
{
    return;
}

void QtSixA_Main::func_UpdateDeviceLQ()
{
    return;
}

void QtSixA_Main::func_HelpGame()
{
    QMessageBox::information(this, tr("QtSixA - Game Help"), tr(""
    "This is easy:<p>"
    "<b>1.</b> Select the game from the combo-box in the middle<br>"
    "<b>2.</b> Choose a configuration on the buttons below<br>"
    "<b>3.</b> Click the apply button on the bottom right<p>"
    "<i>Please be sure to run the game at least once before clicking \"Apply\",<br>"
    "or the profile may not be applied sucessfully</i>"));
}

void QtSixA_Main::func_ApplyGame()
{
    return;
}

void QtSixA_Main::func_Apply_signal()
{
    return;
}

void QtSixA_Main::func_Apply_hidraw()
{
    return;
}

void QtSixA_Main::func_Refresh_signal()
{
    return;
}

void QtSixA_Main::func_Refresh_hidraw()
{
    return;
}

void QtSixA_Main::func_Reset_signal()
{
    return;
}

void QtSixA_Main::func_Tips_signal()
{
    return;
}

void QtSixA_Main::func_Kill_sixadraw()
{
    return;
}
