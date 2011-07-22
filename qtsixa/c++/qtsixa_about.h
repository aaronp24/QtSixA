#ifndef QTSIXA_ABOUT_H
#define QTSIXA_ABOUT_H

#include "ui_qtsixa_about.h"

class  QtSixA_About_Window : public QDialog
{
    Q_OBJECT

public:
     QtSixA_About_Window(QDialog *parent = 0);

private:
    Ui::About ui_about;

};

#endif // QTSIXA_ABOUT_H
