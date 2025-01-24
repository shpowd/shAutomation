#include "modbus_client.h"
#include "qt_modbus2.h"

#include <QDebug>

qt_modbus_client::qt_modbus_client(QObject* parent, qt_modbus2* mainWindow)
    : QModbusTcpClient(parent), mainWindow(mainWindow) {
}

qt_modbus_client::~qt_modbus_client() {}

bool qt_modbus_client::connectToServer(const QString& ipAddress, int port) {
    setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipAddress);
    setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    setTimeout(2000);
    setNumberOfRetries(3);

    if (!connectDevice()) {
        qWarning() << "Failed to connect to server:" << errorString();
        return false;
    }

    qDebug() << "Connected to server at" << ipAddress << ":" << port;
    return true;
}

void qt_modbus_client::readData(QModbusDataUnit::RegisterType type, quint16 startAddress, quint16 numberOfEntries) {
    QModbusDataUnit unit(type, startAddress, numberOfEntries);
    auto* reply = sendReadRequest(unit, 1);

    if (!reply) {
        qWarning() << "Failed to send read request:" << errorString();
        return;
    }

    connect(reply, &QModbusReply::finished, this, [this, reply]() {
        if (reply->error() == QModbusDevice::NoError) {
            QModbusDataUnit result = reply->result();

            // mainWindow를 통해 UI 업데이트
            if (mainWindow) {
                // QModbusDataUnit에서 데이터 추출
                QVector<quint16> vectorData = result.values();

                // QVector -> QList 변환 (updateGraphData가 QList를 요구하는 경우)
                QList<quint16> listData = vectorData.toList();

                // mainWindow의 updateGraphData 호출
                mainWindow->updateGraphData(listData);
            }

            emit dataReceived(result); // 신호 전송
        }
        else {
            qWarning() << "Read error:" << reply->errorString();
        }
        reply->deleteLater();
        });
}

void qt_modbus_client::handleError(QModbusDevice::Error error) {
    if (error != QModbusDevice::NoError) {
        qWarning() << "Modbus client error:" << errorString();
    }
}
