// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef FIRSTWINDOW_H
#define FIRSTWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class FirstWindow;
}
QT_END_NAMESPACE

class FirstWindow;

class FirstWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FirstWindow(QWidget* parent = nullptr);
    ~FirstWindow();

private slots:
    void PushButtonClicked();

private:
    Ui::FirstWindow* ui = nullptr;
};

#endif // MAINWINDOW_H
