#include "qt_monitoring.h"
#include "qt_window.h"


// ✅ Monitoring 창 생성 함수
MonitoringWindow::MonitoringWindow(int monitoringIndex, const QMap<int, QMap<QString, QString>>& settings, QWidget* parent)
    : QMainWindow(parent), monitoringIndex(monitoringIndex) {
    setWindowFlags(Qt::Window); // 독립적인 창으로 설정
    setAttribute(Qt::WA_DeleteOnClose); // 창 닫을 때 자동 삭제
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);
    setWindowTitle("Monitoring " + QString::number(monitoringIndex));

    initMonitoringUI(); // UI 초기화
    // 부모 창의 크기로 초기화
    if (parent) {
        qDebug() << "✅ Parent exists! Size:" << parent->size();
        resize(parent->size());
        move(parent->frameGeometry().topLeft());
    }
    else {
        qDebug() << "⚠️ No Parent! Using default size.";
        resize(800, 600);
    }
}


// ✅ Monitoring 창 소멸자
MonitoringWindow::~MonitoringWindow() {
    qDebug() << "❌ Monitoring 창 닫힘, 관련 Graph 창 제거";

}

// ✅ Monitoring 창 소멸 이벤트
void MonitoringWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "❌ Monitoring 창 닫힘, 자식 Graph 창 자동 정리됨";

    // 모든 그래프 창을 안전하게 닫기
    for (auto it = graphWindows.begin(); it != graphWindows.end(); ++it) {
        if (it.value()) {
            it.value()->close();  // 그래프 창 닫기
            it.value()->deleteLater();  // 메모리 해제 예약
        }
    }
    graphWindows.clear();  // 그래프 창 목록 정리

    emit windowClosed(monitoringIndex);  // 메인 윈도우에 알림
    event->accept();
}

// ✅ MonitoringUI 생성 함수
void MonitoringWindow::initMonitoringUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ✅ 하단에 30px 여백 추가
    mainLayout->addSpacing(30);

    // ✅ 상단 Description 레이아웃
    QHBoxLayout* descriptionLayout = new QHBoxLayout;
    descriptionLayout->setSpacing(0);  // ✅ 요소 간 간격 제거
    descriptionLayout->setAlignment(Qt::AlignCenter);  // ✅ 중앙 정렬

    QLabel* descriptionTextLabel = new QLabel("Description:");
    descriptionTextLabel->setFont(QFont("맑은 고딕", 20, QFont::Bold));
    descriptionTextLabel->setStyleSheet("padding-right: 5px;");  // ✅ 오른쪽 여백 최소화

    settings = CSVReader::readAllSettings();
    descriptionLabel = new QLabel(settings.value(monitoringIndex).value("Description", ""));
    descriptionLabel->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setStyleSheet("background-color: #ADB9CA; color: white; border-radius: 10px; padding: 10px; border: 2px solid darkgray;");
    descriptionLabel->setMinimumHeight(50);
    descriptionLabel->setMinimumWidth(600);

    descriptionLayout->addWidget(descriptionTextLabel);
    descriptionLayout->addWidget(descriptionLabel);
    mainLayout->addLayout(descriptionLayout);


    // ✅ 하단에 30px 여백 추가
    mainLayout->addSpacing(60);


    // ✅ 버튼 배치 (2행 4열)
    QGridLayout* buttonLayout = new QGridLayout;
    buttonLayout->setVerticalSpacing(40); 
    QFont buttonFont("맑은 고딕", 20, QFont::Bold);
    QString buttonStyle = "QPushButton {"
        "background-color: #ADB9CA;"
        "color: white; "
        "border: 2px solid darkgray;"
        "border-radius: 10px;"
        "padding: 20px;"
        "font-size: 35px;"
        "} "
        "QPushButton:hover { background-color: #2E75B6; }"
        "QPushButton:pressed { background-color: #FFA500; }";

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 4; ++j) {
            int buttonNumber = i * 4 + j + 1;
            QPushButton* button = new QPushButton(QString::number(buttonNumber));
            button->setFont(buttonFont);
            button->setStyleSheet(buttonStyle);
            button->setFixedSize(200, 120);
            buttonLayout->addWidget(button, i, j);

            // ✅ 버튼 클릭 시 그래프 창 열기
            connect(button, &QPushButton::clicked, this, [this, buttonNumber]() {
                openGraphWindow(buttonNumber);
            });
            buttonLayout->addWidget(button, i, j);

        }
    }
    mainLayout->addLayout(buttonLayout);



    // ✅ 하단에 30px 여백 추가
    mainLayout->addSpacing(120);



    // ✅ 하단 버튼 레이아웃
    QHBoxLayout* bottomLayout = new QHBoxLayout;

    // ✅ Main 버튼 (좌측)
    QPushButton* mainButton = new QPushButton("main");
    mainButton->setFixedSize(150, 60);
    mainButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    applyButtonStyle(mainButton);
    QString currentStyle = mainButton->styleSheet();  // 기존 스타일 가져오기
    mainButton->setStyleSheet(currentStyle + "QPushButton { font-size: 22px; }");
    connect(mainButton, &QPushButton::clicked, this, &QMainWindow::close);
    bottomLayout->addWidget(mainButton);

    // ✅ CSV 설정 버튼 (좌측)
    QPushButton* csvButton = new QPushButton("CSV 설정");
    csvButton->setFixedSize(150, 60);
    csvButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    applyButtonStyle(csvButton);
    currentStyle = csvButton->styleSheet();  // 기존 스타일 가져오기
    csvButton->setStyleSheet(currentStyle + "QPushButton { font-size: 22px; }");
    connect(csvButton, &QPushButton::clicked, this, &MonitoringWindow::openCSVSettingDialog);
    bottomLayout->addWidget(csvButton);

    // ✅ 유동적 간격 추가 (버튼과 로고 사이)
    bottomLayout->addStretch();

    // ✅ 로고 추가 (우측)
    QLabel* logoLabel = new QLabel;
    QPixmap logoPixmap("./src/logo.png");
    logoPixmap = logoPixmap.scaled(150, 75, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    logoLabel->setPixmap(logoPixmap);
    bottomLayout->addWidget(logoLabel);

    mainLayout->addLayout(bottomLayout);
}


// ✅ CSV 설정 창 생성 함수
void MonitoringWindow::openCSVSettingDialog() {
    QDialog* csvDialog = new QDialog(this);
    csvDialog->setWindowTitle("CSV 저장 주기 설정");
    csvDialog->setFixedSize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(csvDialog);

    // ✅ CSV 저장 주기 라벨
    QLabel* titleLabel = new QLabel("CSV 저장 주기", csvDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    layout->addWidget(titleLabel);

    // ✅ CSV 설정용 프레임
    QFrame* csvFrame = new QFrame(csvDialog);
    csvFrame->setStyleSheet("border: 1px solid gray; border-radius: 10px; padding: 10px;");
    QVBoxLayout* csvOptionsLayout = new QVBoxLayout(csvFrame);

    // ✅ CSV 설정용 라디오 버튼 생성 (순서 유지)
    QList<QPair<QString, int>> csvOptions = {
        {"1일", 60*60*24}, {"1주일", 60*60*24*7}, {"1달", 60*60*24*7*30}, {"3개월", 60*60*24*7*30*3}
    };

    QList<QRadioButton*> csvButtons;
    QButtonGroup* csvButtonGroup = new QButtonGroup(csvDialog);

    for (const auto& option : csvOptions) {
        QRadioButton* radioButton = new QRadioButton(option.first, csvFrame);
        csvButtons.append(radioButton);
        csvButtonGroup->addButton(radioButton, option.second);  // ✅ ID 설정
        csvOptionsLayout->addWidget(radioButton);
    }

    csvFrame->setLayout(csvOptionsLayout);
    layout->addWidget(csvFrame);



    // ✅ 현재 모니터링 인덱스에 맞는 CSV 설정 값을 가져와서 반영
    settings = CSVReader::readAllSettings();
    
    for (auto* btn : csvButtons) {
        bool ok;
        int csvRateValue = settings.value(monitoringIndex).value("CSVRate", "").toInt(&ok);
        if (ok && csvButtonGroup->id(btn) == csvRateValue) {
            btn->setChecked(true);
            break;
        }
    }

    // ✅ 저장 및 닫기 버튼 추가
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("저장", csvDialog);
    QPushButton* closeButton = new QPushButton("닫기", csvDialog);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

    // ✅ 저장 버튼 클릭 시 선택된 값 저장
    connect(saveButton, &QPushButton::clicked, this, [=]() {
        int selectedValue = csvButtonGroup->checkedId();
        if (selectedValue != -1) {
            settings[monitoringIndex]["CSVRate"] = QString::number(selectedValue);  // ✅ 선택된 값 저장
            CSVReader::writeAllSettings(settings);  // ✅ CSV 파일 업데이트
            qDebug() << "✅ CSVRate 값 저장됨: " << selectedValue;
        }
        else {
            qDebug() << "⚠ 선택된 CSV 옵션이 없음!";
        }
        csvDialog->accept();
        });

    // ✅ 닫기 버튼 클릭 시 창 닫기
    connect(closeButton, &QPushButton::clicked, csvDialog, &QDialog::reject);

    csvDialog->exec();
}



// ✅ "그래프" 창 생성 함수
void MonitoringWindow::openGraphWindow(int graphIndex) {
    if (graphWindows.contains(graphIndex)) {
        graphWindows[graphIndex]->raise();
        graphWindows[graphIndex]->activateWindow();
        return;
    }

    // ✅ 부모를 'this'로 설정하여 모니터링 창의 자식으로 둠
    GraphWidget* graphWindow = new GraphWidget(graphIndex, this);
    graphWindows[graphIndex] = graphWindow;
    graphWindow->move(this->pos());
    graphWindow->show();

    // ✅ 그래프 창이 닫힐 때 제거
    connect(graphWindow, &GraphWidget::destroyed, this, [this, graphIndex]() {
        graphWindows.remove(graphIndex);
        qDebug() << "✅ Graph 창이 닫혀서 제거됨: " << graphIndex;
    });

}


int MonitoringWindow::getMonitoringIndex() const {
    return monitoringIndex;
}

const QMap<int, QPointer<GraphWidget>>& MonitoringWindow::getGraphWindows() const {
    return graphWindows;
}