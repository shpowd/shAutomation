#include "qt_window.h"
#include <QDebug>

constexpr int NUM_SLAVES = 16;

qt_window::qt_window(QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QVBoxLayout* leftColumn = new QVBoxLayout;
    QVBoxLayout* rightColumn = new QVBoxLayout;

    // clients와 pollTimers 리스트 초기화
    clients.resize(NUM_SLAVES);
    pollTimers.resize(NUM_SLAVES);
    dataDisplays.resize(NUM_SLAVES); // 14개 데이터를 가로로 표시하도록 초기화

    for (int i = 0; i < NUM_SLAVES; ++i) {
        QVBoxLayout* slaveLayout = new QVBoxLayout;

        // 상단: IP, Port, Connect, Disconnect 버튼 배치
        QHBoxLayout* topRow = new QHBoxLayout;

        QLabel* label = new QLabel(QString("Slave %1:").arg(i + 1));
        topRow->addWidget(label);

        QLineEdit* ipInput = new QLineEdit("127.0.0.1");
        ipInputs.append(ipInput);
        topRow->addWidget(ipInput);

        QLineEdit* portInput = new QLineEdit(QString::number(1502 + i));
        portInputs.append(portInput);
        topRow->addWidget(portInput);

        QPushButton* connectButton = new QPushButton("Connect");
        connectButtons.append(connectButton);
        topRow->addWidget(connectButton);

        QPushButton* disconnectButton = new QPushButton("Disconnect");
        disconnectButtons.append(disconnectButton);
        topRow->addWidget(disconnectButton);
        disconnectButton->setEnabled(false);  // 초기 상태에서는 비활성화

        connect(connectButton, &QPushButton::clicked, this, [this, i]() { connectToSlave(i); });
        connect(disconnectButton, &QPushButton::clicked, this, [this, i]() { disconnectFromSlave(i); });

        // 데이터 14개를 가로로 표시하는 행(Row) 생성
        QHBoxLayout* dataRow = new QHBoxLayout;
        for (int j = 0; j < NUM_REGISTERS; ++j) {
            QLabel* dataLabel = new QLabel("No Data");
            dataLabel->setFixedWidth(40);  // 각 데이터 크기 조정
            dataLabel->setAlignment(Qt::AlignCenter);
            dataLabel->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: white;");
            dataDisplays[i].append(dataLabel);
            dataRow->addWidget(dataLabel);
        }

        QLabel* statusDisplay = new QLabel("Disconnected");
        statusDisplay->setFixedHeight(20);
        statusDisplay->setAlignment(Qt::AlignCenter);
        statusDisplay->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: lightgray;");
        statusDisplays.append(statusDisplay);

        slaveLayout->addLayout(topRow);
        slaveLayout->addLayout(dataRow); // 14개 데이터를 가로로 출력
        slaveLayout->addWidget(statusDisplay);

        // 왼쪽 8개, 오른쪽 8개로 나누기
        if (i < 8) {
            leftColumn->addLayout(slaveLayout);
        }
        else {
            rightColumn->addLayout(slaveLayout);
        }
    }

    mainLayout->addLayout(leftColumn);
    mainLayout->addLayout(rightColumn);
    setLayout(mainLayout);
}

qt_window::~qt_window()
{
    for (auto client : clients) {
        if (client) {
            client->disconnectDevice();
            delete client;
        }
    }
    for (auto timer : pollTimers) {
        if (timer) {
            timer->stop();
            delete timer;
        }
    }
}

void qt_window::connectToSlave(int index)
{
    if (index < 0 || index >= NUM_SLAVES) return;

    QString ip = ipInputs[index]->text();
    int port = portInputs[index]->text().toInt();

    if (!clients[index]) {
        clients[index] = new QModbusTcpClient(this);
    }

    clients[index]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    clients[index]->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);

    connect(clients[index], &QModbusTcpClient::stateChanged, this, [this, index](QModbusDevice::State state) {
        updateStatus(index, state);
        if (state == QModbusDevice::ConnectedState) {
            connectButtons[index]->setEnabled(false);
            disconnectButtons[index]->setEnabled(true);
            if (!pollTimers[index]) {
                pollTimers[index] = new QTimer(this);
                connect(pollTimers[index], &QTimer::timeout, this, [this, index]() { readFromSlave(index); });
            }
            pollTimers[index]->start(1000);  // 1초마다 데이터 읽기
        }
        else {
            if (pollTimers[index]) {
                pollTimers[index]->stop();
            }
        }
        });

    connect(clients[index], &QModbusClient::errorOccurred, this, [this, index](QModbusDevice::Error error) {
        qDebug() << "Slave" << index + 1 << "Error:" << error;
        statusDisplays[index]->setText("Error");
        statusDisplays[index]->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: red;");
        });

    clients[index]->connectDevice();
}

void qt_window::disconnectFromSlave(int index)
{
    if (index < 0 || index >= NUM_SLAVES) return;

    if (clients[index]) {
        clients[index]->disconnectDevice();
        connectButtons[index]->setEnabled(true);
        disconnectButtons[index]->setEnabled(false);
        updateStatus(index, QModbusDevice::UnconnectedState);
    }
}

void qt_window::readFromSlave(int index)
{
    if (index < 0 || index >= NUM_SLAVES) return;
    if (clients[index]->state() != QModbusDevice::ConnectedState) return;

    QModbusDataUnit request(QModbusDataUnit::HoldingRegisters, 1000, NUM_REGISTERS);
    QModbusReply* reply = clients[index]->sendReadRequest(request, 1);

    if (reply) {
        connect(reply, &QModbusReply::finished, this, [this, reply, index]() {
            if (!reply->isFinished()) return;
            if (reply->error() == QModbusDevice::NoError) {
                QList<quint16> values = reply->result().values();

                for (int j = 0; j < values.size() && j < NUM_REGISTERS; ++j) {
                    dataDisplays[index][j]->setText(QString::number(values[j]));  // 14개 값을 가로로 표시
                }

                qDebug() << "Slave" << index + 1 << "Received Data:" << values;
            }
            else {
                for (int j = 0; j < NUM_REGISTERS; ++j) {
                    dataDisplays[index][j]->setText("Error");
                }
                qDebug() << "Slave" << index + 1 << "Read error:" << reply->error();
            }
            reply->deleteLater();
            });
    }
}

void qt_window::updateStatus(int index, QModbusDevice::State state)
{
    if (index < 0 || index >= NUM_SLAVES) return;

    switch (state) {
    case QModbusDevice::UnconnectedState:
        statusDisplays[index]->setText("Disconnected");
        statusDisplays[index]->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: lightgray;");
        break;
    case QModbusDevice::ConnectingState:
        statusDisplays[index]->setText("Connecting...");
        statusDisplays[index]->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: yellow;");
        break;
    case QModbusDevice::ConnectedState:
        statusDisplays[index]->setText("Connected");
        statusDisplays[index]->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: green;");
        break;
    case QModbusDevice::ClosingState:
        statusDisplays[index]->setText("Closing...");
        statusDisplays[index]->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: orange;");
        break;
    }
}
