/********************************************************************************
** Form generated from reading UI file 'qt_graph2.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_GRAPH2_H
#define UI_QT_GRAPH2_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_graph2Class
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qt_graph2Class)
    {
        if (qt_graph2Class->objectName().isEmpty())
            qt_graph2Class->setObjectName("qt_graph2Class");
        qt_graph2Class->resize(600, 400);
        menuBar = new QMenuBar(qt_graph2Class);
        menuBar->setObjectName("menuBar");
        qt_graph2Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qt_graph2Class);
        mainToolBar->setObjectName("mainToolBar");
        qt_graph2Class->addToolBar(mainToolBar);
        centralWidget = new QWidget(qt_graph2Class);
        centralWidget->setObjectName("centralWidget");
        qt_graph2Class->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(qt_graph2Class);
        statusBar->setObjectName("statusBar");
        qt_graph2Class->setStatusBar(statusBar);

        retranslateUi(qt_graph2Class);

        QMetaObject::connectSlotsByName(qt_graph2Class);
    } // setupUi

    void retranslateUi(QMainWindow *qt_graph2Class)
    {
        qt_graph2Class->setWindowTitle(QCoreApplication::translate("qt_graph2Class", "qt_graph2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_graph2Class: public Ui_qt_graph2Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_GRAPH2_H
