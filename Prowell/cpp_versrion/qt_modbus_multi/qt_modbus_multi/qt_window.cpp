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

    // ✅ "Usage" 맵을 CSV 데이터에서 업데이트
    loadSettingsFromCSV();
    usage.clear();
    for (auto it = settings.begin(); it != settings.end(); ++it) {
        usage[it.key()] = it.value().value("Usage", "0"); // 기본값 "0"
    }
    qDebug() << "🔍 usage 맵 업데이트 완료: " << usage;

    // ✅ Modbus 클라이언트 및 UI 벡터 초기화
    clients.resize(NUM_SLAVES, nullptr);
    comValues.resize(NUM_SLAVES); // ✅ 전체 클라이언트 개수만큼 벡터 크기 설정
    logValues.resize(NUM_SLAVES); // ✅ 전체 클라이언트 개수만큼 벡터 크기 설정


    // ✅ Graph 업데이트 타이머 추가
    graphUpdateTimer = new QTimer(this);
    connect(graphUpdateTimer, &QTimer::timeout, this, &qt_window::updateGraphWidgets);
    graphUpdateTimer->start(graphFreq); // ✅ 2초마다 갱신

    // ✅ 2초마다 실행되는 비동기 타이머 설정
    pollingTimer = new QTimer(this);
    connect(pollingTimer, &QTimer::timeout, this, [this]() {
        QTimer::singleShot(0, this, &qt_window::periodicCommunication); // ✅ 비동기 실행
        });

}

// ✅ showEvent()에서 UI가 완전히 로드된 후 타이머 시작
void qt_window::showEvent(QShowEvent* event) {
    QWidget::showEvent(event); // 부모 클래스 이벤트 호출
    if (pollingTimer) {
        qDebug() << "⏳ 타이머 시작";
        pollingTimer->start(pollingFreq); // ✅ UI가 로드된 후 타이머 시작
    }
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
    if (pollingTimer) {
        pollingTimer->stop(); 
        delete pollingTimer;
        pollingTimer = nullptr;
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
    mainTableWidget->setHorizontalHeaderLabels({ "No", "Description", "monitoring" });


        // 헤더 스타일 설정
    QFont headerFont("맑은 고딕", 18, QFont::Bold);
    mainTableWidget->horizontalHeader()->setFont(headerFont);
    mainTableWidget->horizontalHeader()->setFixedHeight(60);
    mainTableWidget->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   border-top: 2px solid black;"  // ✅ 상단 테두리
        "   border-bottom: 2px solid black;"  // ✅ 하단 테두리
        "   border-left: 1px solid black;"  // ✅ 좌측 테두리 추가
        "   border-right: 1px solid black;"  // ✅ 우측 테두리는 1px로 조정 (이중 테두리 방지)"
        "   background-color: #F0F0F0;"
        "   padding: 5px;"
        "}");
    mainTableWidget->setAlternatingRowColors(true);    // ✅ 번갈아가며 배경색을 적용하도록 설정
    mainTableWidget->setStyleSheet(
        "QTableWidget {"
        "   border: 1px solid black;"
        "   gridline-color: black;"
        "   background-color: #C0C0C0;"  // 기본 배경색
        "} "
        "QTableWidget::item {"
        "   padding: 5px;"
        "} "
        "QTableWidget::item:alternate {"
        "   background-color: #F0F0F0;"  // ✅ 짝수 행 배경색 설정
        "}"
        "QTableWidget::item:first-column {"
        "   border-left: 1px solid black;"  // ✅ 첫 번째 컬럼 좌측에 테두리 추가
        "}");
    mainTableWidget->setShowGrid(true); // ✅ `showGrid(true)` 설정
    mainTableWidget->setFrameShape(QFrame::NoFrame);  // 기본 프레임 제거

    // 열 크기 조정 (자동 조정되도록 설정)
    mainTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // "No" 열은 내용에 맞춤
    mainTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // "Description" 열은 창 크기에 맞춤
    mainTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // "Monitoring" 열도 창 크기에 맞춤
    mainLayout->setStretchFactor(mainTableWidget, 1);

    // 행 크기 조정
    mainTableWidget->verticalHeader()->setVisible(false);
    mainTableWidget->verticalHeader()->setDefaultSectionSize(56); // 각 행(셀)의 높이 증가

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
    openSiteSettingButton->setFixedSize(150, 60); // 버튼 크기
    applyButtonStyle(openSiteSettingButton);  // ✅ 스타일 적용
    QString currentStyle = openSiteSettingButton->styleSheet();  // 기존 스타일 가져오기
    openSiteSettingButton->setStyleSheet(currentStyle + "QPushButton { font-size: 22px; }");
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
        QPushButton* monitorButton = new QPushButton("monitoring");
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
        siteSettingWindow->setWindowIcon(QIcon("./src/icon.png"));


        // ✅ 기본 레이아웃 (qt_window와 동일)
        QVBoxLayout* mainLayout = new QVBoxLayout(siteSettingWindow);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // ✅ 중앙 테이블 (10행 5열)
        siteSettingTableWidget = new QTableWidget(10, 6, siteSettingWindow);
        siteSettingTableWidget->setHorizontalHeaderLabels({ "No", "Description", "통신설정", "사용", "통신 상태", "비고" });

        // ✅ 헤더 스타일 설정
        QFont headerFont("맑은 고딕", 18, QFont::Bold);
        siteSettingTableWidget->horizontalHeader()->setFont(headerFont);
        siteSettingTableWidget->horizontalHeader()->setFixedHeight(60);
        siteSettingTableWidget->horizontalHeader()->setStyleSheet(
            "QHeaderView::section {"
            "   border-top: 2px solid black;"  // ✅ 상단 테두리
            "   border-bottom: 2px solid black;"  // ✅ 하단 테두리
            "   border-left: 1px solid black;"  // ✅ 좌측 테두리 추가
            "   border-right: 1px solid black;"  // ✅ 우측 테두리는 1px로 조정 (이중 테두리 방지)"
            "   background-color: #F0F0F0;"
            "   padding: 5px;"
            "}");
        siteSettingTableWidget->setAlternatingRowColors(true);    // ✅ 번갈아가며 배경색을 적용하도록 설정
        siteSettingTableWidget->setStyleSheet(
            "QTableWidget {"
            "   border: 1px solid black;"
            "   gridline-color: black;"
            "   background-color: #C0C0C0;"  // 기본 배경색
            "} "
            "QTableWidget::item {"
            "   padding: 5px;"
            "} "
            "QTableWidget::item:alternate {"
            "   background-color: #F0F0F0;"  // ✅ 짝수 행 배경색 설정
            "}"
            "QTableWidget::item:first-column {"
            "   border-left: 1px solid black;"  // ✅ 첫 번째 컬럼 좌측에 테두리 추가
            "}");
        siteSettingTableWidget->setShowGrid(true); // ✅ `showGrid(true)` 설정
        siteSettingTableWidget->setFrameShape(QFrame::NoFrame);  // 기본 프레임 제거

        siteSettingTableWidget->setAlternatingRowColors(true);
        QPalette pal = siteSettingTableWidget->palette();
        pal.setColor(QPalette::Base, QColor("#C0C0C0"));
        pal.setColor(QPalette::AlternateBase, QColor("#F0F0F0"));
        siteSettingTableWidget->setPalette(pal);

        // ✅ 열 크기 조정
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        siteSettingTableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

        siteSettingTableWidget->verticalHeader()->setVisible(false);
        siteSettingTableWidget->verticalHeader()->setDefaultSectionSize(56);
        mainLayout->addWidget(siteSettingTableWidget);


        // ✅ 표와 버튼 레이아웃 사이 여백
        mainLayout->addSpacing(20);


        // ✅ 하단 버튼 레이아웃 (Main 버튼, Save 버튼, 페이지 전환 버튼, 로고)
        QHBoxLayout* bottomLayout = new QHBoxLayout;
        mainLayout->addLayout(bottomLayout);

        // ✅ Main 버튼 (좌측)
        QPushButton* mainButton = new QPushButton("Main");
        mainButton->setFixedSize(150, 60);
        mainButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
        applyButtonStyle(mainButton);
        QString currentStyle = mainButton->styleSheet();  // 기존 스타일 가져오기
        mainButton->setStyleSheet(currentStyle + "QPushButton { font-size: 22px; }");
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
        saveButton->setFixedSize(150, 60);
        saveButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
        applyButtonStyle(saveButton);
        currentStyle = saveButton->styleSheet();  // 기존 스타일 가져오기
        saveButton->setStyleSheet(currentStyle + "QPushButton { font-size: 22px; }");
        bottomLayout->addWidget(saveButton);
        connect(saveButton, &QPushButton::clicked, this, &qt_window::siteSettingWindowSave);

        // ✅ 유동적 간격 추가 (버튼과 페이지 버튼 사이)
        bottomLayout->addSpacerItem(new QSpacerItem(100, 10, QSizePolicy::Minimum, QSizePolicy::Minimum));

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

        // ✅ CSV에서 불러온 값 적용
        loadSettingsFromCSV();                                                                  // ✅ CSV 데이터 로드
        QString descriptionText = settings.value(monitoringIndex).value("Description", "");     // ✅ 필드
        QString notesText = settings.value(monitoringIndex).value("Notes", "");
        QString usageValue = settings.value(monitoringIndex).value("Usage", "0");               // ✅ 기본값 "0"

        QTableWidgetItem* itemNo = new QTableWidgetItem(QString::number(monitoringIndex));
        QTableWidgetItem* description = new QTableWidgetItem(descriptionText);
        QTableWidgetItem* commState = new QTableWidgetItem("-");
        QTableWidgetItem* notes = new QTableWidgetItem(notesText);

        // ✅ "사용 여부" 체크박스 추가 (QCheckBox 유지)
        QWidget* checkBoxWidget = new QWidget();
        QHBoxLayout* checkBoxLayout = new QHBoxLayout(checkBoxWidget);
        QCheckBox* checkBox = new QCheckBox();
        checkBox->setChecked(usageValue == "1");  // ✅ "1"이면 체크, 아니면 해제

        checkBoxLayout->addWidget(checkBox);
        checkBoxLayout->setAlignment(Qt::AlignCenter);  // ✅ 가운데 정렬
        checkBoxLayout->setContentsMargins(0, 0, 0, 0);
        checkBoxWidget->setLayout(checkBoxLayout);

        // ✅ 체크박스 상태 변경 시 `settings`의 "Usage" 필드 업데이트
        connect(checkBox, &QCheckBox::checkStateChanged, this, [this, monitoringIndex](int state) {
            settings[monitoringIndex]["Usage"] = (state == Qt::Checked) ? "1" : "0";
            qDebug() << "✅ Usage 업데이트됨 (Index:" << monitoringIndex << ") =>" << settings[monitoringIndex]["Usage"];
            });


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
        QPushButton* commButton = new QPushButton("통신 설정");
        applyButtonStyle(commButton);
        // ✅ 버튼 클릭 시 "통신 설정" 팝업 창 열기
        connect(commButton, &QPushButton::clicked, this, [this, monitoringIndex]() {
            openCommSettingsWindow(monitoringIndex);
            });


        // ✅ 아이템 및 버튼 추가
        siteSettingTableWidget->setItem(i, 0, itemNo);
        siteSettingTableWidget->setItem(i, 1, description);
        siteSettingTableWidget->setCellWidget(i, 2, commButton);
        siteSettingTableWidget->setCellWidget(i, 3, checkBoxWidget);  // ✅ 체크박스 추가
        siteSettingTableWidget->setItem(i, 4, commState);
        siteSettingTableWidget->setItem(i, 5, notes);
    }
}

// ✅ "현장 설정" 페이지 SAVE 버튼 함수
void qt_window::siteSettingWindowSave() {
    for (int i = 0; i < 10; ++i) {
        int monitoringIndex = (currentSiteSettingpPage == 1) ? i + 1 : i + 11;

        QTableWidgetItem* descriptionItem = siteSettingTableWidget->item(i, 1);
        QTableWidgetItem* notesItem = siteSettingTableWidget->item(i, 5);
        QWidget* checkBoxWidget = siteSettingTableWidget->cellWidget(i, 3);  // ✅ 체크박스 가져오기
        QCheckBox* checkBox = checkBoxWidget ? checkBoxWidget->findChild<QCheckBox*>() : nullptr;


        QString description = descriptionItem ? descriptionItem->text() : "";
        QString notes = notesItem ? notesItem->text() : "";
        QString usageValue = (checkBox && checkBox->isChecked()) ? "1" : "0";  // ✅ 체크 여부 확인

        // ✅ settings 업데이트 시 필드명 일치
        settings[monitoringIndex]["Description"] = description;
        settings[monitoringIndex]["Usage"] = usageValue;
        settings[monitoringIndex]["Notes"] = notes;

        // ✅ usage 맵 업데이트 추가
        usage[monitoringIndex] = usageValue;
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





// 통신 함수
// ✅ 주기적으로 실행될 통신 함수 (비동기 실행)
void qt_window::periodicCommunication() {
    qDebug() << "⏳ periodicCommunication 실행됨"; // ✅ 디버깅 로그 추가
    loadSettingsFromCSV();
    for (int clientIndex = 0; clientIndex < NUM_SLAVES; ++clientIndex) { // ✅ 인덱스 0부터 시작
        int monitoringIndex = clientIndex + 1; // ✅ monitoringIndex는 1부터 시작

        if (usage.value(monitoringIndex, "0") == "1") { // ✅ Usage가 "1"인 경우만 통신 수행
            qDebug() << "📡 사용중인 장치: " << monitoringIndex; // ✅ 사용중인 장치 확인

            // ✅ 벡터 접근 시 크기 체크
            if (clientIndex >= clients.size()) {
                qDebug() << "❌ clients 인덱스 초과: " << clientIndex;
                continue;
            }

            if (!clients[clientIndex]) {  // ✅ Null 체크 추가
                clients[clientIndex] = new QModbusTcpClient(this);
            }

            if (clients[clientIndex]->state() == QModbusDevice::UnconnectedState) {
                qDebug() << "🔗 장치 " << monitoringIndex << " 연결 시도";
                connectToSlave(clientIndex); // ✅ clientIndex 사용
            }
            else {
                qDebug() << "📨 장치 " << monitoringIndex << " 데이터 읽기";
                readFromSlave(clientIndex);             // ✅ clientIndex 사용
            }

        }
        else {
            if (clients[clientIndex] && clients[clientIndex]->state() == QModbusDevice::ConnectedState) {
                qDebug() << "❌ 장치 " << monitoringIndex << " 연결 해제";
                disconnectFromSlave(clientIndex); // ✅ clientIndex 사용
            }
        }
    }

}

// ✅ Modbus 연결 함수
void qt_window::connectToSlave(int clientIndex) {
    if (clientIndex < 0 || clientIndex >= NUM_SLAVES) return; // ✅ clientIndex 범위 검증

    int monitoringIndex = clientIndex + 1; // ✅ monitoringIndex 1부터 시작

    // ✅ 현재 상태 확인
    if (clients[clientIndex]) {
        QModbusDevice::State currentState = clients[clientIndex]->state();

        // ✅ 연결이 진행 중이거나 이미 연결된 경우, 다시 연결하지 않음
        if (currentState != QModbusDevice::UnconnectedState) {
            return; // ✅ 불필요한 재연결 방지
        }
    }

    QString ip = settings.value(monitoringIndex).value("IP", "127.0.0.1");
    int port = settings.value(monitoringIndex).value("Port", "502").toInt();

    // ✅ 기존 클라이언트 객체가 있으면 삭제 (연결 실패한 경우 대비)
    if (clients[clientIndex]) {
        clients[clientIndex]->disconnectDevice();
        delete clients[clientIndex];
        clients[clientIndex] = nullptr;
    }
    // ✅ 새로운 Modbus TCP 클라이언트 객체 생성
    clients[clientIndex] = new QModbusTcpClient(this);

    clients[clientIndex]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    clients[clientIndex]->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);


    connect(clients[clientIndex], &QModbusTcpClient::stateChanged, this, [this, clientIndex](QModbusDevice::State state) {
        updateStatus(clientIndex, state);
        qDebug() << "📡 Modbus 상태 변경 (clientIndex " << clientIndex << "): " << state;
    });

    clients[clientIndex]->connectDevice();
}

// ✅ 연결 상태 업데이트 함수
void qt_window::updateStatus(int clientIndex, QModbusDevice::State state) {
    if (clientIndex < 0 || clientIndex >= NUM_SLAVES) return; // ✅ clientIndex 범위 검증

    int monitoringIndex = clientIndex + 1; // ✅ monitoringIndex 변환 (1부터 시작)

    QString statusText;
    QString style;

    switch (state) {
    case QModbusDevice::UnconnectedState:
        statusText = "Disconnected";
        style = "border: 1px solid gray; padding: 2px; background-color: lightgray;";
        break;
    case QModbusDevice::ConnectingState:
        statusText = "Connecting...";
        style = "border: 1px solid gray; padding: 2px; background-color: yellow;";
        break;
    case QModbusDevice::ConnectedState:
        statusText = "Connected";
        style = "border: 1px solid gray; padding: 2px; background-color: green;";
        break;
    case QModbusDevice::ClosingState:
        statusText = "Closing...";
        style = "border: 1px solid gray; padding: 2px; background-color: orange;";
        break;
    }

    // ✅ 현장 설정 창이 열려 있을 때 "통신 상태" 업데이트
    if (siteSettingWindow && siteSettingTableWidget) {
        for (int i = 0; i < 10; ++i) { // ✅ 현재 보이는 10개 행만 갱신
            int rowMonitoringIndex = (currentSiteSettingpPage == 1) ? i + 1 : i + 11;

            if (rowMonitoringIndex == monitoringIndex) { // ✅ 일치하는 경우만 업데이트
                QTableWidgetItem* statusItem = siteSettingTableWidget->item(i, 4);
                if (!statusItem) {
                    statusItem = new QTableWidgetItem();
                    siteSettingTableWidget->setItem(i, 4, statusItem);
                }
                statusItem->setText(statusText); // ✅ 상태 업데이트
                break; // ✅ 더 이상 반복할 필요 없음
            }
        }
    }

    qDebug() << "🔄 상태 업데이트 (Client Index: " << clientIndex << ", Monitoring Index: " << monitoringIndex << "): " << statusText;
}

// ✅ Modbus 연결 해제 함수
void qt_window::disconnectFromSlave(int clientIndex) {
    if (clientIndex < 0 || clientIndex >= NUM_SLAVES) return; // ✅ clientIndex 범위 검증

    int monitoringIndex = clientIndex + 1; // ✅ monitoringIndex 1부터 시작

    if (clientIndex >= clients.size() || !clients[clientIndex]) return;

    clients[clientIndex]->disconnectDevice();
    delete clients[clientIndex];
    clients[clientIndex] = nullptr;
    updateStatus(monitoringIndex, QModbusDevice::UnconnectedState);
    qDebug() << "🔌 Modbus 연결 해제됨 (Index " << monitoringIndex << ")";
}

// ✅ 데이터 읽기 & log 데이터 저장
void qt_window::readFromSlave(int clientIndex) {
    if (clientIndex < 0 || clientIndex >= NUM_SLAVES) return; // ✅ clientIndex 범위 검증
    if (clientIndex >= clients.size() || !clients[clientIndex] || clients[clientIndex]->state() != QModbusDevice::ConnectedState) return;

    int monitoringIndex = clientIndex + 1; // ✅ monitoringIndex 1부터 시작
    int modbusID = settings.value(monitoringIndex).value("Modbus ID", "1").toInt(); // ✅ Modbus ID 추가
    maxComValuesSize = settings.value(monitoringIndex).value("CSVRate", "1").toInt()/10;
    logSaveInterval = maxComValuesSize;

    QModbusDataUnit request(QModbusDataUnit::HoldingRegisters, 1000, NUM_REGISTERS);
    QModbusReply* reply = clients[clientIndex]->sendReadRequest(request, modbusID);    
    if (!reply) {
        qWarning() << "❌ Modbus 읽기 요청 실패 (clientIndex:" << clientIndex << ")";
        return;
    }

    if (reply) {
        connect(reply, &QModbusReply::finished, this, [this, reply, clientIndex, monitoringIndex]() {
            if (reply->error() == QModbusDevice::NoError) {
                auto values = reply->result().values();
                QDateTime currentTime = QDateTime::currentDateTime();

                // ✅ comValues에 그래프 데이터 저장
                comValues[clientIndex].append(qMakePair(currentTime, values)); // ✅ 올바른 `.append()` 사용

                // ✅ comValues 크기 제한
                if (comValues[clientIndex].size() > maxComValuesSize) {
                    comValues[clientIndex].removeFirst();
                }
                qDebug() << "📊 데이터 업데이트 (Client Index: " << clientIndex << ")"
                    << "Timestamp:" << currentTime.toString(Qt::ISODate)
                    << "Values:" << values;

                // ✅ logInterval 마다 logValues에 저장
                if (logInterval >0 && logCounter % logInterval == 0) {
                    logValues[clientIndex].append({ QDateTime::currentDateTime(), values });
                    qDebug() << "📊 log 데이터 저장";
                }


                // ✅ logSaveInterval 마다 CSV에 저장
                if (logSaveInterval>0 && logCounter % logSaveInterval == 0) {
                    CSVReader::writeAllCSVlog(logValues[clientIndex], monitoringIndex);
                    logValues[clientIndex].clear();  // 해당 monitoringIndex의 데이터만 초기화
                    qDebug() << "📊 log CSV 저장";
                }

                logCounter++;

            }
            reply->deleteLater();
        });
    }
}




// 그래프 업데이트 함수
void qt_window::updateGraphWidgets() {
    if (monitoringWindows.isEmpty()) return; // ✅ 모니터링 창이 없으면 실행 안 함

    for (auto& monitoringWindow : monitoringWindows) {
        if (!monitoringWindow.isNull()) {
            const auto& graphWidgets = monitoringWindow->getGraphWindows();
            if (graphWidgets.isEmpty()) continue; // ✅ 그래프 창이 없으면 실행 안 함 (monitoringWindows와 동일한 방식)

            int clientIndex = monitoringWindow->getMonitoringIndex() - 1;

            if (clientIndex < 0 || clientIndex >= comValues.size()) {
                qDebug() << "⚠️ 유효하지 않은 clientIndex: " << clientIndex << " (comValues 크기: " << comValues.size() << ")";
                continue; // ❌ 잘못된 인덱스일 경우 실행하지 않음
            }

            if (comValues[clientIndex].isEmpty()) {
                qDebug() << "⚠️ comValues[" << clientIndex << "] 데이터 없음";
                continue;
            }

            qDebug() << "📊 그래프 업데이트 준비 (Client Index: " << clientIndex << ")";
            qDebug() << "   📌 기록 개수: " << comValues[clientIndex].size();

            // ✅ 그래프 윈도우가 열려 있는 경우, comValues[clientIndex] 자체를 전달
            for (auto& graphWindow : graphWidgets) {
                if (!graphWindow.isNull()) {
                    qDebug() << "📈 그래프 업데이트 시작: " << clientIndex;
                    graphWindow->updateGraphData(comValues[clientIndex]);  // ✅ 데이터 전체 전달
                    qDebug() << "📊 그래프 데이터 업데이트 완료 (Client Index: " << clientIndex
                        << ", 데이터 개수: " << comValues[clientIndex].size() << ")";
                }
                else {
                    qDebug() << "⚠️ graphWindow가 null 상태";
                }
            }

        }
    }
}




// 로그 저장 함수
// ✅ CSV log 저장 함수
void qt_window::logSave(int clientIndex, const QVector<quint16>& values, const QDateTime& timestamp) {
    QString filePath = "modbus_log.csv"; // ✅ CSV 파일 이름
    QFile file(filePath);
    bool fileExists = file.exists();

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        // ✅ 파일이 처음 생성될 경우 헤더 추가
        if (!fileExists) {
            out << "Timestamp,Client Index";
            for (int i = 0; i < values.size(); ++i) {
                out << ",Register " << i;
            }
            out << "\n";
        }

        // ✅ 데이터 추가
        out << timestamp.toString(Qt::ISODate) << "," << clientIndex;
        for (quint16 value : values) {
            out << "," << value;
        }
        out << "\n";

        file.close();
        qDebug() << "✅ CSV 로그 저장 완료: " << filePath;
    }
    else {
        qWarning() << "❌ CSV 파일 열기 실패: " << filePath;
    }
}