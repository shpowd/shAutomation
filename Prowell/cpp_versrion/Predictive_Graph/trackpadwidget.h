#ifndef TRACKPADWIDGET_H
#define TRACKPADWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

class TrackpadWidget : public QWidget {
    Q_OBJECT

public:
    explicit TrackpadWidget(QWidget* parent = nullptr);

signals:
    void moveScrollBars(int scaledX, int scaledY);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override; // 🔹 크기 조정 시 1:1 비율 유지

private:
    QPoint lastMousePosition;
    QPoint lastDrawPosition;
    void scaleAndEmitMovement(int deltaX, int deltaY);
};

#endif // TRACKPADWIDGET_H
