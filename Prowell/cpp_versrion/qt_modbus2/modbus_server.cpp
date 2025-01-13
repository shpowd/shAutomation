#include "modbus_server.h"
#include "qt_modbus2.h" 

qt_modbus_server::qt_modbus_server(QObject* parent, qt_modbus2* mainWindow)
    : QModbusTcpServer(parent), mainWindow(mainWindow) {
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
        break;
    //case QModbusPdu::WriteMultipleCoils:
    //    handleWriteMultipleCoils(request);
    //    break;
    case QModbusPdu::WriteSingleCoil:
        handleWriteSingleCoil(request);
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

    QVector<quint16> values;
    for (int i = 0; i < quantity; ++i) {
        quint16 highByte = quint8(data[5 + i * 2]);    // ���� ����Ʈ
        quint16 lowByte = quint8(data[6 + i * 2]);     // ���� ����Ʈ
        quint16 value = (highByte << 8) | lowByte;     // 16��Ʈ �� ����

        qDebug() << "Register" << startAddress + i << "=" << value;
        values.append(value);
    }

    //// 11��° �������͸� �˶� ���·� �ؼ�
    //if (startAddress <= 10 && (startAddress + quantity) > 10) {
    //    int alarmIndex = 10 - startAddress;
    //    quint16 alarmValue = values[alarmIndex];
    //    int graphNumber = values[0] - 1; // �׷��� ��ȣ�� 1���� �����Ѵٰ� ����
    //    QVector<bool> alarmStates;
    //    for (int i = 0; i < 16; ++i) {
    //        alarmStates.append((alarmValue & (1 << i)) != 0);
    //    }
    //    if (mainWindow) {
    //        mainWindow->updateGraphAlarm(graphNumber, alarmStates);
    //    }
    //}

    // qt_modbus2::updateModbus ȣ��
    if (mainWindow) {
        mainWindow->updateModbus(QModbusDataUnit::HoldingRegisters, startAddress, values.size());
    }

}



void qt_modbus_server::handleWriteMultipleCoils(const QModbusPdu& request) {
    const QByteArray& data = request.data();

    // ������ ũ�� ����: �ּ� 5����Ʈ (��� ũ��)
    if (data.size() < 5) {
        qWarning() << "Invalid Write Multiple Coils request: insufficient header size.";
        return;
    }

    // ���� �ּҿ� ���� �� �б�
    quint16 startAddress = quint16((data[0] << 8) | quint8(data[1]));
    quint16 quantity = quint16((data[2] << 8) | quint8(data[3]));

    qDebug() << "Write Multiple Coils: Start Address =" << startAddress
        << ", Quantity =" << quantity;

    // ���� �ּҿ� ���� Ȯ��
    if (startAddress < 0 || startAddress >= 128 || (startAddress + quantity) > 128) {
        qWarning() << "Illegal Data Address: Start Address =" << startAddress
            << ", Quantity =" << quantity;
        return;
    }

    // ��û ó���� �ʿ��� ������ ũ�� ���
    int requiredSize = 5 + (quantity + 7) / 8; // ��� ũ��(5) + ���� ������ ũ��
    if (data.size() < requiredSize) {
        qWarning() << "Data size too small for requested coils. Required:" << requiredSize
            << ", Provided:" << data.size();
        return;
    }

    // Coils �����͸� �о� bool ������ ��ȯ
    QVector<bool> values;
    values.reserve(quantity); // ���� ũ�� ���� (���� ����ȭ)
    for (int i = 0; i < quantity; ++i) {
        int byteIndex = 5 + (i / 8); // �����Ϳ��� ����Ʈ ��ġ
        int bitIndex = i % 8;        // �ش� ����Ʈ���� ��Ʈ ��ġ
        bool value = (data[byteIndex] & (1 << bitIndex)) != 0; // ��Ʈ Ȯ��
        values.append(value);
    }

    // �� ���� ���� ó���Ͽ� �׷��� �˶� ������Ʈ
    for (int i = 0; i < quantity; ++i) {
        int coilIndex = startAddress + i; // ������ ��ü �ּ�

        if (mainWindow) {
            mainWindow->updateGraphAlarm(coilIndex, values[i]);
        }
    }

    if (mainWindow) {
        mainWindow->savingCoilInput(values); // ��� ���� ���¸� ����
    }
    qDebug() << "Alarm updates processed successfully.";
}



void qt_modbus_server::handleWriteSingleCoil(const QModbusPdu& request) {
    const QByteArray& data = request.data();

    if (data.size() < 4) {       // ������ ũ�� Ȯ�� (Write Single Coil�� �ּ� 4����Ʈ �ʿ�)
        qWarning() << "Invalid Write Single Coil request: insufficient data size.";
        return;
    }
    quint16 address = quint16((data[0] << 8) | quint8(data[1])); // 2����Ʈ �ּ�
    quint16 value = quint16((data[2] << 8) | quint8(data[3]));   // 2����Ʈ ��

    bool state = (value == 0xFF00);

    qDebug() << "Write Single Coil: Address =" << address << ", State =" << (state ? "ON" : "OFF");

    // �׷��� �˶� ������Ʈ
    if (mainWindow) {
        mainWindow->updateGraphAlarm(address-1, state); // ���� ���� ���� ������Ʈ
    }

    // �߰� ���� (Coil ����, ���� ���� �� �ʿ� �� ���� ����)
}