#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QQmlContext>
#include "backend.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);   

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    const QString APPLICATION_NAME = "ChangeRes";
    QSettings settings("NoSoft", APPLICATION_NAME);

    BackEnd backEnd(APPLICATION_NAME, &settings, "resolutions.json");
    backEnd.registerHotKeys();
    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("backEnd", &backEnd);

    engine.load(url);

    app.installNativeEventFilter(&backEnd);
    app.setQuitOnLastWindowClosed(false);

    return app.exec();
}
