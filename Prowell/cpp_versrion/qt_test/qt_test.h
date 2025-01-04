#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_test.h"

class qt_test : public QMainWindow
{
    Q_OBJECT

public:
    qt_test(QWidget *parent = nullptr);
    ~qt_test();

private:
    Ui::qt_testClass ui;
};
