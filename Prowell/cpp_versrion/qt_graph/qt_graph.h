#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_graph.h"

class qt_graph : public QMainWindow
{
    Q_OBJECT

public:
    qt_graph(QWidget *parent = nullptr);
    ~qt_graph();

private:
    Ui::qt_graphClass ui;
};
