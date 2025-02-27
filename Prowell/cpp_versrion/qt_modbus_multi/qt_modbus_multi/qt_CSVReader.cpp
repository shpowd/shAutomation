#include "qt_CSVReader.h"

QStringList CSVReader::orderedHeaders;  // ✅ 정적 변수 정의

QMap<int, QMap<QString, QString>> CSVReader::readAllSettings() {
    QMap<int, QMap<QString, QString>> settings;
    orderedHeaders.clear();  // ✅ 헤더 순서 초기화

    QFile file("./config.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ CSV 파일을 열 수 없습니다: " << file.errorString();
        return settings;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine();
    orderedHeaders = headerLine.split(",", Qt::KeepEmptyParts);  // ✅ CSV 헤더 순서 저장
    qDebug() << headerLine;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",", Qt::KeepEmptyParts);

        if (values.size() != orderedHeaders.size()) {
            qWarning() << "⚠️ 잘못된 CSV 형식 (필드 개수 불일치): " << line;
            continue;
        }

        bool ok;
        int monitoringIndex = values[0].toInt(&ok);
        if (!ok) {
            qWarning() << "⚠️ 잘못된 인덱스 값: " << values[0];
            continue;
        }

        QMap<QString, QString> rowData;
        for (int i = 1; i < orderedHeaders.size(); ++i) {  // ✅ 헤더 순서대로 저장
            rowData[orderedHeaders[i]] = values[i];
        }

        settings[monitoringIndex] = rowData;
    }

    file.close();
    qDebug() << "✅ CSV 설정을 불러왔습니다. 총 " << settings.size() << "개의 설정 로드됨.";
    return settings;
}





void CSVReader::writeAllSettings(const QMap<int, QMap<QString, QString>>& settings) {
    QFile file("./config.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "❌ CSV 파일을 저장할 수 없습니다: " << file.errorString();
        return;
    }

    QTextStream out(&file);

    if (settings.isEmpty() || orderedHeaders.isEmpty()) {
        qWarning() << "⚠️ 저장할 데이터가 없거나 헤더 정보가 없습니다.";
        return;
    }


    out << orderedHeaders.join(",") << "\n";  // ✅ 기존 CSV 헤더 순서 유지

    for (auto it = settings.begin(); it != settings.end(); ++it) {
        qDebug() << "✅ siteSettingWindow 설정이 저장되었습니다.";

        QStringList row;
        row.append(QString::number(it.key()));  // ✅ Index 저장

        for (int i = 1; i < orderedHeaders.size(); ++i) {
            row.append(it.value().value(orderedHeaders[i], ""));  // ✅ 헤더 순서 유지하면서 값 저장
            //qDebug() << it.value().value(orderedHeaders[i], "");  // ✅ 디버깅 메시지 추가
        }

        out << row.join(",") << "\n";
    }

    file.close();
    qDebug() << "✅ CSV 설정을 저장했습니다.";
}
