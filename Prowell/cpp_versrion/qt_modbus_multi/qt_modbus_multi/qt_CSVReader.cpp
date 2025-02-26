#include "qt_CSVReader.h"

QString CSVReader::readCSVSetting(int monitoringIndex) {
    QFile file("./config.csv"); // ✅ config.csv 파일 열기
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ CSV 파일을 열 수 없습니다: " << file.errorString();
        return QString(); // 에러 발생 시 빈 문자열 반환
    }

    QTextStream in(&file);
    QString headerLine = in.readLine(); // 첫 번째 줄(헤더) 읽기
    QStringList headers = headerLine.split(",");

    // ✅ "CSVsetting" 컬럼 찾기
    int csvSettingIndex = headers.indexOf("CSVsetting");
    if (csvSettingIndex == -1) {
        qWarning() << "❌ 'CSVsetting' 열을 찾을 수 없습니다.";
        file.close();
        return QString();
    }

    // ✅ 지정된 monitoringIndex에 해당하는 데이터 검색
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");

        if (values.size() > csvSettingIndex) {
            bool ok;
            int rowIndex = values[0].toInt(&ok); // 첫 번째 열(인덱스) 파싱
            if (ok && rowIndex == monitoringIndex) {
                file.close(); // ✅ 파일 닫기
                return values[csvSettingIndex]; // ✅ 찾은 값 반환
            }
        }
    }

    file.close(); // ✅ 파일 닫기
    return QString(); // 데이터가 없을 경우 빈 문자열 반환
}
