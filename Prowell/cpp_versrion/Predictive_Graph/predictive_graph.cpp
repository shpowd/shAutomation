#include "predictive_graph.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <cmath>
#include <numeric>
#include "pybind_.h"

Predictive_Graph::Predictive_Graph(QWidget* parent)
    : QMainWindow(parent)
    , currentTime(0)
{
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

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Predictive_Graph::updateGraph);
    updateTimer->start(1000);
}

Predictive_Graph::~Predictive_Graph() = default;

void Predictive_Graph::updateGraph() {
    int yValue = yScrollBar->maximum() - yScrollBar->value();

    // ������ �߰�
    dataPoints.append(QPointF(currentTime, yValue));
    if (dataPoints.size() > 30) {
        dataPoints.pop_front();
    }

    currentTime++;

    // �ǽð� ������ ����
    series->clear();
    for (const auto& point : dataPoints) {
        series->append(point.x() - currentTime, point.y());
    }
    test();
    int result = test();
    qDebug() << "test() returned:" << result;

    // ARIMA ���� ����Ͽ� �̷� ����
    //std::vector<double> predictions = predict_arima(getDataForPrediction(), 10);  // ���� ��� ��������

    // ������ ������ �׷����� �߰�
    //for (int i = 0; i < predictions.size(); ++i) {
    //    series->append(currentTime + i + 1, predictions[i]);
    //}
}

// ������ �غ� (�ֱ� 30���� ������)
std::vector<double> Predictive_Graph::getDataForPrediction() {
    std::vector<double> data;
    for (const auto& point : dataPoints) {
        data.push_back(point.y());
    }
    return data;
}
