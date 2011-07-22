#ifndef QTSIXA_SIXAXIS_H
#define QTSIXA_SIXAXIS_H

#include "ui_qtsixa_sixaxis.h"

class  QtSixA_Configure_Sixaxis : public QDialog
{
    Q_OBJECT

public:
     QtSixA_Configure_Sixaxis(QDialog *parent = 0);

private:
    Ui::Configure ui_sixaxis;

};

#endif // QTSIXA_SIXAXIS_H
