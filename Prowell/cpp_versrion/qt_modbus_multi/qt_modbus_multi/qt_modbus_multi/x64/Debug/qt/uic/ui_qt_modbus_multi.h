/********************************************************************************
** Form generated from reading UI file 'qt_modbus_multi.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_MODBUS_MULTI_H
#define UI_QT_MODBUS_MULTI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_modbus_multiClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qt_modbus_multiClass)
    {
        if (qt_modbus_multiClass->objectName().isEmpty())
            qt_modbus_multiClass->setObjectName("qt_modbus_multiClass");
        qt_modbus_multiClass->resize(600, 400);
        menuBar = new QMenuBar(qt_modbus_multiClass);
        menuBar->setObjectName("menuBar");
        qt_modbus_multiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qt_modbus_multiClass);
        mainToolBar->setObjectName("mainToolBar");
        qt_modbus_multiClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(qt_modbus_multiClass);
        centralWidget->setObjectName("centralWidget");
        qt_modbus_multiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(qt_modbus_multiClass);
        statusBar->setObjectName("statusBar");
        qt_modbus_multiClass->setStatusBar(statusBar);

        retranslateUi(qt_modbus_multiClass);

        QMetaObject::connectSlotsByName(qt_modbus_multiClass);
    } // setupUi

    void retranslateUi(QMainWindow *qt_modbus_multiClass)
    {
        qt_modbus_multiClass->setWindowTitle(QCoreApplication::translate("qt_modbus_multiClass", "qt_modbus_multi", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_modbus_multiClass: public Ui_qt_modbus_multiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_MODBUS_MULTI_H
