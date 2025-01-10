#include "modbus_server.h"

qt_modbus_server::qt_modbus_server(QObject* parent)
    : QModbusTcpServer(parent) {
}

qt_modbus_server::~qt_modbus_server() {
}

bool qt_modbus_server::startServer(const QString& ipAddress, int port, const QString& modbusID) {
    setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipAddress);
    setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    setServerAddress(modbusID.toInt());

    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 20 });
    reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 160 });
    setMap(reg);

    if (!connectDevice()) {
        return false;
    }
    qDebug() << "Modbus server started at" << ipAddress << "with ID" << modbusID;
    return true;
}



QModbusResponse qt_modbus_server::processRequest(const QModbusPdu& request) {
    switch (request.functionCode()) {
    case QModbusPdu::WriteMultipleRegisters:
        handleWriteMultipleRegisters(request);
        qDebug() << "dddddddddddddddddddd";
        break;
    case QModbusPdu::WriteMultipleCoils:
        handleWriteMultipleCoils(request);
        break;
    default:
        break;
    }

    return QModbusTcpServer::processRequest(request);
}

void qt_modbus_server::handleWriteMultipleRegisters(const QModbusPdu& request) {
    const QByteArray& data = request.data();
    if (data.size() < 5) {
        qWarning() << "Invalid Write Multiple Registers request";
        return;
    }

    quint16 startAddress = quint16((data[0] << 8) | data[1]);
    quint16 quantity = quint16((data[2] << 8) | data[3]);

    qDebug() << "Write Multiple Registers: Start Address =" << startAddress
        << ", Quantity =" << quantity;

    for (int i = 0; i < quantity; ++i) {
        quint16 value = quint16((data[5 + i * 2] << 8) | data[6 + i * 2]);
        qDebug() << "Register" << startAddress + i << "=" << value;
    }
}

void qt_modbus_server::handleWriteMultipleCoils(const QModbusPdu& request) {
    const QByteArray& data = request.data();
    if (data.size() < 5) {
        qWarning() << "Invalid Write Multiple Coils request";
        return;
    }

    quint16 startAddress = quint16((data[0] << 8) | data[1]);
    quint16 quantity = quint16((data[2] << 8) | data[3]);

    qDebug() << "Write Multiple Coils: Start Address =" << startAddress
        << ", Quantity =" << quantity;

    for (int i = 0; i < quantity; ++i) {
        bool value = (data[5 + (i / 8)] & (1 << (i % 8))) != 0;
        qDebug() << "Coil" << startAddress + i << "=" << value;
    }
}