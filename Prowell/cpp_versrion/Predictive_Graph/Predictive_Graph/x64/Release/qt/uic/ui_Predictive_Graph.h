/********************************************************************************
** Form generated from reading UI file 'Predictive_Graph.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREDICTIVE_GRAPH_H
#define UI_PREDICTIVE_GRAPH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Predictive_GraphClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Predictive_GraphClass)
    {
        if (Predictive_GraphClass->objectName().isEmpty())
            Predictive_GraphClass->setObjectName("Predictive_GraphClass");
        Predictive_GraphClass->resize(600, 400);
        menuBar = new QMenuBar(Predictive_GraphClass);
        menuBar->setObjectName("menuBar");
        Predictive_GraphClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Predictive_GraphClass);
        mainToolBar->setObjectName("mainToolBar");
        Predictive_GraphClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(Predictive_GraphClass);
        centralWidget->setObjectName("centralWidget");
        Predictive_GraphClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Predictive_GraphClass);
        statusBar->setObjectName("statusBar");
        Predictive_GraphClass->setStatusBar(statusBar);

        retranslateUi(Predictive_GraphClass);

        QMetaObject::connectSlotsByName(Predictive_GraphClass);
    } // setupUi

    void retranslateUi(QMainWindow *Predictive_GraphClass)
    {
        Predictive_GraphClass->setWindowTitle(QCoreApplication::translate("Predictive_GraphClass", "Predictive_Graph", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Predictive_GraphClass: public Ui_Predictive_GraphClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREDICTIVE_GRAPH_H
