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

constexpr int NUM_REGISTERS = 14;  // 14개의 데이터 값을 표시

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
    QVector<QVector<QLabel*>> dataDisplays; // 14개 값을 저장하는 2D 벡터
    QVector<QLabel*> statusDisplays;
    QVector<QModbusTcpClient*> clients;
    QVector<QTimer*> pollTimers;
};
