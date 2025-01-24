#ifndef QT_MODBUS_CLIENT_H
#define QT_MODBUS_CLIENT_H

#include <QModbusTcpClient>
#include <QModbusDataUnit>

class qt_modbus2; // mainWindow�� �����ϱ� ���� ����

class qt_modbus_client : public QModbusTcpClient {
    Q_OBJECT

public:
    explicit qt_modbus_client(QObject* parent = nullptr, qt_modbus2* mainWindow = nullptr);
    ~qt_modbus_client();

    bool connectToServer(const QString& ipAddress, int port);
    void readData(QModbusDataUnit::RegisterType type, quint16 startAddress, quint16 numberOfEntries);

signals:
    void dataReceived(const QModbusDataUnit& data); // �����͸� ���������� �������� �� �߻��ϴ� ��ȣ

private slots:
    void handleError(QModbusDevice::Error error); // ���� �߻� �� ó��

private:
    qt_modbus2* mainWindow; // UI�� �����ϱ� ���� mainWindow ����
};

#endif // QT_MODBUS_CLIENT_H
