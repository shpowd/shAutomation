#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>


class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget* parent = nullptr);
    ~SettingWidget();

private:
    void initSettingUI(); // UI 구성 메서드
};

#endif // SETTINGWIDGET_H