#include "trackpadwidget.h"

TrackpadWidget::TrackpadWidget(QWidget* parent) : QWidget(parent) {
    setStyleSheet("background-color: #000000; border: 3px solid black; border-radius: 10px;");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 🔹 크기 자동 조정
}

void TrackpadWidget::mousePressEvent(QMouseEvent* event) {
    lastMousePosition = event->pos();
    lastDrawPosition = lastMousePosition;
    update();
}

void TrackpadWidget::mouseMoveEvent(QMouseEvent* event) {
    int deltaX = event->pos().x() - lastMousePosition.x();
    int deltaY = event->pos().y() - lastMousePosition.y();
    lastMousePosition = event->pos();
    lastDrawPosition = lastMousePosition;
    update();

    // 🔹 이동 값을 0~1000 범위로 변환
    scaleAndEmitMovement(deltaX, -deltaY);
}

// 🔹 트랙패드 크기 변경 시 1:1 비율 유지
void TrackpadWidget::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event);
    int newSize = qMin(width(), height()); // 가장 작은 값을 기준으로 1:1 비율 유지
    resize(newSize, newSize);
}

// 🔹 이동 값을 0~1000 범위로 변환하여 시그널 발신
void TrackpadWidget::scaleAndEmitMovement(int deltaX, int deltaY) {
    if (width() == 0 || height() == 0) return; // 방어 코드

    int scaledX = (deltaX * 1000) / width();
    int scaledY = (deltaY * 1000) / height();

    emit moveScrollBars(scaledX, scaledY);
}

void TrackpadWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    int radius = 6;
    painter.drawEllipse(lastDrawPosition, radius, radius);
}
