#pragma once
#ifndef QT_CSV_READER_H
#define QT_CSV_READER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class CSVReader {
public:
    static QString readCSVSetting(int monitoringIndex);
};

#endif // CSV_READER_H
