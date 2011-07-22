#ifndef QTSIXA_SIXAXIS_H
#define QTSIXA_SIXAXIS_H

#include "ui_qtsixa_sixaxis.h"

class  QtSixA_Configure_Sixaxis : public QDialog
{
    Q_OBJECT

public:
     QtSixA_Configure_Sixaxis(QDialog *parent = 0);
     const char *hidd_number_1;
     const char *hidd_number_2;
     const char *hidd_number_3;
     const char *hidd_number_4;
     const char *hidd_number_5;
     const char *hidd_number_6;
     const char *hidd_number_7;
     const char *hidd_number_8;
     int i_saw_the_warning;
     int applied2profile;
     int applied2override;
     int applied2sixad;
     int applied2boot;
     int applied2lr3;
     int applied2uinput;
     int Clicked;
     int nOfDevices;

 private slots:
     void func_NewProfile();
     void func_AddProfile();


private:
    Ui::Configure ui_sixaxis;

};

#endif // QTSIXA_SIXAXIS_H
