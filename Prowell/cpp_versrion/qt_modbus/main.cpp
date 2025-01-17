// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "firstwindow.h"

#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    // Uncomment the following line to enable logging
    // QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));
    QApplication a(argc, argv);
    FirstWindow w;
    w.show();

    return a.exec();
}
