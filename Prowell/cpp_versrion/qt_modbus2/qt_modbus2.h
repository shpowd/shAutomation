#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QPushButton>
#include "settingwidget.h"

class qt_modbus2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit qt_modbus2(QWidget *parent = nullptr);
    ~qt_modbus2();

private slots:
    void openSettingWidget(); // 버튼 클릭 시 호출될 슬롯

private:
    QPushButton* openButton; // 버튼
    SettingWidget* SettingWidget; // 새로운 위젯

};
