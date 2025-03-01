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



void CSVReader::writeAllCSVlog(const QVector<QPair<QDateTime, QVector<quint16>>>& logEntries, int monitoringIndex) {
    if (logEntries.isEmpty()) {
        qDebug() << "⚠️ 저장할 로그 데이터가 없습니다. (monitoringIndex:" << monitoringIndex << ")";
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    qDebug() << "✅ CSV 로그 저장 시간: " << timestamp;
    QString filename = QString("./logs/%1_log_%2.csv").arg(monitoringIndex).arg(timestamp);

    QFile file(filename);
    bool fileExists = file.exists();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "❌ CSV 파일을 저장할 수 없습니다: " << file.errorString();
        return;
    }

    QTextStream out(&file);

    #pragma region //열방향 생성
    // ✅ 데이터를 Transpose 하기 위해 2D 벡터 준비
    QVector<QVector<QString>> transposedData;

    // ✅ 첫 번째 행: Timestamp 목록
    QVector<QString> timestamps;
    timestamps.append("시간"); // 첫 번째 칸
    for (const auto& entry : logEntries) {
        timestamps.append(entry.first.toString("yyyy-MM-dd HH:mm:ss"));
    }
    transposedData.append(timestamps);

    // ✅ 센서 데이터 행 생성
    QStringList labels = {
        "그래프 넘버", "전류R", "전류S", "전류T", "진동", "베어링온도", "권선온도",
        "전류R예측", "전류S예측", "전류T예측", "진동예측", "베어링온도예측", "권선온도예측", "경보"
    };

    int numSensors = labels.size();

    for (int i = 0; i < numSensors; ++i) {
        QVector<QString> row;
        row.append(labels[i]); // ✅ 첫 번째 열(센서 이름)

        // ✅ 센서 값 추가
        for (const auto& entry : logEntries) {
            row.append(QString::number(entry.second[i]));
        }

        transposedData.append(row);
    }

    // ✅ CSV 파일에 저장
    for (const auto& row : transposedData) {
        out << row.join(",") << "\n";
    }
    #pragma endregion


    #pragma region //행방향 생성
    //// ✅ 처음 생성된 파일이면 헤더 추가
    //if (!fileExists) {
    //    out << "Timestamp,그래프넘버,전류R,전류S,전류T,진동,베어링온도,권선온도,"
    //        "전류R예측,전류S예측,전류T예측,진동예측,베어링온도예측,권선온도예측,경보\n";
    //}
    //// ✅ logValues 데이터 저장 (타임스탬프와 값들이 순서대로 저장됨)
    //for (const auto& entry : logEntries) {
    //    out << entry.first.toString("yyyy-MM-dd HH:mm:ss");
    //    for (const auto& value : entry.second) {
    //        out << "," << value;
    //    }
    //    out << "\n";
    //}
    #pragma endregion



    file.close();
    qDebug() << "✅ CSV 로그 저장 완료 (monitoringIndex:" << monitoringIndex << ")";
}


