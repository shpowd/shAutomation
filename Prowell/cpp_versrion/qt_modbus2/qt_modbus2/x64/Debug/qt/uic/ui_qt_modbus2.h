/********************************************************************************
** Form generated from reading UI file 'qt_modbus2.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_MODBUS2_H
#define UI_QT_MODBUS2_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_modbus2Class
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qt_modbus2Class)
    {
        if (qt_modbus2Class->objectName().isEmpty())
            qt_modbus2Class->setObjectName("qt_modbus2Class");
        qt_modbus2Class->resize(600, 400);
        centralWidget = new QWidget(qt_modbus2Class);
        centralWidget->setObjectName("centralWidget");
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(140, 150, 221, 101));
        qt_modbus2Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(qt_modbus2Class);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        qt_modbus2Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qt_modbus2Class);
        mainToolBar->setObjectName("mainToolBar");
        qt_modbus2Class->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(qt_modbus2Class);
        statusBar->setObjectName("statusBar");
        qt_modbus2Class->setStatusBar(statusBar);

        retranslateUi(qt_modbus2Class);

        QMetaObject::connectSlotsByName(qt_modbus2Class);
    } // setupUi

    void retranslateUi(QMainWindow *qt_modbus2Class)
    {
        qt_modbus2Class->setWindowTitle(QCoreApplication::translate("qt_modbus2Class", "qt_modbus2", nullptr));
        pushButton->setText(QCoreApplication::translate("qt_modbus2Class", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_modbus2Class: public Ui_qt_modbus2Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_MODBUS2_H
