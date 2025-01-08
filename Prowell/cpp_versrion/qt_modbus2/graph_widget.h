#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>


class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget* parent = nullptr);
    ~GraphWidget();

private:
    void initSettingUI(); // UI 구성 메서드
};

#endif // SETTINGWIDGET_H