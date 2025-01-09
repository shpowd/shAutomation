#include "graph_widget.h"

GraphWidget::GraphWidget(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Dynamic Line Chart");
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
    chart->setTitle("Dynamic Line Chart");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

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

    // 레이아웃에 ChartView 추가
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(chartView);

    setLayout(layout); // 레이아웃을 위젯에 바로 설정

}


void GraphWidget::updateData() {
    // 새로운 데이터 생성
    now = QDateTime::currentDateTime();
    timestamp = now.toMSecsSinceEpoch();
    double value = QRandomGenerator::global()->bounded(-10, 10);

    // 데이터 추가
    data.append(QPointF(timestamp, value));

    if (data.size() > maxDataSize) {
        data.removeFirst(); // 오래된 데이터 제거
    }

    if (data.isEmpty()) {
        return;
    }

    // 시리즈 업데이트
    series->clear();
    for (const auto& point : data) {
        series->append(point);
    }

    // X축 범위 업데이트
    updateXAxisRange();


    // Y축 범위 업데이트
    updateYAxisRange();
}

void GraphWidget::updateXAxisRange() {
    QDateTime minTime = now.addSecs(-75); // 현재 시간 - 60초
    QDateTime maxTime = now.addSecs(45);  // 현재 시간 + 60초
    axisX->setRange(minTime, maxTime);
    qDebug() << "Axis Range:" << axisX->min().toString("hh:mm:ss") << "to" << axisX->max().toString("hh:mm:ss");



    //QDateTime minTime = QDateTime::fromMSecsSinceEpoch(timestamp - displayRangeInSeconds * 1000);
    //QDateTime maxTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    //axisX->setRange(minTime, maxTime);
    // 스크롤 거리 계산
    qreal scrollStep = chartView->chart()->plotArea().width() / (axisX->tickCount() - 1);


    // 차트 스크롤
    qDebug() << axisX->tickCount();
    chart->scroll(scrollStep, 0);
    //series->attachAxis(axisX);
}

void GraphWidget::updateYAxisRange() {
    // 최소값과 최대값을 계산
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();

    for (const auto& point : data) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    // 여유 범위 추가 (옵션)
    qreal margin = (maxY - minY) * 0.1; // 10% 여유 범위
    if (margin == 0) margin = 1; // 여유 범위가 0인 경우 기본값 설정

    // Y축 범위 설정
    axisY->setRange(minY - margin, maxY + margin);
    //series->attachAxis(axisY);
}

void GraphWidget::addDataPoint(qint64 timestamp, double yValue) {
    data.append(QPointF(timestamp, yValue));

    if (data.size() > maxDataSize) {
        data.removeFirst(); // 오래된 데이터 제거
    }

    // 시리즈 업데이트
    series->clear();
    for (const auto& point : data) {
        series->append(point);
    }

    updateXAxisRange(); // X축 범위 업데이트
    updateYAxisRange(); // Y축 범위 업데이트
}