#pragma once
#ifndef QT_CSV_LOG_H
#define QT_CSV_LOG_H

#include <QObject>
#include <QTimer>
#include "qt_CSVReader.h"  // config 읽기용

class CSVLogWorker : public QObject {
    Q_OBJECT

public:
    explicit CSVLogWorker(QObject* parent = nullptr) : QObject(parent), timer(nullptr) {
        // config.csv 파일에서 CSVRate 값을 읽어옵니다.
        // 여기서는 예를 들어 monitoring index 1의 CSVRate 값을 사용합니다.
        QMap<int, QMap<QString, QString>> config = CSVReader::readAllSettings();
        int csvRate = 10; // 기본값 (예: 10초)
        if (config.contains(1)) {
            bool ok;
            int rate = config[1].value("CSVRate", "10").toInt(&ok);
            if (ok) {
                csvRate = rate;
            }
        }
        // ※ 단위 변환: 실제 단위가 일/주/월이라면 적절히 ms로 변환해야 합니다.
        interval = csvRate * 1000;  // 여기서는 csvRate가 초 단위라고 가정
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
    int interval; // 타이머 주기 (ms)
};

#endif // CSV_LOG_WORKER_H
