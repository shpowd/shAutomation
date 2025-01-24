#ifndef QT_MODBUS_CLIENT_H
#define QT_MODBUS_CLIENT_H

#include <QModbusTcpClient>
#include <QModbusDataUnit>

class qt_modbus2; // mainWindow를 참조하기 위한 선언

class qt_modbus_client : public QModbusTcpClient {
    Q_OBJECT

public:
    explicit qt_modbus_client(QObject* parent = nullptr, qt_modbus2* mainWindow = nullptr);
    ~qt_modbus_client();

    bool connectToServer(const QString& ipAddress, int port);
    void readData(QModbusDataUnit::RegisterType type, quint16 startAddress, quint16 numberOfEntries);

signals:
    void dataReceived(const QModbusDataUnit& data); // 데이터를 성공적으로 수신했을 때 발생하는 신호

private slots:
    void handleError(QModbusDevice::Error error); // 에러 발생 시 처리

private:
    qt_modbus2* mainWindow; // UI와 연동하기 위한 mainWindow 참조
};

#endif // QT_MODBUS_CLIENT_H
