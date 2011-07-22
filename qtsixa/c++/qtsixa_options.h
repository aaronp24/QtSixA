#ifndef QTSIXA_OPTIONS_H
#define QTSIXA_OPTIONS_H

#include "ui_qtsixa_options.h"

class  QtSixA_Options : public QDialog
{
    Q_OBJECT

public:
     QtSixA_Options(QDialog *parent = 0);

private:
    Ui::Options ui_options;

};

#endif // QTSIXA_OPTIONS_H
