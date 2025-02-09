#pragma once

#include <QtWidgets/QMainWindow>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QTimer>
#include <QVector>
#include <QLabel>
#include "ui_qt_modbus_multi.h"


class qt_modbus_multi : public QMainWindow
{
    Q_OBJECT

public:
    qt_modbus_multi(QWidget* parent = nullptr);
    ~qt_modbus_multi();

private slots:
    void connectToSlave(int index);
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void pollSlaves();

private:
    Ui::qt_modbus_multiClass ui;
    QVector<QModbusTcpClient*> clients;
    QLabel* statusLabel;
};
