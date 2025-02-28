#ifndef QT_GRAPH_H
#define QT_GRAPH_H

#include <QWidget>
#include <QtCharts>
#include <QTimer>
#include <QVBoxLayout>
#include <QDateTime>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>


class GraphWidget : public QWidget{
    Q_OBJECT

public:
    explicit GraphWidget(int graphIndex, QWidget* parent = nullptr);  // ✅ 생성자 추가
    ~GraphWidget();
    //void setCurrentButtonIndex(int index);
    void setRangeType(int type); // X축 범위 유형 설정
    void updateGraphData(const QVector<QPair<QDateTime, QVector<quint16>>>& data);


private:
    void initGraphUI(); // UI 구성 메서드
    void updateData(const QVector<QPair<QDateTime, QVector<quint16>>>& data);
    void updateXAxisRange();
    void updateYAxisRange();
    void updateButtonStyles();
    void openGraphSettings();

    int graphIndex;  // ✅ 저장할 변수 추가

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
    QVector<QPair<qint64, QVector<quint16>>> data; // timestamp와 values를 저장
    int rangeType = 0; // X축 범위 유형 (0: 1분, 1: 1시간, 2: 1일, 3: 1달)
    const int maxDataSize = 6000; // 데이터 개수 제한
    //const int displayRangeInSeconds = 10; // X축 표시 범위 (초 단위)
    QVector<QPushButton*> chartButtons; // 버튼 목록
    int currentButtonIndex = -1;         // 현재 선택된 차트 인덱스
};


#endif // SETTINGWIDGET_H