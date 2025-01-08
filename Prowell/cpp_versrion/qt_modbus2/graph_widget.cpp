#include "graph_widget.h"

GraphWidget::GraphWidget(QWidget* parent) : QWidget(parent)
{
    setWindowTitle("Settings");
    setWindowFlags(Qt::Window); // 독립적인 창으로 설정
    initSettingUI(); // UI 초기화
    QPoint parentPos = parent->geometry().topLeft(); // 부모 창의 좌상단 좌표
    move(parentPos); // 새 창의 위치 설정
}

GraphWidget::~GraphWidget()
{
}

void GraphWidget::initSettingUI()
{

}