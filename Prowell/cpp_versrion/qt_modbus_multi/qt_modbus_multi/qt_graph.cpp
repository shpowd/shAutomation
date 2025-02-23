#include "qt_graph.h"


GraphWidget::GraphWidget(int graphIndex, QWidget* parent) : QWidget(parent), graphIndex(graphIndex) {
    setWindowFlags(Qt::Window); // 독립적인 창으로 설정
    setAttribute(Qt::WA_DeleteOnClose); // 창 닫을 때 자동 삭제
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);
    initSettingUI(); // UI 초기화
    // 부모 창의 크기로 초기화
    if (parent) {
        resize(parent->size());
        move(parent->frameGeometry().topLeft());
    }
    else {
        resize(800, 600); // 부모 창이 없는 경우 기본 크기
    }
}


GraphWidget::~GraphWidget() {
    //qDebug() << "🛑 GraphWidget 닫힘: " << graphIndex;

    //if (series) {
    //    series->deleteLater();
    //    series = nullptr;
    //}
    //if (predictionSeries) {
    //    predictionSeries->deleteLater();
    //    predictionSeries = nullptr;
    //}
    //if (axisX) {
    //    axisX->deleteLater();
    //    axisX = nullptr;
    //}
    //if (axisY) {
    //    axisY->deleteLater();
    //    axisY = nullptr;
    //}
    //if (chart) {
    //    chart->deleteLater();
    //    chart = nullptr;
    //}
    //if (chartView) {
    //    chartView->deleteLater();
    //    chartView = nullptr;
    //}
}



void GraphWidget::initSettingUI() {
    // Chart 초기화
    chart = new QChart();
    //chart->setTitle("Dynamic Line Chart");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);

    // Series 초기화 (실제값, 예측값)
    series = new QLineSeries(); // 실제값
    series->setColor(Qt::black); // 예측값은 빨간색으로 표시
    predictionSeries = new QLineSeries(); // 예측값
    predictionSeries->setColor(Qt::red); // 예측값은 빨간색으로 표시

    chart->addSeries(series);
    chart->addSeries(predictionSeries);

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
    predictionSeries->attachAxis(axisX);


    // Y축 설정 (Value)
    axisY = new QValueAxis();
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    predictionSeries->attachAxis(axisY);


    // ChartView 설정
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);


    // 버튼에 표시할 텍스트 목록
    QStringList buttonTexts = {
        "전류(R)",
        "전류(S)",
        "전류(T)",
        "베어링\n온도",
        "권선\n온도(R)",
        "권선\n온도(S)",
        "권선\n온도(T)",
        "진동",
        "구동\n시간"
    };

    // 버튼에 표시할 Y축 레이블 목록
    QStringList yAxisLabels = {
        "mA",        // 전류(R)
        "mA",        // 전류(S)
        "mA",        // 전류(T)
        "°C",        // 베어링 온도
        "°C",        // 권선 온도(R)
        "°C",        // 권선 온도(S)
        "°C",        // 권선 온도(T)
        "cm/sec²",   // 진동
        "operating time" // 구동 시간
    };

    // 9개의 버튼 추가
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    for (int i = 0; i < 9; ++i) {
        QPushButton* button = new QPushButton(buttonTexts[i], this);
        button->setMinimumHeight(60);
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
            "    background-color: #FFA500;"
            "    border-radius: 10px;"
            "    border: 2px solid #FF8C00;"
            "    color: white;"
            "    padding: 10px;"
            "    font-size: 14px;"
            "}"
        );

        connect(button, &QPushButton::clicked, this, [this, i, buttonTexts, yAxisLabels]() {
            currentChartIndex = i; // 선택된 차트 인덱스 업데이트
            updateButtonStyles(); // 버튼 스타일 업데이트
            
            QString chartTitle = buttonTexts[i]; // 줄바꿈(\n)을 공백으로 대체한 텍스트로 차트 타이틀 설정
            chartTitle.replace("\n", " "); // 줄바꿈을 공백으로 대체
            chart->setTitle(chartTitle);  // 차트 타이틀 설정
            chart->axes(Qt::Vertical).first()->setTitleText(yAxisLabels[i]);// Y축 레이블 업데이트
            updateData(); // 선택된 데이터로 시리즈 업데이트
            });

        buttonLayout->addWidget(button);
        chartButtons.append(button);
    }


    // 우측에 16개의 on/off 텍스트 상자 추가
    QVBoxLayout* textBoxLayout = new QVBoxLayout;
    textBoxLayout->setSpacing(0);                       // 텍스트 상자 사이 간격을 없앰
    textBoxLayout->setContentsMargins(10, 10, 10, 10);  // 레이아웃의 여백을 없앰
    for (int i = 0; i < 16; ++i) {
        QLabel* label = new QLabel(tr("알람%1 OFF").arg(i + 1), this);
        label->setObjectName(QString("alarmLabel_%1").arg(i + 1)); // 라벨에 objectName 설정
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


void GraphWidget::updateButtonStyles()
{
    for (int i = 0; i < chartButtons.size(); ++i) {
        QPushButton* button = chartButtons[i];
        if (i == currentChartIndex) {
            // 선택된 버튼 스타일
            button->setStyleSheet(
                "QPushButton {"
                "    background-color: #2E75B6;" // 호버 상태: 파스텔 블루
                "    border: 2px solid #87ceeb;" // 호버 상태: 밝은 블루 테두리
                "    color: white;"              // 호버 상태: 흰색 텍스트
                "    border-radius: 10px;"
                "    padding: 10px;"
                "    font-size: 14px;"
                "}"
                "QPushButton:pressed {"
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
                "    background-color: #FFA500;"
                "    border-radius: 10px;"
                "    border: 2px solid #FF8C00;"
                "    color: white;"
                "    padding: 10px;"
                "    font-size: 14px;"
                "}"
            );
        }
    }
}



void GraphWidget::addDataPoints(qint64 timestamp, const QVector<quint16>& values) {
    // 실제값 처리
    data.append(qMakePair(timestamp, values.mid(0, 10))); // 0~9번 어드레스 (실제값)

    // 예측값 처리
    predictionData.append(qMakePair(timestamp, values.mid(11, 9))); // 11~19번 어드레스 (예측값)

    // 오래된 데이터 제거
    //if (data.size() > maxDataSize) data.removeFirst();
    //if (predictionData.size() > maxDataSize) predictionData.removeFirst();
    // 오래된 데이터 제거 (X축 범위를 기준으로)
    QDateTime minTime = axisX->min();
    data.erase(std::remove_if(data.begin(), data.end(),
        [&](const QPair<qint64, QVector<quint16>>& entry) {
            return QDateTime::fromMSecsSinceEpoch(entry.first) < minTime;
        }),
        data.end());
    predictionData.erase(std::remove_if(predictionData.begin(), predictionData.end(),
        [&](const QPair<qint64, QVector<quint16>>& entry) {
            return QDateTime::fromMSecsSinceEpoch(entry.first) < minTime;
        }),
        predictionData.end());


    if (currentChartIndex >= 0 && currentChartIndex < 10) {
        updateData();
    }
    else {
        qDebug() << "Invalid chart index for current data range.";
    }
}


void GraphWidget::updateData() {
    series->clear();
    predictionSeries->clear();

    // 실제값 시리즈 업데이트
    for (const auto& entry : data) {
        qint64 timestamp = entry.first;
        const QVector<quint16>& values = entry.second;
        if (currentChartIndex < values.size()) {
            series->append(timestamp, values[currentChartIndex+1]);
        }
    }

    // 예측값 시리즈 업데이트
    for (const auto& entry : predictionData) {
        qint64 timestamp = entry.first;
        QDateTime adjustedTime = QDateTime::fromMSecsSinceEpoch(timestamp);

        // X축 범위에 따라 예측값의 타임스탬프 계산
        switch (rangeType) {
        case 0: adjustedTime = adjustedTime.addSecs(60); break;       // 1분 뒤
        case 1: adjustedTime = adjustedTime.addSecs(3600); break;    // 1시간 뒤
        case 2: adjustedTime = adjustedTime.addSecs(24 * 3600); break; // 1일 뒤
        case 3: adjustedTime = adjustedTime.addMonths(1); break;      // 1달 뒤
        }

        const QVector<quint16>& values = entry.second;
        if (currentChartIndex < values.size()) {
            predictionSeries->append(adjustedTime.toMSecsSinceEpoch(), values[currentChartIndex]); // QDateTime을 다시 밀리초로 변환하여 예측값 추가
        }
    }



    updateXAxisRange();     // X축 범위 업데이트
    updateYAxisRange();     // Y축 범위 업데이트
}


void GraphWidget::setRangeType(int type) {
    if (type < 0 || type > 3) {
        qWarning() << "Invalid range type.";
        return;
    }
    rangeType = type; // 범위 유형 업데이트
    updateXAxisRange(); // 새로운 범위로 X축 업데이트
}


void GraphWidget::updateXAxisRange() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime minTime, maxTime;
    QString format;

    switch (rangeType) {
    case 0: // 1 Minute
        minTime = now.addSecs(-60); // 현재 시간 - 1분
        maxTime = now.addSecs(60);  // 현재 시간 + 1분
        format = "hh:mm:ss"; // 초 단위 시간 표시
        break;
    case 1: // 1 Hour
        minTime = now.addSecs(-3600); // 현재 시간 - 1시간
        maxTime = now.addSecs(3600);  // 현재 시간 + 1시간
        format = "hh:mm"; // 시간 및 분 표시
        break;
    case 2: // 1 Day
        minTime = now.addDays(-1);    // 현재 시간 - 1일
        maxTime = now.addDays(1);     // 현재 시간 + 1일
        format = "MM-dd hh:mm"; // 날짜와 시간 표시
        break;
    case 3: // 1 Month
        minTime = now.addMonths(-1);  // 현재 시간 - 1달
        maxTime = now.addMonths(1);   // 현재 시간 + 1달
        format = "yyyy-MM-dd"; // 연-월-일 표시
        break;
    default:
        qWarning() << "Invalid range type for X-axis update.";
        return;
    }

    // X축 범위 및 라벨 포맷 설정
    axisX->setRange(minTime, maxTime);
    axisX->setFormat(format);

    qDebug() << "Updated X Axis Range: " << minTime.toString("yyyy-MM-dd hh:mm:ss")
        << " to " << maxTime.toString("yyyy-MM-dd hh:mm:ss")
        << ", Label Format: " << format;
}

void GraphWidget::updateYAxisRange() {
    // 최소값과 최대값을 계산
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    // 실제값 시리즈의 범위를 계산
    for (const auto& point : series->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // 예측값 시리즈의 범위를 추가 계산
    for (const auto& point : predictionSeries->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // 여유 범위 추가
    qreal margin = (maxY - minY) * 0.1;
    if (margin == 0) margin = 1; // 여유 범위가 0이면 기본값 설정

    // Y축 범위 설정
    axisY->setRange(minY - margin, maxY + margin);

    qDebug() << "Updated Y Axis Range: min =" << minY - margin << ", max =" << maxY + margin;
}

void GraphWidget::setCurrentChartIndex(int index) {
    currentChartIndex = index;
    updateButtonStyles();
    updateData();
}

