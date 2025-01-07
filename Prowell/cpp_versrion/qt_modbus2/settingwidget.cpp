#include "settingwidget.h"

SettingWidget::SettingWidget(QWidget* parent)
    : QWidget(parent) {
    // 새 창 설정
    setWindowTitle("새 창");
    resize(400, 300); // 크기 설정
}

SettingWidget::~SettingWidget() {
}
