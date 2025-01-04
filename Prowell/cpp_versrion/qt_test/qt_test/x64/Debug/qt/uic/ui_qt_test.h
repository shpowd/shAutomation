/********************************************************************************
** Form generated from reading UI file 'qt_test.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_TEST_H
#define UI_QT_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_testClass
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QTextBrowser *textBrowser;
    QTextEdit *textEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qt_testClass)
    {
        if (qt_testClass->objectName().isEmpty())
            qt_testClass->setObjectName("qt_testClass");
        qt_testClass->resize(991, 708);
        centralWidget = new QWidget(qt_testClass);
        centralWidget->setObjectName("centralWidget");
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(450, 420, 75, 24));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(340, 420, 75, 24));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(210, 420, 75, 24));
        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setGeometry(QRect(240, 140, 256, 192));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(580, 220, 104, 64));
        qt_testClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(qt_testClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 991, 33));
        qt_testClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qt_testClass);
        mainToolBar->setObjectName("mainToolBar");
        qt_testClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(qt_testClass);
        statusBar->setObjectName("statusBar");
        qt_testClass->setStatusBar(statusBar);

        retranslateUi(qt_testClass);
        QObject::connect(pushButton_3, &QPushButton::clicked, textBrowser, qOverload<>(&QTextBrowser::paste));

        QMetaObject::connectSlotsByName(qt_testClass);
    } // setupUi

    void retranslateUi(QMainWindow *qt_testClass)
    {
        qt_testClass->setWindowTitle(QCoreApplication::translate("qt_testClass", "qt_test", nullptr));
        pushButton->setText(QCoreApplication::translate("qt_testClass", "PushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("qt_testClass", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("qt_testClass", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_testClass: public Ui_qt_testClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_TEST_H
