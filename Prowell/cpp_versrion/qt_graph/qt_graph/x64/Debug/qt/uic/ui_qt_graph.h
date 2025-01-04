/********************************************************************************
** Form generated from reading UI file 'qt_graph.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_GRAPH_H
#define UI_QT_GRAPH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_graphClass
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qt_graphClass)
    {
        if (qt_graphClass->objectName().isEmpty())
            qt_graphClass->setObjectName("qt_graphClass");
        qt_graphClass->resize(973, 630);
        centralWidget = new QWidget(qt_graphClass);
        centralWidget->setObjectName("centralWidget");
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(130, 420, 161, 71));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(370, 420, 161, 71));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(590, 420, 161, 71));
        qt_graphClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(qt_graphClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 973, 33));
        qt_graphClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qt_graphClass);
        mainToolBar->setObjectName("mainToolBar");
        qt_graphClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(qt_graphClass);
        statusBar->setObjectName("statusBar");
        qt_graphClass->setStatusBar(statusBar);

        retranslateUi(qt_graphClass);
        QObject::connect(pushButton, &QPushButton::clicked, pushButton_2, qOverload<>(&QPushButton::hide));

        QMetaObject::connectSlotsByName(qt_graphClass);
    } // setupUi

    void retranslateUi(QMainWindow *qt_graphClass)
    {
        qt_graphClass->setWindowTitle(QCoreApplication::translate("qt_graphClass", "qt_graph", nullptr));
        pushButton->setText(QCoreApplication::translate("qt_graphClass", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("qt_graphClass", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("qt_graphClass", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_graphClass: public Ui_qt_graphClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_GRAPH_H
