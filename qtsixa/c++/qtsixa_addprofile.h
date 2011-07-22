#ifndef QTSIXA_ADDPROFILE_H
#define QTSIXA_ADDPROFILE_H

#include "ui_qtsixa_addprofile_mod.h"

class  QtSixA_AddProfile : public QDialog
{
    Q_OBJECT

public:
     QtSixA_AddProfile(QDialog *parent = 0);

 private slots:
     void func_Add();
     void func_Location();
     void func_PNG_file();
     void func_Warning();

private:
    Ui::AddProfile ui_addprofile;

};

#endif // QTSIXA_ADDPROFILE_H
