#include "qt_modbus2.h"
#include <QModbusTcpServer>  // TCP 서버 관련 클래스
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>


qt_modbus2::qt_modbus2(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Main");
    setMinimumSize(50, 40);
    resize(1024, 819);
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);

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
        QPushButton* button = new QPushButton(tr("%1").arg(i + 1), this);
        button->setMinimumSize(150, 150); // 버튼 크기 설정
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #ADB9CA;"  // 기본 상태: 진한 회색
            "    color: white;"              // 기본 상태: 흰색 텍스트
            "    border-radius: 10px;"       // 둥근 모서리
            "    border: 2px solid #8c8c8c;" // 테두리: 더 어두운 회색
            "    font-size: 35px;"           // 텍스트 크기
            "    padding: 10px;"             // 텍스트 여백
            "}"
            "QPushButton:hover {"
            "    background-color: #2E75B6;" // 호버 상태: 파스텔 블루
            "    color: white;"              // 호버 상태: 흰색 텍스트
            "    border: 2px solid #87ceeb;" // 호버 상태: 밝은 블루 테두리
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

    graphTimeScrollBar = new QScrollBar(Qt::Horizontal, this);
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
    connect(graphTimeScrollBar, &QScrollBar::valueChanged, this, [graphTimeLabel](int value) {
        QString text;
        switch (value) {
        case 0: text = "Graph Display Time: 1 Minute"; break;
        case 1: text = "Graph Display Time: 1 Hour"; break;
        case 2: text = "Graph Display Time: 1 Day"; break;
        case 3: text = "Graph Display Time: 1 Month"; break;
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
    connect(saveTimeScrollBar, &QScrollBar::valueChanged, this, [this, saveTimeScrollBar, saveTimeLabel]() {
        int value = saveTimeScrollBar->value();
        QString text;
        switch (value) {
        case 0:
            text = "DB Save Frequency: 1 Minute";
            saveIntervalSeconds = 60; // 1분
            break;
        case 1:
            text = "DB Save Frequency: 1 Hour";
            saveIntervalSeconds = 3600; // 1시간
            break;
        case 2:
            text = "DB Save Frequency: 1 Day";
            saveIntervalSeconds = 86400; // 1일
            break;
        case 3:
            text = "DB Save Frequency: 1 Month";
            saveIntervalSeconds = 2629746; // 1달
            break;
        }
        saveTimeLabel->setText(text);
        qDebug() << "Updated save interval to" << saveIntervalSeconds << "seconds.";
        });
    textLayout->addWidget(saveTimeScrollBar);

    // 저장 버튼
    QPushButton* saveButton = new QPushButton("Save", this);
    saveButton->setFixedHeight(60);
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        saveTimer.stop(); // 기존 타이머 중지
        saveTimer.start(saveIntervalSeconds * 1000); // 새로운 주기로 시작
        qDebug() << "Save timer started with interval:" << saveIntervalSeconds << "seconds.";
        // 모든 그래프의 X축 스케일 업데이트
        int graphDisplayTime = graphTimeScrollBar->value();
        for (auto graph : graphWidgets) {
            graph->setRangeType(graphDisplayTime); // 그래프의 X축 스케일 변경
        }

        });
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
    modbusDevice = new qt_modbus_server(this, this);

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

    connect(&saveTimer, &QTimer::timeout, this, &qt_modbus2::saveDataOnTimer);

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
                .arg(address + i)
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
            saveTimer.start(saveIntervalSeconds * 1000); // 타이머 재시작
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


// 모드버스 레지스터 CSV 데이터 셋
void qt_modbus2::savingInput(QModbusDataUnit::RegisterType table, int address, const QVector<quint16>& values){
    QList<quint16> newData;
    newData.append(static_cast<quint16>(table)); // 첫 번째 열에 table 값 추가
    newData.append(values);                     // 이후 열에 레지스터 값 추가
    saveBuffer.append(newData);

}

void qt_modbus2::savingCoilInput(const QVector<bool>& coilStates) {
    if (coilStates.size() != 128) {
        qWarning() << "Invalid coil state size. Expected 128, got:" << coilStates.size();
        //return;
    }

    // 현재 상태를 버퍼에 추가
    QDateTime currentTime = QDateTime::currentDateTime();
    coilBuffer.append(qMakePair(currentTime, coilStates)); // 타임스탬프와 상태 저장
    qDebug() << "Coil states added to buffer. Buffer size:" << coilBuffer.size();
}



// 모드버스 CSV 저장 타이머
void qt_modbus2::saveDataOnTimer() {
    qDebug() << "Saving data after 1 minute of connection.";
    saveDataToCSV();
    saveCoilDataToCSV();
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

}

void qt_modbus2::saveCoilDataToCSV() {
    if (coilBuffer.isEmpty()) {
        qDebug() << "No coil data to save.";
        return;
    }

    QDir dir;
    if (!dir.exists("log")) {
        if (!dir.mkpath("log")) {
            qDebug() << "Failed to create log directory.";
            return;
        }
    }

    QString filePath = QDir::currentPath() + "/log/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + "_modbus_coil_data.csv";
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Failed to open file for saving coil data:" << filePath;
        return;
    }

    QTextStream out(&file);

    // 헤더 작성 (파일이 새로 생성된 경우만)
    if (file.size() == 0) {
        QStringList header;
        header << "Timestamp"; // 타임스탬프 추가
        for (int i = 0; i < 128; ++i) {
            header << QString("Coil Address %1").arg(i);
        }
        out << header.join(",") << "\n";
    }

    // 데이터 작성
    for (const auto& entry : coilBuffer) {
        QStringList row;
        row << entry.first.toString("yyyy-MM-dd hh:mm:ss"); // 첫 번째 열: 타임스탬프
        for (bool state : entry.second) {
            row << (state ? "1" : "0"); // 두 번째 열부터: 코일 데이터
        }
        out << row.join(",") << "\n";
    }

    file.close();
    coilBuffer.clear(); // 버퍼 초기화
    qDebug() << "Coil data saved to CSV:" << filePath;
}


// 그래프 창 생성
void qt_modbus2::openGraphWidget(int graphIndex) {
    if (!graphWidgets.contains(graphIndex)) {
        // 그래프가 생성되지 않은 경우 생성
        GraphWidget* newGraphWidget = new GraphWidget(this);
        newGraphWidget->setAttribute(Qt::WA_DeleteOnClose);
        newGraphWidget->setWindowTitle(tr("모터 %1").arg(graphIndex + 1)); // 그래프 제목 설정
        connect(newGraphWidget, &QObject::destroyed, this, [this, graphIndex]() {
            graphWidgets.remove(graphIndex); // 삭제된 그래프 제거
        });

        graphWidgets.insert(graphIndex, newGraphWidget);

        // 그래프 X축 범위를 save 버튼 설정 값으로 초기화
        int graphDisplayTime = graphTimeScrollBar->value();
        newGraphWidget->setRangeType(graphDisplayTime); // 설정된 X축 스케일 반영
    }

    graphWidgets[graphIndex]->show(); // 그래프 표시
}


// 그래프 데이터 갱신
void qt_modbus2::updateGraphData(const QVector<quint16>& values) {
    if (values.isEmpty()) {
        qDebug() << "No values provided for graph update.";
        return;
    }

    int graphIndex = values[0] - 1; // 0번 값이 그래프 인덱스 (1부터 시작한다고 가정)

    if (graphIndex >= 0 && graphWidgets.contains(graphIndex)) {
        GraphWidget* graph = graphWidgets.value(graphIndex, nullptr);
        if (graph) {
            QDateTime now = QDateTime::currentDateTime();
            qint64 timestamp = now.toMSecsSinceEpoch();

            // 그래프 데이터 업데이트 (실제값과 예측값 포함)
            graph->addDataPoints(timestamp, values);
        }
        else {
            qDebug() << "GraphWidget for index" << graphIndex + 1 << "is nullptr.";
        }
    }
    else {
        qDebug() << "Invalid graph index:" << graphIndex + 1;
    }
}


/* 알람 홀딩 레지스터용
void qt_modbus2::updateGraphAlarm(int graphIndex, const QVector<bool>& alarmStates) {
    // 그래프 번호가 유효한지 확인
    if (!graphWidgets.contains(graphIndex)) {
        qWarning() << "Invalid graph index:" << graphIndex;
        return;
    }
    GraphWidget* graph = graphWidgets.value(graphIndex, nullptr);
    if (!graph) {
        qWarning() << "GraphWidget for index" << graphIndex << "is nullptr.";
        return;
    }

    // 알람 상태 업데이트
    for (int i = 0; i < alarmStates.size(); ++i) {
        QLabel* alarmLabel = graph->findChild<QLabel*>(QString("alarmLabel_%1").arg(i + 1));
        if (alarmLabel) {
            if (alarmStates[i]) {           // 알람 ON 스타일                
                alarmLabel->setText(QString("알람%1 ON").arg(i + 1));
                alarmLabel->setStyleSheet(
                    "QLabel {"
                    "    background-color: orange;"
                    "    border: 1px solid #d0d0d0;"
                    "    border-radius: 5px;"
                    "    color: white;"
                    "    font-size: 14px;"
                    "    padding: 5px;"
                    "    text-align: center;"
                    "}"
                );
            }
            else {                          // 알람 OFF 스타일
                alarmLabel->setText(QString("알람%1 OFF").arg(i + 1));
                alarmLabel->setStyleSheet(
                    "QLabel {"
                    "    background-color: #f0f0f0;"
                    "    border: 1px solid #d0d0d0;"
                    "    border-radius: 5px;"
                    "    color: black;"
                    "    font-size: 14px;"
                    "    padding: 5px;"
                    "    text-align: center;"
                    "}"
                );
            }
        }
        else {
            qWarning() << "Alarm label not found for index" << i + 1 << "in graph" << graphIndex;
        }
    }
}
*/



void qt_modbus2::updateGraphAlarm(int coilIndex, bool state) {
    // 코일 번호에 따라 그래프 및 알람 인덱스 계산
    int graphIndex = coilIndex / 16; // 16개씩 나눠 그래프 인덱스 결정
    int alarmIndex = coilIndex % 16; // 그래프 내 알람 인덱스 결정

    // 그래프 1~8 범위 확인
    if (graphIndex >= 0 && graphIndex < 8) {
        GraphWidget* graph = graphWidgets.value(graphIndex, nullptr);
        if (graph) {
            // 그래프 내 알람 업데이트
            QLabel* alarmLabel = graph->findChild<QLabel*>(QString("alarmLabel_%1").arg(alarmIndex + 1));
            if (alarmLabel) {
                // 알람 상태에 따른 텍스트 및 스타일 설정
                if (state) {
                    alarmLabel->setText("알람 ON");
                    alarmLabel->setStyleSheet(
                        "QLabel {"
                        "    background-color: orange;"  // 배경색: 오렌지색
                        "    border: 1px solid #d0d0d0;" // 테두리
                        "    border-radius: 5px;"        // 둥근 모서리
                        "    color: white;"              // 텍스트 색상
                        "    font-size: 14px;"           // 글자 크기
                        "    padding: 5px;"              // 텍스트와 경계 간 간격
                        "    text-align: center;"        // 중앙 정렬
                        "}"
                    );
                }
                else {
                    alarmLabel->setText("알람 OFF");
                    alarmLabel->setStyleSheet(
                        "QLabel {"
                        "    background-color: #f0f0f0;"  // 배경색: 기본 회색
                        "    border: 1px solid #d0d0d0;"  // 테두리
                        "    border-radius: 5px;"         // 둥근 모서리
                        "    color: black;"               // 텍스트 색상
                        "    font-size: 14px;"            // 글자 크기
                        "    padding: 5px;"               // 텍스트와 경계 간 간격
                        "    text-align: center;"         // 중앙 정렬
                        "}"
                    );
                }
            }
        }
    }
    else {
        qWarning() << "Invalid graph index:" << graphIndex;
    }
}