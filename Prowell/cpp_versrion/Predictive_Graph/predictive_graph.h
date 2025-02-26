#ifndef PREDICTIVE_GRAPH_H
#define PREDICTIVE_GRAPH_H

#include <QMainWindow>
#include <QTimer>
#include <QScrollBar>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>  
#include <vector>
#include "trackpadwidget.h" //  트랙패드 추가

// ARIMA 예측을 위한 함수 선언
std::vector<double> predict_arima(const std::vector<double>& data, int steps);

QT_BEGIN_NAMESPACE
namespace Ui { class Predictive_Graph; }
QT_END_NAMESPACE

class Predictive_Graph : public QMainWindow {
    Q_OBJECT

public:
    Predictive_Graph(QWidget* parent = nullptr);
    ~Predictive_Graph() = default;  // 디폴트 소멸자

private slots:
    void updateGraph();
    void handleTrackpadMove(int deltaX, int deltaY); // 트랙패드 이동 처리

private:
    // 첫 번째 그래프 요소
    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;
    QLineSeries* predictedSeries;
    QValueAxis* xAxis;
    QValueAxis* yAxis;

    // 두 번째 그래프 요소
    QChart* chart2;
    QChartView* chartView2;
    QLineSeries* series2;
    QLineSeries* predictedSeries2;
    QValueAxis* xAxis2;
    QValueAxis* yAxis2;

    // 세 번째 그래프 (산점도)
    QChart* chart3;
    QChartView* chartView3;
    QLineSeries* lineSeries3;
    QValueAxis* xAxis3;
    QValueAxis* yAxis3;
    QLineSeries* predictedSeries3; // 🔹 그래프 3의 예측값을 표시할 점선


    // 개별 스크롤바 추가
    QScrollBar* yScrollBar1;
    QScrollBar* yScrollBar2;

    QTimer* updateTimer;
    TrackpadWidget* trackpadWidget; // 트랙패드 추가

    // 데이터 저장
    QVector<QPointF> dataPoints;  // 첫 번째 그래프 데이터
    QVector<QPointF> dataPoints2; // 두 번째 그래프 데이터

    int currentTime;
    const int maxTime = 60;
};

#endif // PREDICTIVE_GRAPH_H
