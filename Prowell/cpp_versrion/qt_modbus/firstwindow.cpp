// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "firstwindow.h"
#include "ui_firstwindow.h"
#include "mainwindow.h"


FirstWindow::FirstWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::FirstWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton1, &QPushButton::clicked, this, &FirstWindow::PushButtonClicked);
}

FirstWindow::~FirstWindow()
{
    delete ui;
}

void FirstWindow::PushButtonClicked(){
    MainWindow* mainWindow = new MainWindow(this);
    mainWindow->show();
    //this->close();
}