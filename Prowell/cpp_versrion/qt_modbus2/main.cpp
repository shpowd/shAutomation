﻿#include "qt_modbus2.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_modbus2 w;
    w.show();
    return a.exec();
}
