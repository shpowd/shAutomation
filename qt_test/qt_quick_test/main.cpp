#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "buttonhandler.h"  // C++ ��ư �ڵ鷯 Ŭ���� ����

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // C++ ��ü ���� �� QML�� ����
    ButtonHandler buttonHandler;
    engine.rootContext()->setContextProperty("buttonHandler", &buttonHandler);

    // QML ���� ��� ���� (qrc�� �°�)
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/qt_quick_test/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
