#include <QApplication>
#include <stdio.h>

#include "qtsixa_main.h"

int main(int argc, char *argv[])
{
    printf("Main Qt version: %s\n", QT_VERSION_STR);

    QApplication app(argc, argv);
    QtSixA_Main qtsixa;
    qtsixa.show();
    return app.exec();
}
