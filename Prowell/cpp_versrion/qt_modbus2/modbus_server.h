#ifndef MODBUS_SERVER_H
#define MODBUS_SERVER_H

#include <QModbusTcpServer>
#include <QModbusPdu>
#include <QObject>
#include <QVector>
#include <QTimer>
#include <QDebug>
#include <QUrl>

// 전방 선언
class qt_modbus2;

class qt_modbus_server : public QModbusTcpServer {
    Q_OBJECT
public:
    explicit qt_modbus_server(QObject* parent = nullptr, qt_modbus2* mainWindow = nullptr);
    ~qt_modbus_server();

    bool startServer(const QString& ipAddress, int port, const QString& modbusID);

private:
    qt_modbus2* mainWindow; // qt_modbus2 객체 참조

protected:
    QModbusResponse processRequest(const QModbusPdu& request) override;

private slots:
    void handleWriteMultipleRegisters(const QModbusPdu& request);
    //void handleWriteMultipleCoils(const QModbusPdu& request);
    void handleWriteSingleCoil(const QModbusPdu& request);
};

#endif // MODBUS_SERVER_H