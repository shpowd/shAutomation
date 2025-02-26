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
    explicit MonitoringWindow(int monitoringIndex, const QMap<int, QMap<QString, QString>>& settings, QWidget* parent = nullptr);
    ~MonitoringWindow();

private:
    void initMonitoringUI(); // UI 구성 메서드
    int monitoringIndex;
    QMap<int, QMap<QString, QString>> settings; // ✅ settings을 멤버 변수로 저장
    QLabel* descriptionLabel;
    void openCSVSettingDialog();
    int getCSVSetting(int rowNum);
    void updateCSVSetting(int rowNum, int newValue);

    // 그래프 창
    QMap<int, QPointer<GraphWidget>> graphWindows; // ✅ 그래프 창을 저장하는 맵

private slots:
    // "Monitoring" 창
    void openGraphWindow(int monitoringIndex); // ✅ Monitoring 창을 여는 함수


signals:
    void windowClosed(int monitoringIndex); // ✅ 창이 닫힐 때 monitoringIndex를 전달하는 시그널


protected:
    void closeEvent(QCloseEvent* event) override; // ✅ 창이 닫힐 때 실행


};

#endif // MONITORING_WINDOW_H
