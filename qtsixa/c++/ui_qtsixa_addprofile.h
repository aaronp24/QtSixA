/********************************************************************************
** Form generated from reading UI file 'qtsixa_addprofile.ui'
**
** Created: Sat Jan 16 22:57:33 2010
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSIXA_ADDPROFILE_H
#define UI_QTSIXA_ADDPROFILE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddProfile
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_loc;
    QLineEdit *line_loc;
    QToolButton *b_loc;
    QLabel *label_8;
    QSpacerItem *verticalSpacer_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_png;
    QLineEdit *line_png;
    QToolButton *b_png;
    QLabel *label_2;
    QSpacerItem *verticalSpacer_3;
    QFrame *line_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label_short;
    QLineEdit *line_short;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_6;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_full;
    QLineEdit *line_full;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_7;
    QSpacerItem *verticalSpacer_2;
    QFrame *line;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *b_cancel;
    QPushButton *b_add;

    void setupUi(QWidget *AddProfile)
    {
        if (AddProfile->objectName().isEmpty())
            AddProfile->setObjectName(QString::fromUtf8("AddProfile"));
        AddProfile->resize(604, 368);
        verticalLayout = new QVBoxLayout(AddProfile);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_loc = new QLabel(AddProfile);
        label_loc->setObjectName(QString::fromUtf8("label_loc"));

        horizontalLayout_4->addWidget(label_loc);

        line_loc = new QLineEdit(AddProfile);
        line_loc->setObjectName(QString::fromUtf8("line_loc"));
        line_loc->setText(QString::fromUtf8(""));

        horizontalLayout_4->addWidget(line_loc);

        b_loc = new QToolButton(AddProfile);
        b_loc->setObjectName(QString::fromUtf8("b_loc"));
        b_loc->setText(QString::fromUtf8("..."));

        horizontalLayout_4->addWidget(b_loc);


        verticalLayout->addLayout(horizontalLayout_4);

        label_8 = new QLabel(AddProfile);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout->addWidget(label_8);

        verticalSpacer_5 = new QSpacerItem(20, 9, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_5);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_png = new QLabel(AddProfile);
        label_png->setObjectName(QString::fromUtf8("label_png"));

        horizontalLayout_5->addWidget(label_png);

        line_png = new QLineEdit(AddProfile);
        line_png->setObjectName(QString::fromUtf8("line_png"));
        line_png->setText(QString::fromUtf8(""));

        horizontalLayout_5->addWidget(line_png);

        b_png = new QToolButton(AddProfile);
        b_png->setObjectName(QString::fromUtf8("b_png"));
        b_png->setText(QString::fromUtf8("..."));

        horizontalLayout_5->addWidget(b_png);


        verticalLayout->addLayout(horizontalLayout_5);

        label_2 = new QLabel(AddProfile);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        verticalSpacer_3 = new QSpacerItem(20, 9, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        line_2 = new QFrame(AddProfile);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        verticalSpacer = new QSpacerItem(20, 25, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_short = new QLabel(AddProfile);
        label_short->setObjectName(QString::fromUtf8("label_short"));

        horizontalLayout->addWidget(label_short);

        line_short = new QLineEdit(AddProfile);
        line_short->setObjectName(QString::fromUtf8("line_short"));
        line_short->setText(QString::fromUtf8(""));

        horizontalLayout->addWidget(line_short);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        label_6 = new QLabel(AddProfile);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout->addWidget(label_6);

        verticalSpacer_4 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_full = new QLabel(AddProfile);
        label_full->setObjectName(QString::fromUtf8("label_full"));

        horizontalLayout_2->addWidget(label_full);

        line_full = new QLineEdit(AddProfile);
        line_full->setObjectName(QString::fromUtf8("line_full"));
        line_full->setText(QString::fromUtf8(""));

        horizontalLayout_2->addWidget(line_full);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_2);

        label_7 = new QLabel(AddProfile);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout->addWidget(label_7);

        verticalSpacer_2 = new QSpacerItem(20, 29, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        line = new QFrame(AddProfile);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        b_cancel = new QPushButton(AddProfile);
        b_cancel->setObjectName(QString::fromUtf8("b_cancel"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../../../../usr/share/qtsixa/icons/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        b_cancel->setIcon(icon);

        horizontalLayout_3->addWidget(b_cancel);

        b_add = new QPushButton(AddProfile);
        b_add->setObjectName(QString::fromUtf8("b_add"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../../../../../../usr/share/qtsixa/icons/plus.png"), QSize(), QIcon::Normal, QIcon::Off);
        b_add->setIcon(icon1);

        horizontalLayout_3->addWidget(b_add);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(AddProfile);
        QObject::connect(b_cancel, SIGNAL(clicked()), AddProfile, SLOT(close()));

        QMetaObject::connectSlotsByName(AddProfile);
    } // setupUi

    void retranslateUi(QWidget *AddProfile)
    {
        AddProfile->setWindowTitle(QApplication::translate("AddProfile", "QtSixA - Add Profile", 0, QApplication::UnicodeUTF8));
        label_loc->setText(QApplication::translate("AddProfile", "FDI file location:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("AddProfile", "<i>(Use the \"Create New...\" button to make one)</i>", 0, QApplication::UnicodeUTF8));
        label_png->setText(QApplication::translate("AddProfile", "PNG file location:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AddProfile", "<i>(for previewing; use <a href=file:///usr/share/qtsixa/pics/sixa_x.xcf>/usr/share/qtsixa/pics/sixa_x.xcf</a> as template; then resize to half size)</i>", 0, QApplication::UnicodeUTF8));
        label_short->setText(QApplication::translate("AddProfile", "Short profile name:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("AddProfile", "<i>(non capitalized and no spaces!)</i>", 0, QApplication::UnicodeUTF8));
        label_full->setText(QApplication::translate("AddProfile", "Full profile name:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("AddProfile", "<i>(If possible, use only ASCII characters; space allowed)</i>", 0, QApplication::UnicodeUTF8));
        b_cancel->setText(QApplication::translate("AddProfile", "Cancel", 0, QApplication::UnicodeUTF8));
        b_add->setText(QApplication::translate("AddProfile", "Add Profile Now!", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddProfile: public Ui_AddProfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSIXA_ADDPROFILE_H
