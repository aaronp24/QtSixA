#ifndef QTSIXA_KEYLIST_H
#define QTSIXA_KEYLIST_H

#include "ui_qtsixa_keylist.h"

class  QtSixA_Keylist : public QDialog
{
    Q_OBJECT

public:
     QtSixA_Keylist(QDialog *parent = 0);

private:
    Ui::Keylist ui_keylist;

};

#endif // QTSIXA_KEYLIST_H
