#ifndef QT_CSV_READER_H
#define QT_CSV_READER_H


#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QList>
#include <QString>
#include <QPair>
#include <QDatetime>

class CSVReader {
public:
    static QMap<int, QMap<QString, QString>> readAllSettings();
    static void writeAllSettings(const QMap<int, QMap<QString, QString>>& settings);
    static void writeAllCSVlog(const QVector<QPair<QDateTime, QVector<quint16>>>& logValues, int monitoringIndex);

private:
    static QStringList orderedHeaders;  // ✅ CSV 헤더 순서 저장
};



#endif
