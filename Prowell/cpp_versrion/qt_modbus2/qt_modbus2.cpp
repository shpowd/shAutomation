#include "qt_modbus2.h"

qt_modbus2::qt_modbus2(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("빈 창");
    resize(800, 600); // 기본 크기 설정 (800x600)


    // 버튼 생성 및 설정
    openButton = new QPushButton("새 창 열기", this);
    openButton->setGeometry(350, 250, 100, 50); // 위치 및 크기 설정

    // 버튼 클릭 시 슬롯 연결
    connect(openButton, &QPushButton::clicked, this, &qt_modbus2::openSettingWidget);


}

qt_modbus2::~qt_modbus2(){
    
    delete SettingWidget; // 메모리 정리

}


void qt_modbus2::openSettingWidget() {
    // 새로운 위젯 생성 및 표시
    if (!SettingWidget) {
        SettingWidget = new SettingWidget(this); // 두 번째 창 생성
    }
    SettingWidget->show(); // 창 표시
}