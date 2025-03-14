﻿#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

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
    void addDataPoints(qint64 timestamp, const QVector<quint16>& values);
    void setCurrentChartIndex(int index);
    void setRangeType(int type); // X축 범위 유형 설정

private:
    void initSettingUI(); // UI 구성 메서드
    void updateData();
    void updateXAxisRange();
    void updateYAxisRange();
    void updateButtonStyles();

    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;
    QLineSeries* predictionSeries;
    QDateTimeAxis* axisX;
    QValueAxis* axisY;
    //QTimer* timer;
    qint64 timestamp;
    QDateTime now;

    QVector<QPair<qint64, QVector<quint16>>> predictionData; // 예측값 저장
    int rangeType = 0; // X축 범위 유형 (0: 1분, 1: 1시간, 2: 1일, 3: 1달)
    QVector<QPair<qint64, QVector<quint16>>> data; // timestamp와 values를 저장
    const int maxDataSize = 6000; // 데이터 개수 제한
    //const int displayRangeInSeconds = 10; // X축 표시 범위 (초 단위)
    QVector<QPushButton*> chartButtons; // 버튼 목록
    int currentChartIndex = -1;         // 현재 선택된 차트 인덱스
};


#endif // SETTINGWIDGET_H