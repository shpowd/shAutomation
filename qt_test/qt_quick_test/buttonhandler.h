#pragma once
#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <QObject>
#include <QDebug>

class ButtonHandler : public QObject {
    Q_OBJECT
public:
    explicit ButtonHandler(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void onButtonClicked() {
        qDebug() << "Button Clicked from C++!";
    }
};

#endif // BUTTONHANDLER_H
