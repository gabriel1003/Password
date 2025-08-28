#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <qtmvc/controller.h>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    Controller controller;

    QQmlApplicationEngine engine;
    // Exposto como "backend" para os QMLs
    engine.rootContext()->setContextProperty("backend", &controller);

    engine.load(QUrl(QStringLiteral("qrc:/model.qml")));
    if (engine.rootObjects().isEmpty()) return -1;

    return app.exec();
}
