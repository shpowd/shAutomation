#include "qt_modbus_multi.h"
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

constexpr int NUM_SLAVES = 16;
const QString SLAVE_IP = "127.0.0.1";
const int MODBUS_PORTS[NUM_SLAVES] = {
    1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509,
    1510, 1511, 1512, 1513, 1514, 1515, 1516, 1517
};

qt_modbus_multi::qt_modbus_multi(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    QVBoxLayout* layout = new QVBoxLayout;
    statusLabel = new QLabel("Initializing Modbus Clients...");
    layout->addWidget(statusLabel);

    QPushButton* pollButton = new QPushButton("Poll Slaves");
    layout->addWidget(pollButton);
    connect(pollButton, &QPushButton::clicked, this, &qt_modbus_multi::pollSlaves);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    for (int i = 0; i < NUM_SLAVES; ++i) {
        QModbusTcpClient* client = new QModbusTcpClient(this);
        clients.append(client);
        connect(client, &QModbusTcpClient::stateChanged, this, &qt_modbus_multi::onStateChanged);
        connect(client, &QModbusClient::errorOccurred, this, &qt_modbus_multi::onErrorOccurred);
        connectToSlave(i);
    }
}

qt_modbus_multi::~qt_modbus_multi() {}

void qt_modbus_multi::connectToSlave(int index) {
    if (index >= 0 && index < NUM_SLAVES) {
        clients[index]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, SLAVE_IP);
        clients[index]->setConnectionParameter(QModbusDevice::NetworkPortParameter, MODBUS_PORTS[index]);
        clients[index]->connectDevice();
    }
}

void qt_modbus_multi::onStateChanged(QModbusDevice::State state) {
    QModbusTcpClient* client = qobject_cast<QModbusTcpClient*>(sender());
    int index = clients.indexOf(client);
    if (index >= 0) {
        qDebug() << "Slave" << index + 1 << "State changed:" << state;
    }
}

void qt_modbus_multi::onErrorOccurred(QModbusDevice::Error error) {
    QModbusTcpClient* client = qobject_cast<QModbusTcpClient*>(sender());
    int index = clients.indexOf(client);
    if (index >= 0) {
        qDebug() << "Slave" << index + 1 << "Error:" << error;
    }
}

void qt_modbus_multi::pollSlaves() {
    for (int i = 0; i < NUM_SLAVES; ++i) {
        if (clients[i]->state() == QModbusDevice::ConnectedState) {
            QModbusDataUnit request(QModbusDataUnit::HoldingRegisters, 0, 10);
            QModbusReply* reply = clients[i]->sendReadRequest(request, 1);
            if (reply) {
                connect(reply, &QModbusReply::finished, this, [this, reply, i]() {
                    if (!reply->isFinished()) return;
                    if (reply->error() == QModbusDevice::NoError) {
                        qDebug() << "Slave" << i + 1 << "Response:" << reply->result().values();
                    }
                    else {
                        qDebug() << "Slave" << i + 1 << "Read error:" << reply->error();
                    }
                    reply->deleteLater();
                    });
            }
        }
    }
}
