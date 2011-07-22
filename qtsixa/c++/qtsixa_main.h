#ifndef QTSIXA_MAIN_H
#define QTSIXA_MAIN_H

#include "ui_qtsixa_main.h"

class QtSixA_Main : public QMainWindow
{
    Q_OBJECT

public:
    QtSixA_Main(QMainWindow *parent = 0);
    char *hidd_number_1;
    char *hidd_number_2;
    char *hidd_number_3;
    char *hidd_number_4;
    char *hidd_number_5;
    char *hidd_number_6;
    char *hidd_number_7;
    char *hidd_number_8;
    char *usb_number_1;
    char *usb_number_2;
    char *usb_number_3;
    char *usb_number_4;
    char *trayTooltip;
    char *SixaxisProfile;
    int trayIsActive;

private slots:
    void func_About_QtSixA();
    void func_Disconnect();
    void func_Battery();
    void func_DiscAllSixaxis();
    void func_DiscEverything();
    void func_DiscSelected(); //FIXME
    void func_BT_Start();
    void func_BT_Stop();
    void func_Sixpair();
    void func_Force();
    void func_Stop();
    void func_ConfSixaxis();
    void func_ConfQtSixA();
    void func_ClearBT();
    void func_RestoreDef();
    void func_RestoreProfiles();
    void func_Manual();
    void func_SourceForge();
    void func_UbuntuForums();
    void func_Donate();
    void func_Report_Bug();
    void func_Idea();
    void func_Questions();
    void func_Reference();
    void func_Features();
    void func_HelpGame();
    void func_ApplyGame();
    void func_Apply_signal();
    void func_Apply_hidraw();
    void func_Refresh_signal();
    void func_Refresh_hidraw();
    void func_Reset_signal();
    void func_Tips_signal();
    void func_Kill_sixadraw();
    void func_UpdateListOfDevices();
    void func_UpdateDeviceLQ();
    void func_UpdateDeviceStats();
    void func_UpdateGames();
    void func_Game_bOff();
    void func_Game_bOn();
    void func_What();

private:
    Ui::MainWindow ui_main;

};

#endif // QTSIXA_MAIN_H
