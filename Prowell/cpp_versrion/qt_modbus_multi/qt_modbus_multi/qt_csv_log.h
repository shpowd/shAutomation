#pragma once
#ifndef QT_CSV_LOG_H
#define QT_CSV_LOG_H

#include <QObject>
#include <QTimer>
#include "qt_CSVReader.h"  // config �б��

class CSVLogWorker : public QObject {
    Q_OBJECT

public:
    explicit CSVLogWorker(QObject* parent = nullptr) : QObject(parent), timer(nullptr) {
        // config.csv ���Ͽ��� CSVRate ���� �о�ɴϴ�.
        // ���⼭�� ���� ��� monitoring index 1�� CSVRate ���� ����մϴ�.
        QMap<int, QMap<QString, QString>> config = CSVReader::readAllSettings();
        int csvRate = 10; // �⺻�� (��: 10��)
        if (config.contains(1)) {
            bool ok;
            int rate = config[1].value("CSVRate", "10").toInt(&ok);
            if (ok) {
                csvRate = rate;
            }
        }
        // �� ���� ��ȯ: ���� ������ ��/��/���̶�� ������ ms�� ��ȯ�ؾ� �մϴ�.
        interval = csvRate * 1000;  // ���⼭�� csvRate�� �� ������� ����
    }

public slots:
    void startTimer() {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CSVLogWorker::onTimeout);
        timer->start(interval);
    }

    void onTimeout() {
        emit requestSave();
    }

signals:
    void requestSave();

private:
    QTimer* timer;
    int interval; // Ÿ�̸� �ֱ� (ms)
};

#endif // CSV_LOG_WORKER_H
