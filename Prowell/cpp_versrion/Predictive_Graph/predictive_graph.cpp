#include "predictive_graph.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QFrame>
#include <cmath>
#include <numeric>
#include "pybind_.h"  // pybind_.h 파일 포함

Predictive_Graph::Predictive_Graph(QWidget* parent)
    : QMainWindow(parent)
    , currentTime(0) {
    this->resize(1000, 800); // 창 크기 증가

    // 첫 번째 그래프 설정
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
    chart->setTitle("Graph 1");
    chart->legend()->hide();
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 예측 선 추가 (Graph 1)
    QPen redPen;
    redPen.setColor(Qt::red);
    redPen.setStyle(Qt::DashLine);
    predictedSeries = new QLineSeries(this);
    predictedSeries->setPen(redPen);
    chart->addSeries(predictedSeries);
    predictedSeries->attachAxis(xAxis);
    predictedSeries->attachAxis(yAxis);

    // 두 번째 그래프 설정
    chart2 = new QChart();
    series2 = new QLineSeries(this);
    chart2->addSeries(series2);
    xAxis2 = new QValueAxis(this);
    yAxis2 = new QValueAxis(this);
    xAxis2->setRange(-maxTime / 2, maxTime / 2);
    yAxis2->setRange(0, 1000);
    chart2->addAxis(xAxis2, Qt::AlignBottom);
    chart2->addAxis(yAxis2, Qt::AlignLeft);
    series2->attachAxis(xAxis2);
    series2->attachAxis(yAxis2);
    chart2->setTitle("Graph 2");
    chart2->legend()->hide();
    chartView2 = new QChartView(chart2, this);
    chartView2->setRenderHint(QPainter::Antialiasing);

    // 예측 선 추가 (Graph 2)
    predictedSeries2 = new QLineSeries(this);
    predictedSeries2->setPen(redPen);
    chart2->addSeries(predictedSeries2);
    predictedSeries2->attachAxis(xAxis2);
    predictedSeries2->attachAxis(yAxis2);

    // 세 번째 그래프 (산점도) 설정
    chart3 = new QChart();
    lineSeries3 = new QLineSeries(this); // 🔹 QLineSeries만 사용
    chart3->addSeries(lineSeries3);
    xAxis3 = new QValueAxis(this);
    yAxis3 = new QValueAxis(this);
    xAxis3->setRange(0, 1000);
    yAxis3->setRange(0, 1000);
    xAxis3->setTitleText("Graph 1 Y-values");
    yAxis3->setTitleText("Graph 2 Y-values");
    chart3->addAxis(xAxis3, Qt::AlignBottom);
    chart3->addAxis(yAxis3, Qt::AlignLeft);
    lineSeries3->attachAxis(xAxis3);
    lineSeries3->attachAxis(yAxis3);
    // 🔹 예측값을 점선으로 표시할 새로운 QLineSeries 추가
    predictedSeries3 = new QLineSeries(this);
    QPen redDashedPen(Qt::red);
    redDashedPen.setStyle(Qt::DashLine);
    redDashedPen.setWidth(2);
    predictedSeries3->setPen(redDashedPen);
    chart3->addSeries(predictedSeries3);
    predictedSeries3->attachAxis(xAxis3);
    predictedSeries3->attachAxis(yAxis3);
    chart3->setTitle("Line Graph (Graph1 Y vs Graph2 Y)");


    chart3->legend()->hide();
    chartView3 = new QChartView(chart3, this);
    chartView3->setRenderHint(QPainter::Antialiasing);

    // 스크롤바 2개 추가
    yScrollBar1 = new QScrollBar(Qt::Vertical, this);
    yScrollBar1->setRange(0, 1000);
    yScrollBar1->setValue(500);
    yScrollBar2 = new QScrollBar(Qt::Vertical, this);
    yScrollBar2->setRange(0, 1000);
    yScrollBar2->setValue(500);

    // 첫 번째 그래프 & 스크롤바 (가로 정렬)
    QHBoxLayout* graph1Layout = new QHBoxLayout();
    graph1Layout->addWidget(chartView, 1);
    graph1Layout->addWidget(yScrollBar1);

    // 두 번째 그래프 & 스크롤바 (가로 정렬)
    QHBoxLayout* graph2Layout = new QHBoxLayout();
    graph2Layout->addWidget(chartView2, 1);
    graph2Layout->addWidget(yScrollBar2);

    // 그래프 1, 2를 세로로 정렬
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addLayout(graph1Layout);
    rightLayout->addLayout(graph2Layout);


    // 트랙패드 추가
    trackpadWidget = new TrackpadWidget(this);
    connect(trackpadWidget, &TrackpadWidget::moveScrollBars, this, &Predictive_Graph::handleTrackpadMove);

    // 🔹 그래프 3 + 트랙패드를 세로 배치
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(chartView3, 3);  // 🔹 그래프 3 
    leftLayout->addWidget(trackpadWidget, 2);  // 🔹 트랙패드 


    // 전체 레이아웃 (좌: Graph3, 우: Graph1 & Graph2 + Scrollbars)
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addLayout(leftLayout, 4); // 왼쪽: 산점도
    mainLayout->addLayout(rightLayout, 2); // 오른쪽: 그래프1,2 + 개별 스크롤바
    setCentralWidget(centralWidget);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Predictive_Graph::updateGraph);
    updateTimer->start(100);
}



void Predictive_Graph::updateGraph() {
    int yValue1 = yScrollBar1->maximum() - yScrollBar1->value();
    int yValue2 = yScrollBar2->maximum() - yScrollBar2->value();

    // 🔹 그래프 1 데이터 업데이트 (독립적)
    dataPoints.append(QPointF(currentTime, yValue1));
    if (dataPoints.size() > 30) dataPoints.pop_front();

    // 🔹 그래프 2 데이터 업데이트 (독립적)
    dataPoints2.append(QPointF(currentTime, yValue2));
    if (dataPoints2.size() > 30) dataPoints2.pop_front();

    // 🔹 실시간 그래프 업데이트 (독립적)
    series->clear();
    for (const auto& point : dataPoints) {
        series->append(point.x() - currentTime, point.y());
    }

    series2->clear();
    for (const auto& point : dataPoints2) {
        series2->append(point.x() - currentTime, point.y());
    }


    // 🔹 그래프 3 업데이트 (실제 데이터)
    lineSeries3->clear();
    for (int i = 0; i < dataPoints.size() && i < dataPoints2.size(); ++i) {
        double xValue = dataPoints[i].y();
        double yValue = dataPoints2[i].y();
        lineSeries3->append(xValue, yValue);
    }

    // 🔹 그래프 3 예측값 점선 초기화
    predictedSeries3->clear();

    // 🔹 예측값 저장을 위한 변수 선언
    std::vector<double> predictions1;
    std::vector<double> predictions2;

    // 🔹 그래프 1 예측 수행 (독립적)
    if (dataPoints.size() == 30) {
        std::vector<double> recentData1;
        for (const auto& point : dataPoints) {
            recentData1.push_back(point.y());
        }

        try {
            predictions1 = predict_arima(recentData1, 30);

            predictedSeries->clear();
            predictedSeries->append(0, dataPoints.back().y());
            for (int i = 0; i < predictions1.size(); ++i) {
                predictedSeries->append(i + 1, predictions1[i]);
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Prediction error (Graph 1):" << e.what();
        }
    }

    // 🔹 그래프 2 예측 수행 (독립적)
    if (dataPoints2.size() == 30) {
        std::vector<double> recentData2;
        for (const auto& point : dataPoints2) {
            recentData2.push_back(point.y());
        }

        try {
            predictions2 = predict_arima(recentData2, 30);

            predictedSeries2->clear();
            predictedSeries2->append(0, dataPoints2.back().y());
            for (int i = 0; i < predictions2.size(); ++i) {
                predictedSeries2->append(i + 1, predictions2[i]);
            }
        }
        catch (const std::exception& e) {
            qDebug() << "Prediction error (Graph 2):" << e.what();
        }
    }

    // 🔹 그래프 1, 2의 예측이 완료된 후 그래프 3에도 예측값 추가 (빨간 점선)
    if (!predictions1.empty() && !predictions2.empty()) {
        for (int i = 0; i < predictions1.size() && i < predictions2.size(); ++i) {
            double xPred = predictions1[i];  // 예측된 Graph 1 Y값
            double yPred = predictions2[i];  // 예측된 Graph 2 Y값
            predictedSeries3->append(xPred, yPred);
        }
    }

    currentTime++;
}


void Predictive_Graph::handleTrackpadMove(int deltaX, int deltaY) {
    int sensitivity = 2; // 이동 속도 조절
    yScrollBar1->setValue(yScrollBar1->value() - deltaX / sensitivity);
    yScrollBar2->setValue(yScrollBar2->value() - deltaY / sensitivity);
}