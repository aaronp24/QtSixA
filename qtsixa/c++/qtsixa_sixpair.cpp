#include <QtGui>

#include "qtsixa_sixpair.h"
#include "main.h"

QtSixA_Sixpair::QtSixA_Sixpair(QDialog *parent)
    : QDialog(parent)
{
    ui_sixpair.setupUi(this);

    connect(ui_sixpair.b_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_sixpair.b_back, SIGNAL(clicked()), this, SLOT(func_GoToPage1()));
    connect(ui_sixpair.b_next, SIGNAL(clicked()), this, SLOT(func_GoToPage2()));
    connect(ui_sixpair.b_finish, SIGNAL(clicked()), this, SLOT(close()));

    ui_sixpair.textEdit->setReadOnly(true);
    CurrentPage = "0";
    devName = "Sixaxis";

    QtSixA_Sixpair::func_GoToPage1();

}

void QtSixA_Sixpair::func_GoToPage1()
{
    CurrentPage = "1";
    ui_sixpair.label->setText(tr(""
        "<font size=4 ><b>Getting ready for Sixpair setup...</b></font>"
        "<br><br>Before continue please make sure that your bluetooth<br>"
        "stick/device/pen is connected to the PC and that the Sixaxis/Keypad<br>"
        "is connected to the PC\'s USB<br><br>"
        "<i>Note: only one Sixaxis/Keypad per setup is supported</i>"));
    ui_sixpair.b_cancel->setEnabled(1);
    ui_sixpair.b_back->setEnabled(0);
    ui_sixpair.b_next->setEnabled(1);
    ui_sixpair.b_finish->setEnabled(0);
    ui_sixpair.textEdit->setVisible(0);
    ui_sixpair.groupDevice->setVisible(1);
}

void QtSixA_Sixpair::func_GoToPage2()
{
    CurrentPage = "2";
    ui_sixpair.b_cancel->setEnabled(0);
    ui_sixpair.b_back->setEnabled(0);
    ui_sixpair.b_next->setEnabled(0);
    ui_sixpair.b_finish->setEnabled(0);
    ui_sixpair.textEdit->setVisible(1);
    ui_sixpair.groupDevice->setVisible(0);
    if (ui_sixpair.radio_dev_sixaxis->isChecked())
    {
        if (look4Root())
        {
            sixpair_report = tr("fake pairing here");
            //sixpair_report = commands.getoutput(ROOT+" /usr/sbin/sixpair");
        } else {
            sixpair_report = tr("Not enough rights");
            devName = "Sixaxis";
        }
    } else {
        if (look4Root())
        {
            sixpair_report = tr("fake pairing here");
            //sixpair_report = commands.getoutput(ROOT+" /usr/sbin/sixpair-kbd");
        } else {
            sixpair_report = tr("Not enough rights");
            devName = "Keypad";
        }
    }

    ui_sixpair.textEdit->setText(sixpair_report);

    if (sixpair_report == tr("Not enough rights"))
      {
          ui_sixpair.label->setText(tr("Sixpair needs root/admin privileges to run\n \nPlease go back or cancel."));
          ui_sixpair.b_cancel->setEnabled(1);
          ui_sixpair.b_back->setEnabled(1);
          ui_sixpair.b_next->setEnabled(0);
          ui_sixpair.b_finish->setEnabled(0);
      } else if (0 == 1) {
    //} else if ("found on USB busses" in sixpair_report) {
          ui_sixpair.label->setText(tr(""
                "Sixpair reports that no %1 was found.\n"
                "It seems like you forgot something...\n \n"
                "Please go back or cancel.\n \n \n"
                "The sixpair report:").arg(devName));
          ui_sixpair.b_cancel->setEnabled(1);
          ui_sixpair.b_back->setEnabled(1);
          ui_sixpair.b_next->setEnabled(0);
          ui_sixpair.b_finish->setEnabled(0);
      } else {
          ui_sixpair.label->setText(tr(""
                "You\'re bluetooth stick/pen/device should now be \n"
                "paired with the %1.\n \n"
                "Below you can see the sixpair report:").arg(devName));
          ui_sixpair.b_cancel->setEnabled(0);
          ui_sixpair.b_back->setEnabled(1);
          ui_sixpair.b_next->setEnabled(0);
          ui_sixpair.b_finish->setEnabled(1);
    }
}
