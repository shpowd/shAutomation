#ifndef QTMODBUS2_H
#define QTMODBUS2_H

#include "settingwidget.h"

#include <QMainWindow>
#include <QWidget>
#include <QModbusServer>

#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QLabel>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class qt_modbus2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit qt_modbus2(QWidget *parent = nullptr);
    ~qt_modbus2();

private slots:
    void openSettingWidget(); // 버튼 클릭 시 호출될 슬롯

private:
    void initUI();
    void connectModbus();
    void disconnectModbus();
    void handleDeviceError(QModbusDevice::Error newError);
    void onStateChanged(int state);
    void updateWidgets(QModbusDataUnit::RegisterType table, int address, int size);

    QTextEdit* protocolSettings; // Modbus ID 입력 필드
    QTextEdit* addressSettings; // IP 주소 입력 필드
    QTextEdit* stateTextWidget;  // 통신 state 표시 위젯
    QTextEdit* debugTextWidget; // 통신 Debug 표시 위젯

    QVBoxLayout* textLayout; // 텍스트 관련 위젯 레이아웃
    QVBoxLayout* buttonLayout; // 버튼 레이아웃
    QHBoxLayout* mainLayout; // 메인 레이아웃

    SettingWidget* settingWidget = nullptr; // 새로운 위젯
    QModbusServer* modbusDevice = nullptr;
    QButtonGroup coilButtons;
    QButtonGroup discreteButtons;
    QHash<QString, QLineEdit*> registers;
};

#endif
