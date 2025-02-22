/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *applyButton;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QComboBox *parityCombo;
    QLabel *label_3;
    QComboBox *baudCombo;
    QLabel *label_4;
    QComboBox *dataBitsCombo;
    QLabel *label_5;
    QComboBox *stopBitsCombo;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(239, 256);
        gridLayout = new QGridLayout(SettingsDialog);
        gridLayout->setObjectName("gridLayout");
        verticalSpacer = new QSpacerItem(20, 43, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 1, 1, 1);

        applyButton = new QPushButton(SettingsDialog);
        applyButton->setObjectName("applyButton");

        gridLayout->addWidget(applyButton, 2, 1, 1, 1);

        groupBox = new QGroupBox(SettingsDialog);
        groupBox->setObjectName("groupBox");
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName("gridLayout_2");
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        parityCombo = new QComboBox(groupBox);
        parityCombo->addItem(QString());
        parityCombo->addItem(QString());
        parityCombo->addItem(QString());
        parityCombo->addItem(QString());
        parityCombo->addItem(QString());
        parityCombo->setObjectName("parityCombo");

        gridLayout_2->addWidget(parityCombo, 0, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        baudCombo = new QComboBox(groupBox);
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->addItem(QString());
        baudCombo->setObjectName("baudCombo");

        gridLayout_2->addWidget(baudCombo, 1, 1, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        dataBitsCombo = new QComboBox(groupBox);
        dataBitsCombo->addItem(QString());
        dataBitsCombo->addItem(QString());
        dataBitsCombo->addItem(QString());
        dataBitsCombo->addItem(QString());
        dataBitsCombo->setObjectName("dataBitsCombo");

        gridLayout_2->addWidget(dataBitsCombo, 2, 1, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 3, 0, 1, 1);

        stopBitsCombo = new QComboBox(groupBox);
        stopBitsCombo->addItem(QString());
        stopBitsCombo->addItem(QString());
        stopBitsCombo->addItem(QString());
        stopBitsCombo->setObjectName("stopBitsCombo");

        gridLayout_2->addWidget(stopBitsCombo, 3, 1, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 2);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Modbus Settings", nullptr));
        applyButton->setText(QCoreApplication::translate("SettingsDialog", "Apply", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SettingsDialog", "Serial Parameters", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Parity:", nullptr));
        parityCombo->setItemText(0, QCoreApplication::translate("SettingsDialog", "No", nullptr));
        parityCombo->setItemText(1, QCoreApplication::translate("SettingsDialog", "Even", nullptr));
        parityCombo->setItemText(2, QCoreApplication::translate("SettingsDialog", "Odd", nullptr));
        parityCombo->setItemText(3, QCoreApplication::translate("SettingsDialog", "Space", nullptr));
        parityCombo->setItemText(4, QCoreApplication::translate("SettingsDialog", "Mark", nullptr));

        label_3->setText(QCoreApplication::translate("SettingsDialog", "Baud Rate:", nullptr));
        baudCombo->setItemText(0, QCoreApplication::translate("SettingsDialog", "1200", nullptr));
        baudCombo->setItemText(1, QCoreApplication::translate("SettingsDialog", "2400", nullptr));
        baudCombo->setItemText(2, QCoreApplication::translate("SettingsDialog", "4800", nullptr));
        baudCombo->setItemText(3, QCoreApplication::translate("SettingsDialog", "9600", nullptr));
        baudCombo->setItemText(4, QCoreApplication::translate("SettingsDialog", "19200", nullptr));
        baudCombo->setItemText(5, QCoreApplication::translate("SettingsDialog", "38400", nullptr));
        baudCombo->setItemText(6, QCoreApplication::translate("SettingsDialog", "57600", nullptr));
        baudCombo->setItemText(7, QCoreApplication::translate("SettingsDialog", "115200", nullptr));

        label_4->setText(QCoreApplication::translate("SettingsDialog", "Data Bits:", nullptr));
        dataBitsCombo->setItemText(0, QCoreApplication::translate("SettingsDialog", "5", nullptr));
        dataBitsCombo->setItemText(1, QCoreApplication::translate("SettingsDialog", "6", nullptr));
        dataBitsCombo->setItemText(2, QCoreApplication::translate("SettingsDialog", "7", nullptr));
        dataBitsCombo->setItemText(3, QCoreApplication::translate("SettingsDialog", "8", nullptr));

        label_5->setText(QCoreApplication::translate("SettingsDialog", "Stop Bits:", nullptr));
        stopBitsCombo->setItemText(0, QCoreApplication::translate("SettingsDialog", "1", nullptr));
        stopBitsCombo->setItemText(1, QCoreApplication::translate("SettingsDialog", "3", nullptr));
        stopBitsCombo->setItemText(2, QCoreApplication::translate("SettingsDialog", "2", nullptr));

    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
