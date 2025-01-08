#include "qt_modbus2.h"
#include <QModbusTcpServer>  // TCP 서버 관련 클래스
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>
#include <QUrl>


qt_modbus2::qt_modbus2(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("메인 윈도우");
    setMinimumSize(50, 40);
    resize(1024, 819);


    initUI();
}


qt_modbus2::~qt_modbus2(){
    
    if (settingWidget) {
        delete settingWidget;
    }
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
    }
}


void qt_modbus2::initUI(){
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(this);

    // 좌측 버튼 추가
    buttonLayout = new QVBoxLayout;
    QHBoxLayout* topButtonLayout = new QHBoxLayout;
    QHBoxLayout* bottomButtonLayout = new QHBoxLayout;

    for (int i = 0; i < 8; ++i) {
        QPushButton* button = new QPushButton(tr("그래프 %1").arg(i + 1), this);
        button->setMinimumSize(150, 150); // 버튼 크기 설정
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #f0f0f0;"
            "    border-radius: 10px;"
            "    border: 2px solid #d0d0d0;"
            "    color: black;"
            "    padding: 10px;"
            "    font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #e0e0e0;"
            "    border: 2px solid #b0b0b0;"
            "}"
        );
        if (i < 4) {
            topButtonLayout->addWidget(button);
        }
        else {
            bottomButtonLayout->addWidget(button);
        }
    }
    buttonLayout->addLayout(topButtonLayout);
    buttonLayout->addLayout(bottomButtonLayout);

    // 우측 텍스트 및 스크롤바
    textLayout = new QVBoxLayout;

    // 상단 Spacer 추가
    QSpacerItem* upperSpacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    textLayout->addSpacerItem(upperSpacer);

    // Modbus ID 설정
    QLabel* protocolLabel = new QLabel("Modbus ID:", this);
    protocolLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(protocolLabel);

    protocolSettings = new QTextEdit(this);
    protocolSettings->setText("1");
    protocolSettings->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
    protocolSettings->setFixedHeight(30);
    textLayout->addWidget(protocolSettings);

    // IP 설정
    QLabel* addressLabel = new QLabel("Ethernet IP:", this);
    addressLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(addressLabel);

    addressSettings = new QTextEdit(this);
    addressSettings->setText("127.0.0.1:50200");
    addressSettings->setStyleSheet("background-color: #f0f0f0; border: 1px solid #d0d0d0; border-radius: 5px;");
    addressSettings->setFixedHeight(30);
    textLayout->addWidget(addressSettings);

    // 통신 state 표시 위젯
    stateTextWidget = new QTextEdit(this);
    stateTextWidget->setText("이 영역은 통신 상태를 표시합니다."); // 초기 텍스트 설정
    stateTextWidget->setReadOnly(true); // 수정 불가능하게 설정
    stateTextWidget->setFixedHeight(30);
    stateTextWidget->setStyleSheet(
        "background-color: #eaeaea;" // 배경색을 연한 회색으로 설정
        "border: 1px solid #d0d0d0;" // 테두리 설정
        "border-radius: 5px;"        // 모서리를 둥글게
        "font-size: 14px;"           // 글자 크기 설정
        "color: #333333;"            // 글자 색 설정
    );
    textLayout->addWidget(stateTextWidget);

    // 통신 debug 표시 위젯
    debugTextWidget = new QTextEdit(this);
    debugTextWidget->setText("이 영역은 수신값을 표시합니다."); // 초기 텍스트 설정
    debugTextWidget->setReadOnly(true); // 수정 불가능하게 설정
    debugTextWidget->setFixedHeight(60);
    debugTextWidget->setStyleSheet(
        "background-color: #eaeaea;" // 배경색을 연한 회색으로 설정
        "border: 1px solid #d0d0d0;" // 테두리 설정
        "border-radius: 5px;"        // 모서리를 둥글게
        "font-size: 14px;"           // 글자 크기 설정
        "color: #333333;"            // 글자 색 설정
    );
    textLayout->addWidget(debugTextWidget);

    // 통신 버튼 레이아웃 (Connect, Disconnect)
    QHBoxLayout* conbuttonLayout = new QHBoxLayout;

    QPushButton* connectButton = new QPushButton("Connect", this);
    connectButton->setFixedHeight(60);
    conbuttonLayout->addWidget(connectButton);
    connect(connectButton, &QPushButton::clicked, this, &qt_modbus2::connectModbus);

    QPushButton* disconnectButton = new QPushButton("Disconnect", this);
    disconnectButton->setFixedHeight(60);
    conbuttonLayout->addWidget(disconnectButton);
    connect(disconnectButton, &QPushButton::clicked, this, &qt_modbus2::disconnectModbus);

    // 통신 버튼 레이아웃 추가
    textLayout->addLayout(conbuttonLayout);

    // 버튼 Spacer 추가
    QSpacerItem* buttonSpacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Minimum);
    textLayout->addSpacerItem(buttonSpacer);


    // 그래프 시간 설정
    QLabel* graphTimeLabel = new QLabel("Graph Display Time: 1 Minute", this);
    graphTimeLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(graphTimeLabel);

    QScrollBar* graphTimeScrollBar = new QScrollBar(Qt::Horizontal, this);
    graphTimeScrollBar->setMinimum(0);
    graphTimeScrollBar->setMaximum(3);
    graphTimeScrollBar->setPageStep(1);         // 한 번에 이동할 수 있는 범위
    graphTimeScrollBar->setStyleSheet(
        "QScrollBar:horizontal {"
        "    height: 40px;"                     // 스크롤바의 높이를 40px로 설정
        "    background-color: #d0d0d0;"        // 배경색을 회색빛으로 설정
        "    border: 2px solid #d0d0d0;"        // 테두리 색상 설정
        "    border-radius: 6px;"               // 둥근 모서리
        "}"
    );
    connect(graphTimeScrollBar, &QScrollBar::valueChanged, this, [graphTimeScrollBar, graphTimeLabel]() {
        int value = graphTimeScrollBar->value();
        QString text;
        switch (value) {
        case 0: text = "Graph Display Time: 1 Minute"; break;
        case 1: text = "Graph Display Time: 1 Hour"; break;
        case 2: text = "Graph Display Time: 1 Day"; break;
        case 3: text = "Graph Display Time: 1 Week"; break;
        }
        graphTimeLabel->setText(text);
        });
    textLayout->addWidget(graphTimeScrollBar);

    // DB 저장 주기 설정
    QLabel* saveTimeLabel = new QLabel("DB Save Frequency: 1 Minute", this);
    saveTimeLabel->setStyleSheet("font-weight: bold;");
    textLayout->addWidget(saveTimeLabel);

    QScrollBar* saveTimeScrollBar = new QScrollBar(Qt::Horizontal, this);
    saveTimeScrollBar->setMinimum(0);
    saveTimeScrollBar->setMaximum(3);
    saveTimeScrollBar->setPageStep(1);  // 한 번에 이동할 수 있는 범위
    saveTimeScrollBar->setStyleSheet(
        "QScrollBar:horizontal {"
        "    height: 40px;"  // 스크롤바의 높이를 40px로 설정
        "    background-color: #d0d0d0;"  // 배경색을 회색빛으로 설정
        "    border: 2px solid #d0d0d0;"  // 테두리 색상 설정
        "    border-radius: 6px;"          // 둥근 모서리
        "}"
    );
    connect(saveTimeScrollBar, &QScrollBar::valueChanged, this, [saveTimeScrollBar, saveTimeLabel]() {
        int value = saveTimeScrollBar->value();
        QString text;
        switch (value) {
        case 0: text = "DB Save Frequency: 1 Minute"; break;
        case 1: text = "DB Save Frequency: 1 Hour"; break;
        case 2: text = "DB Save Frequency: 1 Day"; break;
        case 3: text = "DB Save Frequency: 1 Week"; break;
        }
        saveTimeLabel->setText(text);
        });
    textLayout->addWidget(saveTimeScrollBar);

    // 저장 버튼
    QPushButton* saveButton = new QPushButton("Save", this);
    saveButton->setFixedHeight(60);
    connect(saveButton, &QPushButton::clicked, this, &QWidget::close);
    textLayout->addWidget(saveButton);

    // 저장 Spacer 추가
    QSpacerItem* saveSpacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    textLayout->addSpacerItem(saveSpacer);


    // 메인 레이아웃에 추가
    mainLayout->addLayout(buttonLayout, 3);  // 버튼 레이아웃
    mainLayout->addLayout(textLayout, 1);   // 텍스트 레이아웃
    
    // 중앙 위젯에 레이아웃 설정
    centralWidget->setLayout(mainLayout);

}


void qt_modbus2::connectModbus() {
    // UI에서 Modbus ID와 IP 주소 가져오기
    QString modbusID = protocolSettings->toPlainText(); // Modbus ID 가져오기
    QString ipAddress = addressSettings->toPlainText(); // IP 주소 가져오기

    // IP 주소 유효성 검사
    QUrl url = QUrl::fromUserInput(ipAddress);
    if (!url.isValid() || url.host().isEmpty() || url.port() <= 0) {
        qDebug() << "Invalid IP address or port:" << ipAddress;
        return;
    }

    // 기존 Modbus 서버 초기화
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    // Modbus TCP 서버 생성
    modbusDevice = new QModbusTcpServer(this);

    // Modbus 서버 설정
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
    modbusDevice->setServerAddress(modbusID.toInt());

    // 데이터 맵 초기화
    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 0, 30 });
    reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 10 });
    reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 10 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 100 });

    modbusDevice->setMap(reg);

    //// 상태 변경 핸들러 연결
    connect(modbusDevice, &QModbusServer::stateChanged, this, &qt_modbus2::onStateChanged);

    // 에러 핸들러 연결
    connect(modbusDevice, &QModbusServer::errorOccurred, this, &qt_modbus2::handleDeviceError);

    //// 데이터 변경 핸들러 연결
    connect(modbusDevice, &QModbusServer::dataWritten, this, &qt_modbus2::updateWidgets);

    // Modbus 서버 시작
    if (!modbusDevice->connectDevice()) {
        qDebug() << "Failed to start Modbus server:" << modbusDevice->errorString();
        delete modbusDevice;
        modbusDevice = nullptr;
        return;
    }

    qDebug() << "Modbus server started at" << ipAddress << "with ID" << modbusID;


}


void qt_modbus2::disconnectModbus() {
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }
}


void qt_modbus2::updateWidgets(QModbusDataUnit::RegisterType table, int address, int size) {
    if (!modbusDevice)
        return;

    for (int i = 0; i < size; ++i) {
        quint16 value;
        if (modbusDevice->data(table, address + i, &value)) {
            QString logMessage = QString("Table %1, Address %2, Value %3")
                .arg(static_cast<int>(table))
                .arg(address + i)
                .arg(value);

            // displayTextWidget에 변경 내용 추가
            if (debugTextWidget) {
                if (i == 0) {
                    debugTextWidget->setText(logMessage);
                }
                else {
                    QString currentText = debugTextWidget->toPlainText();
                    debugTextWidget->setText(currentText + "\n" + logMessage);
                }
            }

            // 디버그 로그 출력
            qDebug() << "log:" << logMessage;
        }
    }
}

void qt_modbus2::onStateChanged(int state) {
    QString stateMessage;
    switch (state) {
        case QModbusDevice::UnconnectedState:
            stateMessage = "Modbus server offline.";
            break;
        case QModbusDevice::ConnectingState:
            stateMessage = "Modbus server connecting...";
            break;
        case QModbusDevice::ConnectedState:
            stateMessage = "Modbus server online.";
            break;
        case QModbusDevice::ClosingState:
            stateMessage = "Modbus server closing...";
            break;
        default:
            stateMessage = "Modbus server unknown state.";
            break;
    }

    // displayTextWidget에 상태 메시지 추가
    if (stateTextWidget) {
        stateTextWidget->setText(stateMessage);
    }

    // 디버그 로그 출력
    qDebug() << "State message updated to:" << stateMessage;
}

void qt_modbus2::handleDeviceError(QModbusDevice::Error newError) {
    if (newError == QModbusDevice::NoError || !modbusDevice)
        return;

    QString errorMessage = modbusDevice->errorString();

    // displayTextWidget에 에러 메시지 추가
    if (debugTextWidget) {
        debugTextWidget->setText(errorMessage); // 새 메시지 추가
    }

    // 상태 표시줄 대신 이 코드로 표시
    qDebug() << "Error: " << errorMessage; // 디버그 로그 출력
}



// 세팅창 생성
void qt_modbus2::openSettingWidget() {
    if (!settingWidget) {
        settingWidget = new SettingWidget(this);
        settingWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(settingWidget, &QObject::destroyed, this, [this]() {
            settingWidget = nullptr; // 포인터 초기화
         });
    }

    settingWidget->show();
}