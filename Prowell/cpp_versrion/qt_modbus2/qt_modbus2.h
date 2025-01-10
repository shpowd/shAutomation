#ifndef QT_MODBUS2_H
#define QT_MODBUS2_H

#include "graph_widget.h"
#include "modbus_server.h"

#include <QMainWindow>
#include <QWidget>
#include <QModbusServer>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QModbusPdu>

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


class qt_modbus2 : public QMainWindow{
    Q_OBJECT

public:
    explicit qt_modbus2(QWidget *parent = nullptr);
    ~qt_modbus2();

private slots:
    void connectModbus();
    void disconnectModbus();
    void updateModbus(QModbusDataUnit::RegisterType table, int address, int size);
    void handleDeviceError(QModbusDevice::Error newError);
    void onStateChanged(int state);
    void saveDataOnTimer();

    void openGraphWidget(int graphIndex); // 특정 인덱스의 그래프를 열기
    void updateGraphData(const QVector<quint16>& values); // 그래프 데이터 업데이트

signals:
    void dataSavedToCSV(const QString& filePath);

private:
    void initUI();
    void savingInput(QModbusDataUnit::RegisterType table, int address, const QVector<quint16>& values);
    void saveDataToCSV();

    const int saveCSVtime = 3000; // CSV로 저장하기 위한 시간ms

    QTextEdit* protocolSettings; // Modbus ID 입력 필드
    QTextEdit* addressSettings; // IP 주소 입력 필드
    QTextEdit* stateTextWidget;  // 통신 state 표시 위젯
    QTextEdit* debugTextWidget; // 통신 Debug 표시 위젯

    QVBoxLayout* textLayout; // 텍스트 관련 위젯 레이아웃
    QVBoxLayout* buttonLayout; // 버튼 레이아웃
    QHBoxLayout* mainLayout; // 메인 레이아웃
    GraphWidget* graphWidget = nullptr; // 새로운 위젯


    qt_modbus_server *modbusDevice = nullptr;
    QButtonGroup coilButtons;
    QButtonGroup discreteButtons;
    QHash<QString, QLineEdit*> registers;
    QVector<QList<quint16>> saveBuffer; // modbus save csv 버퍼

    QDateTime connectionStartTime; // 연결 시작 시간을 추적
    QTimer saveTimer;              // 1분마다 저장을 처리할 타이머

    QMap<int, GraphWidget*> graphWidgets; // 그래프 위젯들을 관리하는 맵

};

#endif
