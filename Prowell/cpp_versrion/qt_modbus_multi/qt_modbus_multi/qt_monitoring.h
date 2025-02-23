#ifndef QT_MONITORING_H
#define QT_MONITORING_H

#include "qt_graph.h" // ✅ GraphWidget 포함

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCloseEvent>
#include <QMap>
#include <QString>
#include <QPointer>

class MonitoringWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MonitoringWindow(int rowNumber, const QMap<int, QMap<QString, QString>>& settings, QWidget* parent = nullptr);
    ~MonitoringWindow();

private:
    int rowNumber;
    QLabel* descriptionLabel;
    
    // 그래프 창
    QMap<int, QPointer<GraphWidget>> graphWindows; // ✅ 그래프 창을 저장하는 맵

private slots:
    // "Monitoring" 창
    void openGraphWindow(int rowNumber); // ✅ Monitoring 창을 여는 함수


signals:
    void windowClosed(int rowNumber); // ✅ 창이 닫힐 때 rowNumber를 전달하는 시그널


protected:
    void closeEvent(QCloseEvent* event) override; // ✅ 창이 닫힐 때 실행


};

#endif // MONITORING_WINDOW_H
