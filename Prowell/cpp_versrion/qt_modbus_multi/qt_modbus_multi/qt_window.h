#pragma once

#include <QtWidgets/QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include <QTimer>
#include <QModbusTcpClient>
#include <QModbusDataUnit>

constexpr int NUM_REGISTERS = 14;  // 14���� ������ ���� ǥ��

class qt_window : public QWidget
{
    Q_OBJECT

public:
    explicit qt_window(QWidget* parent = nullptr);
    ~qt_window();

private slots:
    void connectToSlave(int index);
    void disconnectFromSlave(int index);
    void readFromSlave(int index);
    void updateStatus(int index, QModbusDevice::State state);

private:
    QVector<QLineEdit*> ipInputs;
    QVector<QLineEdit*> portInputs;
    QVector<QPushButton*> connectButtons;
    QVector<QPushButton*> disconnectButtons;
    QVector<QVector<QLabel*>> dataDisplays; // 14�� ���� �����ϴ� 2D ����
    QVector<QLabel*> statusDisplays;
    QVector<QModbusTcpClient*> clients;
    QVector<QTimer*> pollTimers;
};
