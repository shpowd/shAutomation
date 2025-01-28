#ifndef PREDICTIVE_GRAPH_H
#define PREDICTIVE_GRAPH_H

#include <QMainWindow>
#include <QTimer>
#include <QScrollBar>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QChart>
#include <vector>

// ARIMA 예측을 위한 함수 선언
std::vector<double> predict_arima(const std::vector<double>& data, int steps);

QT_BEGIN_NAMESPACE
namespace Ui { class Predictive_Graph; }
QT_END_NAMESPACE

class Predictive_Graph : public QMainWindow {
    Q_OBJECT

public:
    Predictive_Graph(QWidget* parent = nullptr);
    ~Predictive_Graph();

private slots:
    void updateGraph();

private:
    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;

    QLineSeries* predictedSeries;  // 예측값을 위한 QLineSeries 추가

    QValueAxis* xAxis;
    QValueAxis* yAxis;
    QScrollBar* yScrollBar;
    QTimer* updateTimer;

    QVector<QPointF> dataPoints; // 그래프 데이터
    int currentTime;
    const int maxTime = 60; // X축 범위 (60초)

};

#endif // PREDICTIVE_GRAPH_H
