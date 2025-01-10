#include "qt_modbus2.h"
#include <QModbusTcpServer>  // TCP 서버 관련 클래스
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>


qt_modbus2::qt_modbus2(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("메인 윈도우");
    setMinimumSize(50, 40);
    resize(1024, 819);
    initUI();
}


qt_modbus2::~qt_modbus2(){
    
    if (graphWidget) {
        delete graphWidget;
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

        // 버튼 클릭 시 그래프 위젯 생성
        connect(button, &QPushButton::clicked, this, [this, i]() {
            openGraphWidget(i); // 그래프 생성 및 표시
            });

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
    QString modbusID = protocolSettings->toPlainText();
    QString ipAddress = addressSettings->toPlainText();

    // IP 주소 및 포트 유효성 검사
    QUrl url = QUrl::fromUserInput(ipAddress);
    if (!url.isValid() || url.host().isEmpty() || url.port() <= 0) {
        qDebug() << "Invalid IP address or port:" << ipAddress;
        return;
    }

    // 기존 Modbus 서버 초기화
    if (modbusDevice) {
        disconnectModbus();
    }

    // Modbus 서버 객체 생성
    modbusDevice = new qt_modbus_server(this);

    // 상태 변경 핸들러 연결
    connect(modbusDevice, &qt_modbus_server::stateChanged, this, &qt_modbus2::onStateChanged);
    // 에러 변경 핸들러 연결
    connect(modbusDevice, &qt_modbus_server::errorOccurred, this, &qt_modbus2::handleDeviceError);
    
    // 데이터 변경 핸들러 연결
    //connect(modbusDevice, &QModbusServer::dataWritten, this, &qt_modbus2::updateModbus);


    // Modbus 서버 시작
    if (modbusDevice->startServer(url.host(), url.port(), modbusID)) {
        qDebug() << "Modbus server successfully started at" << ipAddress << "with ID" << modbusID;
    }
    else {
        qDebug() << "Failed to start Modbus server.";
    }

    //connect(&saveTimer, &QTimer::timeout, this, &qt_modbus2::saveDataOnTimer);


}


void qt_modbus2::disconnectModbus() {
    if (modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
        qDebug() << "Modbus server destroyed.";
    }
}




void qt_modbus2::updateModbus(QModbusDataUnit::RegisterType table, int address, int size) {
    if (!modbusDevice)
        return;

    QVector<quint16> values;
    for (int i = 0; i < size; ++i) {
        quint16 value;
        if (modbusDevice->data(table, address + i, &value)) {
            values.append(value);

            qDebug() << "Register" << address + i << "updated to" << value;

            // displayTextWidget에 변경 내용 추가
            QString logMessage = QString("Table %1, Address %2, Value %3")
                .arg(static_cast<int>(table))
                .arg(address)
                .arg(value);
            if (debugTextWidget) {
                if (i == 0) {
                    debugTextWidget->setText(logMessage);
                }
                else {
                    QString currentText = debugTextWidget->toPlainText();
                    debugTextWidget->setText(currentText + "\n" + logMessage);
                }
            }
        }
    }

    if (!values.isEmpty()) {
        savingInput(table, address, values);
        updateGraphData(values);
    }
    else{
        qDebug() << "No values provided to updateGraphData.";
        return;
    }

    //// Holding Register 초기화
    //if (table == QModbusDataUnit::HoldingRegisters) {
    //    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, address, size);
    //    // 모든 값을 0으로 설정
    //    QVector<quint16> resetValues(size, 0); // 크기가 size인 벡터를 생성하고 모든 값을 0으로 초기화
    //    for (int i = 0; i < size; ++i) {
    //        unit.setValue(i, resetValues[i]);
    //    }
    //    if (!modbusDevice->setData(unit)) {
    //        qDebug() << "Failed to reset holding registers in range:" << address << "to" << address + size - 1;
    //    }
    //    else {
    //        qDebug() << "Holding registers in range" << address << "to" << address + size - 1 << "reset to 0.";
    //    }
    //}

}

void qt_modbus2::onStateChanged(int state) {
    QString stateMessage;
    switch (state) {
        case QModbusDevice::UnconnectedState:
            stateMessage = "Modbus server offline.";
            saveDataToCSV(); // 연결이 끊어진 경우 데이터 저장
            saveTimer.stop(); // 타이머 중지
            break;
        case QModbusDevice::ConnectingState:
            stateMessage = "Modbus server connecting...";
            break;
        case QModbusDevice::ConnectedState:
            stateMessage = "Modbus server online.";
            connectionStartTime = QDateTime::currentDateTime(); // 연결 시작 시간 재설정
            saveTimer.start(saveCSVtime); // 타이머 재시작
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


// 모드버스 CSV 데이터 셋
void qt_modbus2::savingInput(QModbusDataUnit::RegisterType table, int address, const QVector<quint16>& values){
    QList<quint16> newData;
    newData.append(static_cast<quint16>(table)); // 첫 번째 열에 table 값 추가
    newData.append(values);                     // 이후 열에 레지스터 값 추가
    saveBuffer.append(newData);

}

// 모드버스 CSV 저장 타이머
void qt_modbus2::saveDataOnTimer() {
    qDebug() << "Saving data after 1 minute of connection.";
    saveDataToCSV();
    saveBuffer.clear(); // 저장 후 버퍼 초기화
}

// 모드버스 CSV 저장
void qt_modbus2::saveDataToCSV() {

    // saveBuffer가 비어 있는 경우 저장하지 않음
    if (saveBuffer.isEmpty()) {
        qDebug() << "No data to save. Skipping CSV save.";
        return;
    }

    // log 폴더가 없다면 생성
    QDir dir;
    if (!dir.exists("log")) {
        if (!dir.mkpath("log")) {
            qDebug() << "Failed to create log directory.";
            return;
        }
    }
    QString filePath = QDir::currentPath() + "/log/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + "_modbus_data.csv";
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to save data to CSV file:" << filePath;
        return;
    }

    QTextStream out(&file);

    // 헤더 작성
    out << "Table";
    for (int i = 0; i < saveBuffer.first().size() - 1; ++i) {
        out << ",Address" << i;
    }
    out << "\n";

    // 데이터 작성
    for (const auto& row : saveBuffer) {
        for (int i = 0; i < row.size(); ++i) {
            if (i > 0) out << ",";
            out << row[i];
        }
        out << "\n";
    }

    file.close();
    
    saveBuffer.clear();
    qDebug() << "Data saved to CSV:" << filePath;
    emit dataSavedToCSV(filePath);

}


// 그래프 창 생성
void qt_modbus2::openGraphWidget(int graphIndex) {
    if (!graphWidgets.contains(graphIndex)) {
        // 그래프가 생성되지 않은 경우 생성
        GraphWidget* newGraphWidget = new GraphWidget(this);
        newGraphWidget->setAttribute(Qt::WA_DeleteOnClose);
        newGraphWidget->setWindowTitle(tr("그래프 %1").arg(graphIndex + 1)); // 그래프 제목 설정
        newGraphWidget->setChartTitle(tr("그래프 %1").arg(graphIndex + 1)); // 차트 레이블 설정
        connect(newGraphWidget, &QObject::destroyed, this, [this, graphIndex]() {
            graphWidgets.remove(graphIndex); // 삭제된 그래프 제거
            });

        graphWidgets.insert(graphIndex, newGraphWidget);
    }

    graphWidgets[graphIndex]->show(); // 그래프 표시
}

// 그래프 데이터 추가
void qt_modbus2::updateGraphData(const QVector<quint16>& values) {
    //인덱스에 따라 그래프 데이터 전달
    int graphIndex = values.first() - 1; // 첫 번째 값에서 1을 뺌 (0부터 시작하는 인덱스)
    if (graphIndex >= 0 && graphIndex < 8) { // 유효한 그래프 인덱스인지 확인
        if (!graphWidgets.contains(graphIndex)) { // 해당 인덱스에 그래프가 없을 경우
            qDebug() << "GraphWidget for index" << graphIndex + 1 << "does not exist.";
            return;
        }

        GraphWidget* graph = graphWidgets.value(graphIndex, nullptr); // 그래프 가져오기
        if (graph) {
            QDateTime now = QDateTime::currentDateTime();
            qint64 timestamp = now.toMSecsSinceEpoch();
            graph->addDataPoints(timestamp, values); // 데이터를 한 번에 추가
        }
        else {
            qDebug() << "GraphWidget for index" << graphIndex + 1 << "is nullptr.";
        }
    }
    else {
        qDebug() << "Invalid graph index:" << graphIndex + 1;
    }
}