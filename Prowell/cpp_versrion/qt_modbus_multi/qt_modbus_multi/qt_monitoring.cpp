#include "qt_monitoring.h"
#include <QDebug>

MonitoringWindow::MonitoringWindow(int rowNumber, const QMap<int, QMap<QString, QString>>& settings, QWidget* parent)
    : QMainWindow(parent), rowNumber(rowNumber) {

    setWindowTitle("Monitoring " + QString::number(rowNumber));
    resize(1024, 768);
    setAttribute(Qt::WA_DeleteOnClose);  // ✅ 창이 닫힐 때 자동으로 메모리 해제

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

    descriptionLabel = new QLabel(settings.value(rowNumber).value("description", "No Description"));
    descriptionLabel->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setStyleSheet("background-color: white; border-radius: 10px; padding: 10px; border: 2px solid darkgray;");
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
        "background-color: #F0F0F0;"
        "border: 2px solid darkgray;"
        "border-radius: 10px;"
        "padding: 20px;"
        "font-size: 24px;"
        "} "
        "QPushButton:hover { background-color: #E0E0E0; }"
        "QPushButton:pressed { background-color: #C8C8C8; }";

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
    QPushButton* mainButton = new QPushButton("Main");
    mainButton->setFixedSize(150, 75);
    mainButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    mainButton->setStyleSheet("QPushButton {"
        "background-color: #EFF4F9;"
        "border: 2px solid darkgray;"
        "border-radius: 10px;"
        "padding: 10px;"
        "} "
        "QPushButton:hover { background-color: #E0E0E0; }"
        "QPushButton:pressed { background-color: #C8C8C8; }");
    connect(mainButton, &QPushButton::clicked, this, &QMainWindow::close);
    bottomLayout->addWidget(mainButton);

    // ✅ CSV 설정 버튼 (좌측)
    QPushButton* csvButton = new QPushButton("CSV 설정");
    csvButton->setFixedSize(150, 75);
    csvButton->setFont(QFont("맑은 고딕", 20, QFont::Normal));
    csvButton->setStyleSheet("QPushButton {"
        "background-color: #EFF4F9;"
        "border: 2px solid darkgray;"
        "border-radius: 10px;"
        "padding: 10px;"
        "} "
        "QPushButton:hover { background-color: #E0E0E0; }"
        "QPushButton:pressed { background-color: #C8C8C8; }");
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



// ✅ Monitoring 창이 닫힐 때 열린 모든 그래프 창도 닫음
void MonitoringWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "🔄 Monitoring 창이 닫혔음 - Row:" << rowNumber;

    // ✅ 열린 모든 Graph 창 닫기
    for (auto graphWindow : graphWindows) {
        graphWindow->close(); // 모든 그래프 창 닫기
    }
    graphWindows.clear(); // ✅ 모든 창을 닫은 후 맵 초기화

    event->accept();
}



// ✅ "그래프" 창 생성 함수
void MonitoringWindow::openGraphWindow(int buttonNumber) {
    if (graphWindows.contains(buttonNumber)) {
        graphWindows[buttonNumber]->raise();
        graphWindows[buttonNumber]->activateWindow();
        return;
    }

    // ✅ 새 그래프 창 생성
    GraphWidget* graphWindow = new GraphWidget(this);
    graphWindow->setWindowFlags(Qt::Window); // 독립 창 설정
    graphWindow->resize(this->size());
    graphWindow->move(this->pos()); // Monitoring 창과 동일한 위치에서 시작
    graphWindow->show();

    // ✅ 창이 닫힐 때 graphWindows에서 제거
    connect(graphWindow, &QObject::destroyed, this, [this, buttonNumber]() {
        graphWindows.remove(buttonNumber);
        });

    graphWindows[buttonNumber] = graphWindow;
    graphWindow->show();
}