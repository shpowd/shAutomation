#include "qt_window.h"
#include <QDebug>




// ✅ main 창 생성 함수
qt_window::qt_window(QWidget* parent)
    : QWidget(parent), siteSettingWindow(nullptr) {
    setWindowTitle("Main");
    setMinimumSize(200, 150);
    resize(1024, 768);
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);

    initMainUI();                       // ✅ UI 초기화
}

// ✅ main 창 소멸자
qt_window::~qt_window() {
    monitoringWindows.clear();  // 


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

// ✅ mainUI 생성 함수 
void qt_window::initMainUI(){
    setWindowTitle("Main");
    setMinimumSize(200, 150);
    resize(1024, 768);
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);
    

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);  // 상하 여백 추가

    // 중앙 테이블
    mainTableWidget = new QTableWidget(10, 3, this);
    mainTableWidget->setHorizontalHeaderLabels({ "No", "Description", "Monitoring" });

        // 헤더 스타일 설정
    QFont headerFont("맑은 고딕", 18, QFont::Bold);
    mainTableWidget->horizontalHeader()->setFont(headerFont);
    mainTableWidget->horizontalHeader()->setFixedHeight(55);
    mainTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {"
        "background-color: #EFF4F9;"
        "padding: 5px;"
        "}");

        // 테이블 셀 스타일 설정 (헤더보다 글자 크기가 약간 작음)
    mainTableWidget->setStyleSheet("QTableWidget::item {"
        "}");

    // 열 크기 조정 (자동 조정되도록 설정)
    mainTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // "No" 열은 내용에 맞춤
    mainTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // "Description" 열은 창 크기에 맞춤
    mainTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // "Monitoring" 열도 창 크기에 맞춤
    mainLayout->setStretchFactor(mainTableWidget, 1);

    // 행 크기 조정
    mainTableWidget->verticalHeader()->setVisible(false);
    mainTableWidget->verticalHeader()->setDefaultSectionSize(55); // 각 행(셀)의 높이 증가

    mainTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 수정 불가
    mainTableWidget->setSelectionMode(QAbstractItemView::NoSelection);   // 선택 불가
    mainLayout->addWidget(mainTableWidget);

    // 표와 버튼레이아웃 사이 여백
    mainLayout->addSpacing(20);



    // 하단 버튼 레이아웃 (페이지 버튼 + 환경 설정 버튼)
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    mainLayout->addLayout(bottomLayout);


    // 현장 설정 버튼 (좌측)
    QHBoxLayout* settingsButtonLayout = new QHBoxLayout;
    openSiteSettingButton = new QPushButton("현장 설정");
    openSiteSettingButton->setFixedSize(150, 50); // 버튼 크기
    applyButtonStyle(openSiteSettingButton);  // ✅ 스타일 적용
    settingsButtonLayout->addWidget(openSiteSettingButton); 
    bottomLayout->addLayout(settingsButtonLayout);
    connect(openSiteSettingButton, &QPushButton::clicked, this, &qt_window::openSiteSettingWindow);


    // 유동적 간격 추가 (환경설정 버튼과 페이지 버튼 사이)
    bottomLayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));


    // 페이지 변경 버튼 (중앙)
    mainPage1Button = new QPushButton("1");
    mainPage2Button = new QPushButton("2");
    mainPage1Button->setFixedSize(70, 50);
    mainPage2Button->setFixedSize(70, 50);
    applyButtonStyle(mainPage1Button);
    applyButtonStyle(mainPage2Button);
    bottomLayout->addWidget(mainPage1Button);
    bottomLayout->addWidget(mainPage2Button);

    // ✅ 버튼 클릭 시 페이지 변경 함수 호출
    connect(mainPage1Button, &QPushButton::clicked, this, [this]() { mainWindowDisplayPage(1); });
    connect(mainPage2Button, &QPushButton::clicked, this, [this]() { mainWindowDisplayPage(2); });
    

    // 유동적 간격 추가 (페이지 버튼과 로고 사이)
    bottomLayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));


    // 로고 추가 (우측 정렬)
    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPixmap("./src/logo.png");  // 로고 이미지 로드
    logoPixmap = logoPixmap.scaled(150, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 크기 조정
    logoLabel->setPixmap(logoPixmap);
    bottomLayout->addWidget(logoLabel);


    // 버튼 레이아웃 아래 여백
    mainLayout->addSpacing(20);

    setLayout(mainLayout);


    mainWindowDisplayPage(currentMainPage);

}


// ✅ main 페이지 전환 함수
void qt_window::mainWindowDisplayPage(int pageIndex){
    mainTableWidget->clearContents(); // 기존 데이터 삭제
    currentMainPage = pageIndex; // 현재 페이지 저장

    // ✅ 폰트 설정
    QFont cellFont("맑은 고딕", 16, QFont::Normal);

    for (int i = 0; i < 10; ++i) {
        int monitoringIndex = (pageIndex == 1) ? i + 1 : i + 11;

        // ✅ CSV에서 불러온 값 적용
        loadSettingsFromCSV();                                                                  // ✅ CSV 데이터 로드
        qDebug() << settings;
        QString descriptionText = settings.value(monitoringIndex).value("Description", "");     // ✅ 필드
        
        QTableWidgetItem* itemNo = new QTableWidgetItem(QString::number(monitoringIndex));
        QTableWidgetItem* description = new QTableWidgetItem(descriptionText);

        // ✅ 폰트 적용 (한 번만 설정)
        itemNo->setFont(cellFont);
        description->setFont(cellFont);

        // ✅ 중앙 정렬
        itemNo->setTextAlignment(Qt::AlignCenter);
        description->setTextAlignment(Qt::AlignCenter);

        // ✅ 모니터링 버튼 추가
        QPushButton* monitorButton = new QPushButton("monitoring" + QString::number(monitoringIndex));
        monitorButton->setFixedSize(250, 40);
        applyButtonStyle(monitorButton);
        // ✅ 버튼을 감싸는 위젯과 레이아웃 추가
        QWidget* monitoringContainer = new QWidget();
        QHBoxLayout* monitoringlayout = new QHBoxLayout(monitoringContainer);
        monitoringlayout->addWidget(monitorButton);
        monitoringlayout->setAlignment(Qt::AlignCenter);  // ✅ 아래위 + 좌우 모두 가운데 정렬
        monitoringlayout->setContentsMargins(0, 0, 0, 0); // ✅ 불필요한 여백 제거
        monitoringContainer->setLayout(monitoringlayout);


        // ✅ 버튼 클릭 시 Monitoring 창 열기
        connect(monitorButton, &QPushButton::clicked, this, [this, monitoringIndex]() {
            openMonitoringWindow(monitoringIndex);
        });

        mainTableWidget->setItem(i, 0, itemNo);
        mainTableWidget->setItem(i, 1, description);
        mainTableWidget->setCellWidget(i, 2, monitoringContainer);
    }
}



// ✅ "Monitoring" 창 생성 함수
void qt_window::openMonitoringWindow(int monitoringIndex) {
    if (monitoringWindows.contains(monitoringIndex) && !monitoringWindows[monitoringIndex].isNull()) {
        monitoringWindows[monitoringIndex]->raise();
        monitoringWindows[monitoringIndex]->activateWindow();
        return;
    }

    // ✅ MonitoringWindow를 자식 창으로 실행
    MonitoringWindow* monitoringWindow = new MonitoringWindow(monitoringIndex, settings, this);
    monitoringWindows[monitoringIndex] = monitoringWindow;
    monitoringWindow->move(this->pos());
    monitoringWindow->show();

    // ✅ 창이 닫힐 때 monitoringWindows에서 제거
    connect(monitoringWindow, &MonitoringWindow::windowClosed, this, [this](int monitoringIndex) {
        monitoringWindows.remove(monitoringIndex);
        qDebug() << "✅ Monitoring 창이 닫혀서 제거됨: " << monitoringIndex;
    });

    monitoringWindows[monitoringIndex] = monitoringWindow;
}





// ✅ "현장 설정" 창 생성 함수
void qt_window::openSiteSettingWindow(){
    if (siteSettingWindow) {
        siteSettingWindow->close();
    }

    if (!siteSettingWindow) {
        siteSettingWindow = new QWidget;
        siteSettingWindow->setWindowTitle("현장 설정");
        siteSettingWindow->resize(1024, 768);


        // ✅ 기본 레이아웃 (qt_window와 동일)
        QVBoxLayout* mainLayout = new QVBoxLayout(siteSettingWindow);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // ✅ 중앙 테이블 (10행 5열)
        siteSettingTableWidget = new QTableWidget(10, 5, siteSettingWindow);
        siteSettingTableWidget->setHorizontalHeaderLabels({ "No", "Description", "통신설정", "통신 상태", "비고" });

        // ✅ 헤더 스타일 설정
        QFont headerFont("맑은 고딕", 18, QFont::Bold);
        siteSettingTableWidget->horizontalHeader()->setFont(headerFont);
        siteSettingTableWidget->horizontalHeader()->setFixedHeight(55);
        siteSettingTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {"
            "background-color: #EFF4F9;"
            "padding: 5px;"
            "}");

        // ✅ 테이블 셀 스타일 설정
        siteSettingTableWidget->setStyleSheet("QTableWidget::item {"
            "font-size: 18px;"
            "padding: 5px;"
            "}");

        // ✅ 열 크기 조정
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

        siteSettingTableWidget->verticalHeader()->setVisible(false);
        siteSettingTableWidget->verticalHeader()->setDefaultSectionSize(55);
        mainLayout->addWidget(siteSettingTableWidget);

        // ✅ 표와 버튼 레이아웃 사이 여백
        mainLayout->addSpacing(20);


        // ✅ 하단 버튼 레이아웃 (Main 버튼, Save 버튼, 페이지 전환 버튼, 로고)
        QHBoxLayout* bottomLayout = new QHBoxLayout;
        mainLayout->addLayout(bottomLayout);

        // ✅ Main 버튼 (좌측)
        QPushButton* mainButton = new QPushButton("Main");
        mainButton->setFixedSize(150, 50);
        mainButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
        applyButtonStyle(mainButton);
        bottomLayout->addWidget(mainButton);
        connect(mainButton, &QPushButton::clicked, this, [=]() {
            if (siteSettingWindow) {
                siteSettingWindow->close();
                mainWindowDisplayPage(currentMainPage);
                qDebug() << "✅ Main 버튼 클릭 - Setup Window 닫힘";
            }
        });

        // ✅ Save 버튼 (좌측)
        QPushButton* saveButton = new QPushButton("Save");
        saveButton->setFixedSize(150, 50);
        saveButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
        applyButtonStyle(saveButton);
        bottomLayout->addWidget(saveButton);
        connect(saveButton, &QPushButton::clicked, this, &qt_window::siteSettingWindowSave);

        // ✅ 유동적 간격 추가 (버튼과 페이지 버튼 사이)
        bottomLayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));

        // ✅ 페이지 변경 버튼 (중앙)
        QPushButton* page1Button = new QPushButton("1");
        QPushButton* page2Button = new QPushButton("2");
        page1Button->setFixedSize(70, 50);
        page2Button->setFixedSize(70, 50);
        applyButtonStyle(page1Button);
        applyButtonStyle(page2Button);
        bottomLayout->addWidget(page1Button);
        bottomLayout->addWidget(page2Button);

        // ✅ 버튼 클릭 시 페이지 변경 함수 호출
        connect(page1Button, &QPushButton::clicked, this, [this]() { siteSettingWindowDisplayPage(1); });
        connect(page2Button, &QPushButton::clicked, this, [this]() { siteSettingWindowDisplayPage(2); });

        siteSettingWindowDisplayPage(1); // 초기 페이지 로드


        // ✅ 유동적 간격 추가 (페이지 버튼과 로고 사이)
        bottomLayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));

        // ✅ 로고 추가 (우측)
        QLabel* logoLabel = new QLabel(siteSettingWindow);
        QPixmap logoPixmap("./src/logo.png");
        logoPixmap = logoPixmap.scaled(150, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        logoLabel->setPixmap(logoPixmap);
        bottomLayout->addWidget(logoLabel);

        // ✅ 버튼 레이아웃 아래 여백
        mainLayout->addSpacing(20);


        siteSettingWindow->setLayout(mainLayout);

    }

    siteSettingWindow->show();
}

// ✅ "현장 설정" 페이지 전환 함수
void qt_window::siteSettingWindowDisplayPage(int pageIndex){
    siteSettingTableWidget->clearContents(); // 기존 데이터 삭제
    currentSiteSettingpPage = pageIndex; // 현재 페이지 저장
    
    // ✅ 폰트 설정
    QFont cellFont("맑은 고딕", 16, QFont::Normal);

    for (int i = 0; i < 10; ++i) {
        int monitoringIndex = (pageIndex == 1) ? i + 1 : i + 11;

        // ✅ CSV에서 불러온 값 적용 (필드명 정확하게 확인)
        loadSettingsFromCSV();                                                                  // ✅ CSV 데이터 로드
        QString descriptionText = settings.value(monitoringIndex).value("Description", "");     // ✅ 필드
        QString notesText = settings.value(monitoringIndex).value("Notes", "");                 // ✅ 필드

        QTableWidgetItem* itemNo = new QTableWidgetItem(QString::number(monitoringIndex));
        QTableWidgetItem* description = new QTableWidgetItem(descriptionText);
        QTableWidgetItem* commState = new QTableWidgetItem(QString("Value %1").arg(monitoringIndex + 20));
        QTableWidgetItem* notes = new QTableWidgetItem(notesText);

        // ✅ 폰트 적용 (메인 페이지와 동일)
        itemNo->setFont(cellFont);
        description->setFont(cellFont);
        commState->setFont(cellFont);
        notes->setFont(cellFont);

        // ✅ 중앙 정렬 (기존 구조 유지)
        itemNo->setTextAlignment(Qt::AlignCenter);
        description->setTextAlignment(Qt::AlignCenter);
        commState->setTextAlignment(Qt::AlignCenter);
        notes->setTextAlignment(Qt::AlignCenter);

        // ✅ "No"와 "통신 상태" 열을 선택 불가능하게 설정
        itemNo->setFlags(Qt::ItemIsEnabled);
        commState->setFlags(Qt::ItemIsEnabled);

        // ✅ "통신 설정" 버튼 추가 (메인 윈도우의 Monitoring 버튼과 동일한 스타일)
        QPushButton* commButton = new QPushButton("통신 설정" + QString::number(monitoringIndex));
        applyButtonStyle(commButton);  // ✅ 기존 스타일 적용
        // ✅ 버튼 클릭 시 "통신 설정" 팝업 창 열기
        connect(commButton, &QPushButton::clicked, this, [this, monitoringIndex]() {
            openCommSettingsWindow(monitoringIndex);
            });


        // ✅ 아이템 및 버튼 추가
        siteSettingTableWidget->setItem(i, 0, itemNo);
        siteSettingTableWidget->setItem(i, 1, description);
        siteSettingTableWidget->setCellWidget(i, 2, commButton);  // ✅ 3열(통신 설정 버튼 추가)
        siteSettingTableWidget->setItem(i, 3, commState);
        siteSettingTableWidget->setItem(i, 4, notes);
    }
}

// ✅ "현장 설정" 페이지 SAVE 버튼 함수
void qt_window::siteSettingWindowSave() {
    for (int i = 0; i < 10; ++i) {
        int monitoringIndex = (currentSiteSettingpPage == 1) ? i + 1 : i + 11;

        QTableWidgetItem* descriptionItem = siteSettingTableWidget->item(i, 1);
        QTableWidgetItem* notesItem = siteSettingTableWidget->item(i, 4);

        QString description = descriptionItem ? descriptionItem->text() : "";
        QString notes = notesItem ? notesItem->text() : "";

        // ✅ settings 업데이트 시 필드명 일치
        settings[monitoringIndex]["Description"] = description;
        settings[monitoringIndex]["Notes"] = notes;
    }

    saveSettingsToCSV();
    qDebug() << "✅ siteSettingWindow 설정이 저장되었습니다.";
}


// ✅ "통신 설정" 창 생성 함수
void qt_window::openCommSettingsWindow(int monitoringIndex){
    QWidget* commSettingsDialog = new QWidget;
    commSettingsDialog->setWindowTitle("통신 설정 " + QString::number(monitoringIndex));
    commSettingsDialog->setFixedSize(400, 350);

    QVBoxLayout* mainLayout = new QVBoxLayout(commSettingsDialog);

    // ✅ 폰트 설정
    QFont font("맑은 고딕", 16);

    // ✅ Modbus TCP/IP 설정 그룹박스
    QGroupBox* modbusGroup = new QGroupBox("Modbus TCP/IP 설정");
    modbusGroup->setFont(font);
    QVBoxLayout* groupLayout = new QVBoxLayout(modbusGroup);

    // ✅ CSV에서 기존 설정 불러오기
    loadSettingsFromCSV();  // ✅ CSV 데이터 로드

    // ✅ Modbus ID 입력 필드
    QLabel* modbusIdLabel = new QLabel("Modbus ID:");
    modbusIdLabel->setFont(font);
    QLineEdit* modbusIdInput = new QLineEdit();
    modbusIdInput->setFont(font);
    modbusIdInput->setPlaceholderText("예: 1");
    modbusIdInput->setText(settings[monitoringIndex].value("Modbus ID", ""));

    // ✅ IP 주소 입력 필드
    QLabel* ipLabel = new QLabel("IP 주소:");
    ipLabel->setFont(font);
    QLineEdit* ipInput = new QLineEdit();
    ipInput->setFont(font);
    ipInput->setPlaceholderText("예: 192.168.0.1");
    ipInput->setText(settings[monitoringIndex].value("IP", ""));

    // ✅ 포트 번호 입력 필드
    QLabel* portLabel = new QLabel("포트 번호:");
    portLabel->setFont(font);
    QLineEdit* portInput = new QLineEdit();
    portInput->setFont(font);
    portInput->setPlaceholderText("예: 502");
    portInput->setText(settings[monitoringIndex].value("Port", ""));

    // ✅ 그룹 레이아웃에 위젯 추가
    groupLayout->addWidget(modbusIdLabel);
    groupLayout->addWidget(modbusIdInput);
    groupLayout->addWidget(ipLabel);
    groupLayout->addWidget(ipInput);
    groupLayout->addWidget(portLabel);
    groupLayout->addWidget(portInput);
    modbusGroup->setLayout(groupLayout);

    // ✅ 버튼 레이아웃 (저장 + 닫기)
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    // ✅ 저장 버튼
    QPushButton* saveButton = new QPushButton("저장");
    saveButton->setFont(font);
    saveButton->setFixedSize(150, 50);
    applyButtonStyle(saveButton);  // ✅ 기존 스타일 적용
    connect(saveButton, &QPushButton::clicked, commSettingsDialog, [=]() {
        settings[monitoringIndex]["Modbus ID"] = modbusIdInput->text();
        settings[monitoringIndex]["IP"] = ipInput->text();
        settings[monitoringIndex]["Port"] = portInput->text();
        saveSettingsToCSV();
        commSettingsDialog->close();
        });

    // ✅ 닫기 버튼
    QPushButton* closeButton = new QPushButton("닫기");
    closeButton->setFont(font);
    closeButton->setFixedSize(150, 50);
    applyButtonStyle(closeButton);  // ✅ 기존 스타일 적용
    connect(closeButton, &QPushButton::clicked, commSettingsDialog, &QWidget::close);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);

    mainLayout->addWidget(modbusGroup);
    mainLayout->addLayout(buttonLayout);

    commSettingsDialog->setLayout(mainLayout);
    commSettingsDialog->show();
}




// ✅ CSV 설정 불러오기 함수 
void qt_window::loadSettingsFromCSV() {
    settings = CSVReader::readAllSettings();
    qDebug() << "✅ CSVReader 사용하여 설정을 불러왔습니다.";
}

// ✅ CSV 설정 저장 함수 
void qt_window::saveSettingsToCSV() {
    CSVReader::writeAllSettings(settings);
    qDebug() << "✅ CSVReader 사용하여 설정을 저장했습니다.";
}







//이전 modbus 접속 설정 
//void qt_window::openSetupWindow()
//{
//    if (!setupWindow) {
//        setupWindow = new QWidget;
//        setupWindow->setWindowTitle("Setup Window");
//
//        QHBoxLayout* mainLayout = new QHBoxLayout(setupWindow);
//        QVBoxLayout* leftColumn = new QVBoxLayout;
//        QVBoxLayout* rightColumn = new QVBoxLayout;
//
//        clients.resize(NUM_SLAVES);
//        pollTimers.resize(NUM_SLAVES);
//        dataDisplays.resize(NUM_SLAVES);
//
//        for (int i = 0; i < NUM_SLAVES; ++i) {
//            QVBoxLayout* slaveLayout = new QVBoxLayout;
//
//            QHBoxLayout* controlRow = new QHBoxLayout;
//            QLabel* slaveLabel = new QLabel(QString("Slave %1:").arg(i + 1));
//            controlRow->addWidget(slaveLabel);
//
//            QLineEdit* ipField = new QLineEdit("127.0.0.1");
//            ipInputs.append(ipField);
//            controlRow->addWidget(ipField);
//
//            QLineEdit* portField = new QLineEdit(QString::number(1502 + i));
//            portInputs.append(portField);
//            controlRow->addWidget(portField);
//
//            QPushButton* connectBtn = new QPushButton("Connect");
//            connectButtons.append(connectBtn);
//            controlRow->addWidget(connectBtn);
//
//            QPushButton* disconnectBtn = new QPushButton("Disconnect");
//            disconnectButtons.append(disconnectBtn);
//            controlRow->addWidget(disconnectBtn);
//            disconnectBtn->setEnabled(false);
//
//            connect(connectBtn, &QPushButton::clicked, this, [this, i]() { connectToSlave(i); });
//            connect(disconnectBtn, &QPushButton::clicked, this, [this, i]() { disconnectFromSlave(i); });
//
//            QHBoxLayout* dataRow = new QHBoxLayout;
//            for (int j = 0; j < NUM_REGISTERS; ++j) {
//                QLabel* dataLabel = new QLabel("No Data");
//                dataLabel->setFixedWidth(40);
//                dataLabel->setAlignment(Qt::AlignCenter);
//                dataLabel->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: white;");
//                dataDisplays[i].append(dataLabel);
//                dataRow->addWidget(dataLabel);
//            }
//
//            QLabel* statusLabel = new QLabel("Disconnected");
//            statusLabel->setFixedHeight(20);
//            statusLabel->setAlignment(Qt::AlignCenter);
//            statusLabel->setStyleSheet("border: 1px solid gray; padding: 2px; background-color: lightgray;");
//            statusDisplays.append(statusLabel);
//
//            slaveLayout->addLayout(controlRow);
//            slaveLayout->addLayout(dataRow);
//            slaveLayout->addWidget(statusLabel);
//
//            if (i < 8) {
//                leftColumn->addLayout(slaveLayout);
//            }
//            else {
//                rightColumn->addLayout(slaveLayout);
//            }
//        }
//
//        mainLayout->addLayout(leftColumn);
//        mainLayout->addLayout(rightColumn);
//        setupWindow->setLayout(mainLayout);
//    }
//
//    setupWindow->show();
//}

// Modbus 연결
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
            pollTimers[index]->start(1000);
        }
        else {
            if (pollTimers[index]) {
                pollTimers[index]->stop();
            }
        }
        });

    clients[index]->connectDevice();
}

// 연결 상태 업데이트 함수
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

// Modbus 연결 해제
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

// 데이터 읽기
void qt_window::readFromSlave(int index)
{
    if (index < 0 || index >= NUM_SLAVES) return;
    if (clients[index]->state() != QModbusDevice::ConnectedState) return;

    QModbusDataUnit request(QModbusDataUnit::HoldingRegisters, 1000, NUM_REGISTERS);
    QModbusReply* reply = clients[index]->sendReadRequest(request, 1);

    if (reply) {
        connect(reply, &QModbusReply::finished, this, [this, reply, index]() {
            if (reply->error() == QModbusDevice::NoError) {
                auto values = reply->result().values();
                for (int j = 0; j < values.size() && j < NUM_REGISTERS; ++j) {
                    dataDisplays[index][j]->setText(QString::number(values[j]));
                }
            }
            reply->deleteLater();
            });
    }
}
