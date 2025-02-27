#include "qt_graph.h"
#include "qt_window.h"


// ✅ Graph 창 생성 함수
GraphWidget::GraphWidget(int graphIndex, QWidget* parent)
    : QWidget(parent), graphIndex(graphIndex) {
    setWindowFlags(Qt::Window); // 독립적인 창으로 설정
    setAttribute(Qt::WA_DeleteOnClose); // 창 닫을 때 자동 삭제
    QIcon icon("./src/icon.png");
    setWindowIcon(icon);
    setWindowTitle(QString("모터%1 예측 그래프").arg(graphIndex));

    initGraphUI(); // UI 초기화
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

// ✅ Graph 창 소멸자
GraphWidget::~GraphWidget() {

}



// ✅ GraphUI 생성 함수
void GraphWidget::initGraphUI() {
    // Chart 초기화
    chart = new QChart();
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);

    // Series 초기화 (실제값, 예측값)
    series = new QLineSeries(); // 실제값
    series->setColor(Qt::black); // 실제값은 검정색으로 표시
    predictionSeries = new QLineSeries(); // 예측값
    predictionSeries->setColor(Qt::red); // 예측값은 빨간색으로 표시
    chart->addSeries(series);
    chart->addSeries(predictionSeries);

    // ✅ X축 설정을 별도의 함수로 관리
    axisX = new QDateTimeAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    predictionSeries->attachAxis(axisX);

    // ✅ X축 업데이트 함수 호출 (초기화 시점에서 자동 적용)
    updateXAxisRange();



    // Y축 설정 (Value)
    axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    predictionSeries->attachAxis(axisY);

    // ChartView 설정
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);


    // ▶ 하단 버튼 추가
    QStringList buttonTexts = {
        "전류(R)",
        "전류(S)",
        "전류(T)",
        "진동",
        "베어링\n온도",
        "권선\n온도"
    };

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    for (int i = 0; i < buttonTexts.size(); ++i) {
        QPushButton* button = new QPushButton(buttonTexts[i], this);
        button->setMinimumWidth(100);
        button->setMinimumHeight(55);
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 자동 크기 조정 방지
        // ✅ 공통 버튼 스타일 적용
        applyButtonStyle(button);

        connect(button, &QPushButton::clicked, this, [this, i, buttonTexts]() {
            currentChartIndex = i; // 선택된 차트 인덱스 업데이트
            updateButtonStyles(); // 버튼 스타일 업데이트
            
            QString graphTitle = buttonTexts[i];                                                // 줄바꿈(\n)을 공백으로 대체한 텍스트로 차트 타이틀 설정
            graphTitle.replace("\n", " ");                                                      // 줄바꿈을 공백으로 대체
            setWindowTitle(QString("모터%1 %2 예측 그래프").arg(graphIndex).arg(graphTitle));     // ✅ 타이틀을 "모터X 전류(S) 예측 그래프" 형식으로 변경

            //chart->axes(Qt::Vertical).first()->setTitleText(yAxisLabels[i]);// Y축 레이블 업데이트
            updateData(); // 선택된 데이터로 시리즈 업데이트
            });

        buttonLayout->addWidget(button);
        chartButtons.append(button);
    }



    // Chart 영역 레이아웃
    QVBoxLayout* chartLayout = new QVBoxLayout();
    chartLayout->addWidget(chartView);
    chartLayout->addLayout(buttonLayout);


    // ▶ 우측 레이아웃 (알람 + 추가 UI 요소)
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(10, 10, 10, 10);


    // ▶ "Monitoring" 버튼 (그래프 창 닫기)
    QPushButton* monitoringButton = new QPushButton("Monitoring", this);
    monitoringButton->setFixedSize(140, 60); // ✅ 크기 설정
    applyButtonStyle(monitoringButton);
    connect(monitoringButton, &QPushButton::clicked, this, &GraphWidget::close);
    rightLayout->addWidget(monitoringButton, 0, Qt::AlignCenter);



    // ▶ 그래프 설정 위쪽 Expanding 스페이서 추가
    rightLayout->addSpacing(5);


    // ✅ 알람 텍스트 설정 (2줄 형식)
    QStringList alarmTexts = {
        "R Phase Current\nAlarm",
        "S Phase Current\nAlarm",
        "T Phase Current\nAlarm",
        "Vibration\nAlarm",
        "Winding Temperature\nAlarm",
        "Bearing Temperature\nAlarm"
    };

    // ▶ 알람 텍스트 6개 추가
    QVBoxLayout* arlamBoxLayout = new QVBoxLayout;
    arlamBoxLayout->setSpacing(3);                          // 텍스트 상자 사이 간격을 없앰
    arlamBoxLayout->setContentsMargins(10, 10, 10, 10);     // 레이아웃의 여백을 없앰

    for (int i = 0; i < 6; ++i) {
        QLabel* alarmLabel = new QLabel(alarmTexts[i], this);
        alarmLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        alarmLabel->setAlignment(Qt::AlignCenter);
        alarmLabel->setStyleSheet("background-color: #ADB9CA; border: 1px solid #d0d0d0; border-radius: 22px; color: white; font-size: 14px; padding: 5px;");
        applyShadowEffect(alarmLabel);
        arlamBoxLayout->addWidget(alarmLabel);
    }
    rightLayout->addLayout(arlamBoxLayout);



    // ▶ 그래프 설정 위쪽 Expanding 스페이서 추가
    rightLayout->addSpacing(15);


    // ▶ "그래프 설정" 버튼 추가
    QPushButton* settingButton = new QPushButton("그래프 설정", this);
    settingButton->setFixedSize(140, 60); // ✅ 크기 설정
    applyButtonStyle(settingButton);
    connect(settingButton, &QPushButton::clicked, this, &GraphWidget::openGraphSettings);
    rightLayout->addWidget(settingButton, 0, Qt::AlignCenter);


    // ▶ 그래프 설정 아래쪽 Expanding 스페이서 추가
    QSpacerItem* Spacer3 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightLayout->addItem(Spacer3);



    // ▶ "운전 시간" 컨테이너 생성 (테두리 역할)
    QFrame* operatingTimeFrame = new QFrame(this);
    operatingTimeFrame->setMinimumHeight(140); // ✅ 기본 최소 높이 설정 (필요시 조정 가능)

    // ✅ 버튼 및 레이블과 동일한 스타일 적용 (테두리 역할)
    operatingTimeFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ADB9CA;"  // 배경색
        "    border: 1px solid #d0d0d0;"  // 테두리
        "    border-radius: 10px;"        // 둥근 모서리
        "}"
    );
    applyShadowEffect(operatingTimeFrame); // ✅ 그림자 효과 추가

    // ▶ "운전 시간" 입력 필드를 포함하는 내부 레이아웃 생성 (위아래 정렬)
    QVBoxLayout* operatingTimeLayout = new QVBoxLayout(operatingTimeFrame);
    operatingTimeLayout->setContentsMargins(10, 10, 10, 10); // 내부 여백 조정
    operatingTimeLayout->setSpacing(5); // 시간과 분 사이 간격 설정

    // ▶ "운전 시간" 입력 필드 (텍스트 박스)
    QHBoxLayout* hourLayout = new QHBoxLayout;
    QLineEdit* operatingHourInput = new QLineEdit(operatingTimeFrame);
    operatingHourInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    operatingHourInput->setAlignment(Qt::AlignCenter);
    operatingHourInput->setPlaceholderText("시간");
    operatingHourInput->setValidator(new QIntValidator(0, 999999, this)); // 0 ~ 999999시간 제한

    // ▶ "hours" 고정 텍스트 레이블
    QLabel* hoursLabel = new QLabel("hours", operatingTimeFrame);
    hoursLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hoursLabel->setAlignment(Qt::AlignCenter);
    hoursLabel->setStyleSheet("border: none; color: white;"); // ✅ 테두리 제거


    // ▶ "운전 시간" 수평 레이아웃에 추가
    hourLayout->addWidget(operatingHourInput, 2);
    hourLayout->addWidget(hoursLabel, 1);

    // ▶ "운전 분" 입력 필드 (텍스트 박스)
    QHBoxLayout* minuteLayout = new QHBoxLayout;
    QLineEdit* operatingMinuteInput = new QLineEdit(operatingTimeFrame);
    operatingMinuteInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    operatingMinuteInput->setAlignment(Qt::AlignCenter);
    operatingMinuteInput->setPlaceholderText("분");
    operatingMinuteInput->setValidator(new QIntValidator(0, 59, this)); // 0 ~ 59분 제한

    // ▶ "min" 고정 텍스트 레이블
    QLabel* minuteLabel = new QLabel("mins", operatingTimeFrame);
    minuteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    minuteLabel->setAlignment(Qt::AlignCenter);
    minuteLabel->setStyleSheet("border: none; color: white;"); // ✅ 테두리 제거

    // ▶ "운전 분" 수평 레이아웃에 추가
    minuteLayout->addWidget(operatingMinuteInput, 2);
    minuteLayout->addWidget(minuteLabel, 1);

    // ▶ "운전 시간"과 "운전 분"을 세로 정렬하여 컨테이너에 추가
    operatingTimeLayout->addLayout(hourLayout);
    operatingTimeLayout->addLayout(minuteLayout);

    // ▶ "운전 시간" 컨테이너를 우측 레이아웃에 추가
    rightLayout->addWidget(operatingTimeFrame);




    // ▶ 그래프 설정 아래쪽 Expanding 스페이서 추가
    QSpacerItem* Spacer4 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    rightLayout->addItem(Spacer4);




    // ▶ logo.png 추가
    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPixmap("./src/logo.png");
    logoLabel->setPixmap(logoPixmap.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(logoLabel);


    // 메인 레이아웃 설정
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    int mainspacing = 15;
    mainLayout->setContentsMargins(mainspacing, mainspacing, mainspacing, mainspacing); // ✅ 창 전체 여백 추가
    mainLayout->addLayout(chartLayout, 4); // Chart 영역 (3 비율)
    mainLayout->addLayout(rightLayout, 1); // 텍스트 상자 영역 (1 비율)


    setLayout(mainLayout); // 레이아웃을 위젯에 바로 설정

}


// ✅ Graph 창 하단 버튼 스타일 함수
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
            // ✅ 공통 버튼 스타일 적용
            applyButtonStyle(button);
        }
    }
}


// ✅ 그래프 설정 창 생성 함수
void GraphWidget::openGraphSettings() {
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("그래프 설정");
    settingsDialog.setFixedSize(350, 300); // 팝업 크기

    QVBoxLayout* mainLayout = new QVBoxLayout(&settingsDialog);

    // ▶ X축 설정 (테두리 적용 X)
    QFrame* xAxisFrame = new QFrame(&settingsDialog);
    xAxisFrame->setStyleSheet("QFrame { border: none; background-color: #F0F0F0; }");
    xAxisFrame->setFixedHeight(100); // ✅ Y축과 동일한 높이 설정
    applyShadowEffect(xAxisFrame);
    QVBoxLayout* xAxisLayout = new QVBoxLayout(xAxisFrame);
    xAxisLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* xAxisTitle = new QLabel("그래프 X축", xAxisFrame);
    xAxisTitle->setAlignment(Qt::AlignCenter);
    xAxisTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout* xAxisOptionsLayout = new QHBoxLayout;
    QButtonGroup* xAxisButtonGroup = new QButtonGroup(&settingsDialog);

    QStringList xAxisOptions = { "1분", "1시간", "1일", "1주", "1달" };
    QList<QRadioButton*> xAxisButtons;

    for (int i = 0; i < xAxisOptions.size(); ++i) {
        QRadioButton* radioButton = new QRadioButton(xAxisOptions[i], xAxisFrame);
        xAxisButtons.append(radioButton);
        xAxisButtonGroup->addButton(radioButton, i);
        xAxisOptionsLayout->addWidget(radioButton);
    }

    // ✅ rangeType 기본 선택
    if (rangeType >= 0 && rangeType < xAxisButtons.size()) {
        xAxisButtons[rangeType]->setChecked(true);
    }

    xAxisLayout->addWidget(xAxisTitle);
    xAxisLayout->addLayout(xAxisOptionsLayout);
    mainLayout->addWidget(xAxisFrame);

    // ▶ Y축 설정 (테두리 적용 X)
    QFrame* yAxisFrame = new QFrame(&settingsDialog);
    yAxisFrame->setStyleSheet("QFrame { border: none; background-color: #F0F0F0; }");
    yAxisFrame->setFixedHeight(100); // ✅ X축과 동일한 높이 설정
    applyShadowEffect(yAxisFrame);
    QVBoxLayout* yAxisLayout = new QVBoxLayout(yAxisFrame);
    yAxisLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* yAxisTitle = new QLabel("그래프 Y축", yAxisFrame);
    yAxisTitle->setAlignment(Qt::AlignCenter);
    yAxisTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout* yAxisMinMaxLayout = new QHBoxLayout;

    // ▶ 최소값 입력
    QVBoxLayout* yMinLayout = new QVBoxLayout;
    QLabel* yMinLabel = new QLabel("최소값", yAxisFrame);
    QLineEdit* yAxisMinInput = new QLineEdit(yAxisFrame);
    yAxisMinInput->setValidator(new QIntValidator(yAxisFrame));
    yAxisMinInput->setText(QString::number(axisY->min()));
    yMinLayout->addWidget(yMinLabel);
    yMinLayout->addWidget(yAxisMinInput);

    // ▶ 최대값 입력
    QVBoxLayout* yMaxLayout = new QVBoxLayout;
    QLabel* yMaxLabel = new QLabel("최대값", yAxisFrame);
    QLineEdit* yAxisMaxInput = new QLineEdit(yAxisFrame);
    yAxisMaxInput->setValidator(new QIntValidator(yAxisFrame));
    yAxisMaxInput->setText(QString::number(axisY->max()));
    yMaxLayout->addWidget(yMaxLabel);
    yMaxLayout->addWidget(yAxisMaxInput);

    yAxisMinMaxLayout->addLayout(yMinLayout);
    yAxisMinMaxLayout->addLayout(yMaxLayout);

    yAxisLayout->addWidget(yAxisTitle);
    yAxisLayout->addLayout(yAxisMinMaxLayout);
    mainLayout->addWidget(yAxisFrame);

    // ▶ 저장 및 닫기 버튼
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("저장", &settingsDialog);
    QPushButton* closeButton = new QPushButton("닫기", &settingsDialog);

    connect(saveButton, &QPushButton::clicked, [&]() {
        // X축 주기 반영 (1분, 1시간, 1일, 1주, 1달)
        int rangeType = xAxisButtonGroup->checkedId();
        setRangeType(rangeType);

        // 설정값 적용
        axisY->setRange(yAxisMinInput->text().toInt(), yAxisMaxInput->text().toInt());

        update(); // 그래프 업데이트
        settingsDialog.accept();
        });

    connect(closeButton, &QPushButton::clicked, &settingsDialog, &QDialog::reject);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);

    settingsDialog.exec(); // 팝업 창 실행
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
    int tickCount = 7; // ✅ Tick 개수 7개 고정
    QString format;

    switch (rangeType) {
    case 0: // 1분 단위
        minTime = now.addSecs(-50);
        maxTime = now.addSecs(10);
        format = "hh:mm:ss";
        break;
    case 1: // 1시간 단위
        minTime = now.addSecs(-50 * 60);
        maxTime = now.addSecs(10 * 60);
        format = "hh:mm";
        break;
    case 2: // 1일 단위
        minTime = now.addSecs(-20 * 60 *60);
        maxTime = now.addSecs(4 * 60 * 60);
        format = "dd hh:mm";
        break;
    case 3: // 1주 단위
        minTime = now.addDays(-5);
        maxTime = now.addDays(1);
        format = "MM-dd hh:mm";
        break;
    case 4: // 1달 단위
        minTime = now.addDays(-25);
        maxTime = now.addDays(5);
        format = "MM-dd hh:mm";
        break;
    default:
        qWarning() << "Invalid range type for X-axis update.";
        return;
    }

    // ✅ X축 범위 설정
    axisX->setRange(minTime, maxTime);
    axisX->setFormat(format);
    axisX->setTickCount(tickCount);

    //// ✅ 현재 시간이 80% 지점에 오도록 설정
    //qreal timeSpan = maxTime.toSecsSinceEpoch() - minTime.toSecsSinceEpoch();
    //qreal shiftAmount = timeSpan * 0.2; // ✅ 20%를 왼쪽으로 이동

    //minTime = minTime.addSecs(-shiftAmount);
    //maxTime = maxTime.addSecs(-shiftAmount);

    axisX->setRange(minTime, maxTime); // ✅ 새로운 X축 범위 적용

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

