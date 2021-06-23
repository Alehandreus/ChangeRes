#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QString>
#include <QAbstractNativeEventFilter>
#include <QSettings>
#include <QVector>
#include <QPair>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>

class BackEnd : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(bool autorunProperty READ autorunProperty WRITE setAutorunProperty NOTIFY autorunPropertyChanged)
    Q_PROPERTY(QString resolutionsText READ resolutionsText WRITE setResolutionsText NOTIFY resolutionsTextChanged)

public:
    BackEnd(QString appName, QSettings *settings, QString jsonFilePath);
    void registerHotKeys();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;

    QString resolutionsText();
    void setResolutionsText(QString newResolutionsText);

    bool autorunProperty();
    void setAutorunProperty(bool autorun);

    QSettings *settings;

signals:
    void resolutionsTextChanged();
    void autorunPropertyChanged();

private:    
    QVector<QPair<int, int>> resolutions;
    int currentResolutionIndex = 0;
    int upResolutionHotkeyId = 100;
    int downResolutionHotkeyId = 101;
    void writeResolutionsToJson();
    void loadResolutionsFromJson();
    void changeResolutionToCurrentIndex();
    void setTextFromResolutions();
    void upResolution();
    void downResolution();

    QString jsonFilePath;
    QString resolutionsTextData;
    QString appName;

public slots:
    bool setResolutionsFromText(QString resolutionListString);
    void addToAutorun();
    void removeFromAutorun();

};

#endif // BACKEND_H
