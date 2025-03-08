#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "buttonhandler.h"  // C++ 버튼 핸들러 클래스 포함

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // C++ 객체 생성 및 QML에 전달
    ButtonHandler buttonHandler;
    engine.rootContext()->setContextProperty("buttonHandler", &buttonHandler);

    // QML 파일 경로 수정 (qrc에 맞게)
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/qt_quick_test/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
