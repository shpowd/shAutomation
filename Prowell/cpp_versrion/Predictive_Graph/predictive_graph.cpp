#include "predictive_graph.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <cmath>
#include <numeric>
#include "pybind_.h"  // pybind_.h 파일 포함

Predictive_Graph::Predictive_Graph(QWidget* parent)
    : QMainWindow(parent)
    , currentTime(0) {
    this->resize(600, 400);

    chart = new QChart();
    series = new QLineSeries(this);

    chart->addSeries(series);

    xAxis = new QValueAxis(this);
    yAxis = new QValueAxis(this);
    xAxis->setRange(-maxTime / 2, maxTime / 2);
    yAxis->setRange(0, 1000);
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    chart->setTitle("");
    chart->legend()->hide();

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    yScrollBar = new QScrollBar(Qt::Vertical, this);
    yScrollBar->setRange(0, 1000);
    yScrollBar->setValue(500);

    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(chartView, 1);
    mainLayout->addWidget(yScrollBar);
    setCentralWidget(centralWidget);

    // 예측값을 빨간 점선으로 표시
    QPen redPen;
    redPen.setColor(Qt::red);
    redPen.setStyle(Qt::DashLine);  // 점선으로 설정
    predictedSeries = new QLineSeries(this);
    predictedSeries->setPen(redPen);  // 빨간 점선으로 설정
    chart->addSeries(predictedSeries);
    predictedSeries->attachAxis(xAxis);
    predictedSeries->attachAxis(yAxis);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Predictive_Graph::updateGraph);
    updateTimer->start(1000);  // 1초 간격으로 업데이트
}

Predictive_Graph::~Predictive_Graph() = default;

void Predictive_Graph::updateGraph() {
    int yValue = yScrollBar->maximum() - yScrollBar->value();

    // 데이터 추가
    dataPoints.append(QPointF(currentTime, yValue));
    if (dataPoints.size() > 30) {
        dataPoints.pop_front();
    }

    // 실시간 데이터 갱신
    series->clear();
    for (const auto& point : dataPoints) {
        series->append(point.x() - currentTime, point.y());
    }

    currentTime++;

    // 예측값 계산 (현재 시간부터 30초까지 30개의 예측값)
    std::vector<double> predictions = predict_sarima(getDataForPrediction(), 30);  // 예측 결과 가져오기

    // 예측된 데이터 그래프에 추가 (빨간 점선으로 표시)
    predictedSeries->clear();  // 기존 예측값 제거
    for (int i = 0; i < predictions.size(); ++i) {
        // 예측값을 추가할 때 x 값을 currentTime + i로 설정하여 1초 간격으로 표시
        predictedSeries->append( i, predictions[i]);  // 1초 간격으로 예측값 추가
    }
}

// 데이터 준비 (최근 30개의 데이터)
std::vector<double> Predictive_Graph::getDataForPrediction() {
    std::vector<double> data;
    for (const auto& point : dataPoints) {
        data.push_back(point.y());
    }
    return data;
}
