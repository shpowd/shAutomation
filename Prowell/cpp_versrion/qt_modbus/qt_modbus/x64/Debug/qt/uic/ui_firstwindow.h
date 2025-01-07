/********************************************************************************
** Form generated from reading UI file 'firstwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIRSTWINDOW_H
#define UI_FIRSTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FirstWindow
{
public:
    QPushButton *pushButton1;

    void setupUi(QWidget *FirstWindow)
    {
        if (FirstWindow->objectName().isEmpty())
            FirstWindow->setObjectName("FirstWindow");
        FirstWindow->resize(579, 417);
        pushButton1 = new QPushButton(FirstWindow);
        pushButton1->setObjectName("pushButton1");
        pushButton1->setGeometry(QRect(160, 140, 201, 101));

        retranslateUi(FirstWindow);

        QMetaObject::connectSlotsByName(FirstWindow);
    } // setupUi

    void retranslateUi(QWidget *FirstWindow)
    {
        FirstWindow->setWindowTitle(QCoreApplication::translate("FirstWindow", "Form", nullptr));
        pushButton1->setText(QCoreApplication::translate("FirstWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FirstWindow: public Ui_FirstWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIRSTWINDOW_H
