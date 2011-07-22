#ifndef QTSIXA_SIXPAIR_H
#define QTSIXA_SIXPAIR_H

#include "ui_qtsixa_sixpair.h"

class  QtSixA_Sixpair : public QDialog
{
    Q_OBJECT

public:
     QtSixA_Sixpair(QDialog *parent = 0);
     const char *CurrentPage;
     const char *devName;
     QString sixpair_report;

 private slots:
     void func_GoToPage1();
     void func_GoToPage2();

private:
    Ui::Sixpair ui_sixpair;

};

#endif // QTSIXA_SIXPAIR_H
