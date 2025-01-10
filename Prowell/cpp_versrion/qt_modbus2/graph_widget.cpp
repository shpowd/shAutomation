#include "graph_widget.h"

GraphWidget::GraphWidget(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window); // 독립적인 창으로 설정
    setAttribute(Qt::WA_DeleteOnClose); // 창 닫을 때 자동 삭제
    initSettingUI(); // UI 초기화
    // 부모 창의 크기로 초기화
    if (parent) {
        resize(parent->size());
        move(parent->frameGeometry().topLeft());
    }
    else {
        resize(800, 600); // 부모 창이 없는 경우 기본 크기
    }
    //QPoint parentPos = parent ? parent->geometry().topLeft() : QPoint(100, 100); // 부모 창 좌표
    //move(parentPos); // 새 창의 위치 설정
}

GraphWidget::~GraphWidget()
{
    qDebug() << "GraphWidget destroyed.";
}

void GraphWidget::initSettingUI() {
    // Chart 초기화
    chart = new QChart();
    //chart->setTitle("Dynamic Line Chart");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);

    // Series 초기화
    series = new QLineSeries();
    chart->addSeries(series);

    // X축 설정 (DateTime)
    axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm:ss");
    axisX->setTitleText("Time");

    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    now = QDateTime::fromMSecsSinceEpoch(timestamp);
    QDateTime minTime = now.addSecs(-60); // 현재 시간 - 60초
    QDateTime maxTime = now.addSecs(60);  // 현재 시간 + 60초

    axisX->setTickCount(9); // 틱 수를 9개로 설정 (범위: -60초 ~ +60초)
    axisX->setRange(minTime, maxTime);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);


    // Y축 설정 (Value)
    axisY = new QValueAxis();
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);


    // ChartView 설정
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);


    // 9개의 버튼 추가
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    for (int i = 0; i < 9; ++i) {
        QPushButton* button = new QPushButton(tr("버튼 %1").arg(i + 1), this);
        button->setMinimumHeight(50);
        button->setMinimumWidth(80);
        // 스타일 시트 적용: Windows 기본 회색 배경과 경계선
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #f0f0f0;"  // Windows 기본 회색 배경
            "    border-radius: 10px;"         // 모서리 둥글게
            "    border: 2px solid #d0d0d0;"  // 윈도우 기본 경계선 색 (회색)
            "    color: black;"                // 글자 색
            "    padding: 10px;"               // 버튼 내 여백
            "    font-size: 14px;"             // 글자 크기
            "}"
            "QPushButton:hover {"
            "    background-color: #e0e0e0;"  // 마우스 오버 시 배경색 변경
            "    border: 2px solid #b0b0b0;"  // 마우스 오버 시 경계선 색 변경
            "}"
            "QPushButton:pressed {"
            "    background-color: #d0d0d0;"  // 눌렀을 때 배경색 변경
            "    border: 2px solid #a0a0a0;"  // 눌렀을 때 경계선 색 변경
            "}"
        );

        connect(button, &QPushButton::clicked, this, [this, i]() {
            currentChartIndex = i; // 선택된 차트 인덱스 업데이트
            updateButtonStyles(); // 버튼 스타일 업데이트
            chart->setTitle(tr("차트 %1").arg(i + 1)); // 차트 제목 설정
            });

        buttonLayout->addWidget(button);
        chartButtons.append(button);
    }


    // 우측에 16개의 on/off 텍스트 상자 추가
    QVBoxLayout* textBoxLayout = new QVBoxLayout;
    textBoxLayout->setSpacing(0);                       // 텍스트 상자 사이 간격을 없앰
    textBoxLayout->setContentsMargins(10, 10, 10, 10);  // 레이아웃의 여백을 없앰
    for (int i = 0; i < 16; ++i) {
        QLabel* label = new QLabel(tr("알람 %1").arg(i + 1), this);
        label->setStyleSheet(
            "QLabel {"
            "    background-color: #f0f0f0;"  // 배경색
            "    border: 1px solid #d0d0d0;" // 테두리
            "    border-radius: 5px;"        // 둥근 모서리
            "    color: black;"              // 텍스트 색상
            "    font-size: 14px;"           // 글자 크기
            "    padding: 5px;"              // 텍스트와 경계 간 간격
            "    text-align: center;"        // 중앙 정렬
            "}"
        );
        label->setFixedHeight(35);   // 텍스트 상자의 세로 크기 설정
        label->setMinimumWidth(100); // 텍스트 상자의 가로 크기 설정
        label->setAlignment(Qt::AlignCenter); // 중앙 정렬

        // 텍스트 상자 추가
        textBoxLayout->addWidget(label);
    }
    
    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    textBoxLayout->addSpacerItem(spacer);

    // PNG 이미지 추가 (QPushButton으로 대체)
    QPushButton* imageButton = new QPushButton(this);
    QPixmap pixmap("./src/image.png");  // 이미지 경로
    imageButton->setIcon(QIcon(pixmap));  // 이미지를 아이콘으로 설정
    imageButton->setIconSize(QSize(200, 100));  // 이미지 크기 설정
    imageButton->setStyleSheet("border: none;");  // 버튼 경계선 없애기
    // 클릭 시 창 닫기
    //connect(imageButton, &QPushButton::clicked, this, &MainWidget::openSettingWindow);  

    textBoxLayout->addWidget(imageButton);  // 이미지 버튼 추가


    // Chart 영역 레이아웃
    QVBoxLayout* chartLayout = new QVBoxLayout();
    chartLayout->addWidget(chartView);
    chartLayout->addLayout(buttonLayout);

    // 메인 레이아웃 설정
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(chartLayout, 4); // Chart 영역 (3 비율)
    mainLayout->addLayout(textBoxLayout, 1); // 텍스트 상자 영역 (1 비율)


    setLayout(mainLayout); // 레이아웃을 위젯에 바로 설정

}

void GraphWidget::setChartTitle(const QString& title) {
    if (chart) {
        chart->setTitle(title);
    }
}

void GraphWidget::updateButtonStyles()
{
    for (int i = 0; i < chartButtons.size(); ++i) {
        QPushButton* button = chartButtons[i];
        if (i == currentChartIndex) {
            // 선택된 버튼 스타일
            button->setStyleSheet(
                "QPushButton {"
                "    background-color: #FFA500;"
                "    border-radius: 10px;"
                "    border: 2px solid #FF8C00;"
                "    color: white;"
                "    padding: 10px;"
                "    font-size: 14px;"
                "}"
            );
        }
        else {
            // 기본 버튼 스타일
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
        }
    }
}





void GraphWidget::addDataPoints(qint64 timestamp, const QVector<quint16>& values) {
    // 데이터 추가: timestamp와 values를 함께 저장
    data.append(qMakePair(timestamp, values));

    // 오래된 데이터 제거
    if (data.size() > maxDataSize) {
        data.removeFirst();
    }

    updateData();
}

void GraphWidget::updateData() {
    // 시리즈 업데이트
    series->clear();

    int valueIndex = currentChartIndex + 1; // 버튼 인덱스가 1이라면 values의 두 번째 값, 2라면 세 번째 값
    // 유효한 인덱스인지 확인
    if (valueIndex >= 1) { // values는 최소 1개의 요소가 있어야 함
        for (const auto& entry : data) {
            qint64 timestamp = entry.first; // 데이터의 타임스탬프
            const QVector<quint16>& values = entry.second; // values 배열

            // values 배열에서 valueIndex에 해당하는 값이 있는지 확인
            if (valueIndex < values.size()) {
                double yValue = static_cast<double>(values[valueIndex]);
                series->append(timestamp, yValue); // 시리즈에 데이터 추가
            }
        }
    }
    else {
        qDebug() << "Invalid chart index or value index:" << currentChartIndex;
    }

    // X축 범위 업데이트
    updateXAxisRange();
    // Y축 범위 업데이트
    updateYAxisRange();
}

void GraphWidget::updateXAxisRange() {
    // 데이터의 timestamp 범위로 X축 범위 설정
    if (!data.isEmpty()) {
        qint64 currentTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

        // -60초와 +60초 범위 설정
        QDateTime minTime = QDateTime::fromMSecsSinceEpoch(currentTimestamp - 60 * 1000); // 현재 시간 - 60초
        QDateTime maxTime = QDateTime::fromMSecsSinceEpoch(currentTimestamp + 60 * 1000); // 현재 시간 + 60초

        // X축 범위 설정
        axisX->setRange(minTime, maxTime);

        qDebug() << "Updated X Axis Range:"
            << minTime.toString("hh:mm:ss")
            << "to"
            << maxTime.toString("hh:mm:ss");
    }
}

void GraphWidget::updateYAxisRange() {
    // 최소값과 최대값을 계산
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    for (const auto& point : series->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // 여유 범위 추가
    qreal margin = (maxY - minY) * 0.1;
    if (margin == 0) margin = 1;

    axisY->setRange(minY - margin, maxY + margin);
}

