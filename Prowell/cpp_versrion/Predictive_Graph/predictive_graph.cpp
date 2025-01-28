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
    updateTimer->start(100);  // 1초 간격으로 업데이트
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

    // 데이터가 정확히 30개일 때만 예측 수행
    if (dataPoints.size() == 30) {
        // 최근 30개의 데이터를 Python으로 전달
        std::vector<double> recentData;
        for (const auto& point : dataPoints) {
            recentData.push_back(point.y());
        }

        try {
            // Python ARIMA 예측 호출
            std::vector<double> predictions = predict_arima(recentData, 30);

            // 예측값 그래프에 추가
            predictedSeries->clear();

            // 0초의 현재값과 1초의 예측값을 연결
            predictedSeries->append(0, dataPoints.back().y()); // 현재값 (0초)
            predictedSeries->append(1, predictions[0]);       // 첫 번째 예측값 (1초)

            // 나머지 예측값 추가 (1초부터 30초까지)
            for (int i = 1; i < predictions.size(); ++i) {
                predictedSeries->append(i + 1, predictions[i]); // X값은 1초부터 시작
            }

        }
        catch (const std::exception& e) {
            qDebug() << "Prediction error:" << e.what();
        }
    }

}
