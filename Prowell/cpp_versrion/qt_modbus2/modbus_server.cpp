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

    QVector<quint16> values;
    for (int i = 0; i < quantity; ++i) {
        quint16 highByte = quint8(data[5 + i * 2]);    // 상위 바이트
        quint16 lowByte = quint8(data[6 + i * 2]);     // 하위 바이트
        quint16 value = (highByte << 8) | lowByte;     // 16비트 값 조합

        qDebug() << "Register" << startAddress + i << "=" << value;
        values.append(value);
    }

    // qt_modbus2::updateModbus 호출
    if (mainWindow) {
        mainWindow->updateModbus(QModbusDataUnit::HoldingRegisters, startAddress, values.size());
    }

}



void qt_modbus_server::handleWriteMultipleCoils(const QModbusPdu& request) {
    const QByteArray& data = request.data();

    // 데이터 크기 검증: 최소 5바이트 (헤더 크기)
    if (data.size() < 5) {
        qWarning() << "Invalid Write Multiple Coils request: insufficient header size.";
        return;
    }

    // 시작 주소와 코일 수 읽기
    quint16 startAddress = quint16((data[0] << 8) | quint8(data[1]));
    quint16 quantity = quint16((data[2] << 8) | quint8(data[3]));

    qDebug() << "Write Multiple Coils: Start Address =" << startAddress
        << ", Quantity =" << quantity;

    // 시작 주소와 범위 확인
    if (startAddress < 0 || startAddress >= 128 || (startAddress + quantity) > 128) {
        qWarning() << "Illegal Data Address: Start Address =" << startAddress
            << ", Quantity =" << quantity;
        return;
    }

    // 요청 처리에 필요한 데이터 크기 계산
    int requiredSize = 5 + (quantity + 7) / 8; // 헤더 크기(5) + 코일 데이터 크기
    if (data.size() < requiredSize) {
        qWarning() << "Data size too small for requested coils. Required:" << requiredSize
            << ", Provided:" << data.size();
        return;
    }

    // Coils 데이터를 읽어 bool 값으로 변환
    QVector<bool> values;
    values.reserve(quantity); // 벡터 크기 예약 (성능 최적화)
    for (int i = 0; i < quantity; ++i) {
        int byteIndex = 5 + (i / 8); // 데이터에서 바이트 위치
        int bitIndex = i % 8;        // 해당 바이트에서 비트 위치
        bool value = (data[byteIndex] & (1 << bitIndex)) != 0; // 비트 확인
        values.append(value);
    }

    // 각 코일 값을 처리하여 그래프 알람 업데이트
    for (int i = 0; i < quantity; ++i) {
        int coilIndex = startAddress + i; // 코일의 전체 주소

        if (mainWindow) {
            mainWindow->updateGraphAlarm(coilIndex, values[i]);
            mainWindow->savingCoilInput(coilIndex, values[i]);
        }
    }

    qDebug() << "Alarm updates processed successfully.";
}