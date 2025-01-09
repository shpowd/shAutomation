#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QTimer>
#include <QVBoxLayout>
#include <QDateTime>
#include <QRandomGenerator>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget* parent = nullptr);
    ~GraphWidget();
    void addDataPoint(qint64 timestamp, double yValue);

private:
    void initSettingUI(); // UI 구성 메서드
    void updateData();
    void updateYAxisRange();
    void updateXAxisRange();

    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;
    QDateTimeAxis* axisX;
    QValueAxis* axisY;
    QTimer* timer;
    qint64 timestamp;
    QDateTime now;

    QVector<QPointF> data; // 그래프 데이터
    const int maxDataSize = 100; // 데이터 개수 제한
    const int displayRangeInSeconds = 10; // X축 표시 범위 (초 단위)
};


#endif // SETTINGWIDGET_H