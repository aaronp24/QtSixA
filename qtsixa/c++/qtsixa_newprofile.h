#ifndef QTSIXA_NEWPROFILE_H
#define QTSIXA_NEWPROFILE_H

#include "ui_qtsixa_newprofile.h"

class  QtSixA_NewProfile : public QDialog
{
    Q_OBJECT

public:
     QtSixA_NewProfile(QDialog *parent = 0);

private:
    Ui::NewProfile ui_newprofile;

};

#endif // QTSIXA_NEWPROFILE_H
