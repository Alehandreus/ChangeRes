#include <QDebug>
#include "backend.h"
#include "windows.h"

BackEnd::BackEnd(QString appName, QSettings *settings, QString jsonFilePath)
{
    this->jsonFilePath = jsonFilePath;
    this->settings = settings;
    this->appName = appName;

    loadResolutionsFromJson();
    setTextFromResolutions();
}

void BackEnd::addToAutorun()
{
    #ifdef Q_OS_WIN32
        QSettings runSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        runSettings.setValue(appName, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        runSettings.sync();
    #endif
}

void BackEnd::removeFromAutorun()
{
    #ifdef Q_OS_WIN32
        QSettings runSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        runSettings.remove(appName);
    #endif
}

bool BackEnd::setResolutionsFromText(QString resolutionListString)
{
    resolutions.clear();
    currentResolutionIndex = 0;
    QString resXString = "";
    QString resYString = "";
    int parseState = 0;
    int resX;
    int resY;
    QRegExp re("\\d*");
    for (auto c : resolutionListString) {
        if (c == ' ')
        {
            parseState = 1 - parseState;
        }
        else if (c == '\xa')
        {
            parseState = 1 - parseState;
            if (re.exactMatch(resXString) & re.exactMatch(resYString))
            {
                resolutions.append(qMakePair(resXString.toInt(), resYString.toInt()));
            }
            resXString = "";
            resYString = "";

        }
        else if (parseState == 0) {
            resXString = resXString + c;
        }
        else
        {
            resYString = resYString + c;
        }
    }
    if (re.exactMatch(resXString) & re.exactMatch(resYString))
    {
        resolutions.append(qMakePair(resXString.toInt(), resYString.toInt()));
    }
    if (!resolutions.empty())
    {
        changeResolutionToCurrentIndex();
    }
    resolutionsTextData = resolutionListString;
    writeResolutionsToJson();
    return true;
}

void BackEnd::setTextFromResolutions()
{
    QString newResolutionsTextData = "";
    for (const QPair<int, int> resolutionPair : resolutions) {
        QJsonObject resolutionObject;
        newResolutionsTextData.append(QString::number(resolutionPair.first) + " " + QString::number(resolutionPair.second) + "\n");
    }
    resolutionsTextData = newResolutionsTextData;
}

void BackEnd::writeResolutionsToJson()
{
    QFile saveFile(jsonFilePath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject saveObject;
    QJsonArray resolutionsArray;
    for (const QPair<int, int> resolutionPair : resolutions) {
        QJsonObject resolutionObject;
        resolutionObject["x"] = resolutionPair.first;
        resolutionObject["y"] = resolutionPair.second;
        resolutionsArray.append(resolutionObject);
    }
    saveObject["resolutions"] = resolutionsArray;
    saveFile.write(QJsonDocument(saveObject).toJson());
}

void BackEnd::loadResolutionsFromJson()
{
    QVector<QPair<int, int>> newResolutions;
    QFile loadFile(jsonFilePath);
    loadFile.open(QIODevice::ReadOnly);
    QByteArray saveData = loadFile.readAll();

    QJsonDocument document = QJsonDocument::fromJson(saveData);
    QJsonObject object = document.object();

    QJsonValue value = object.value("resolutions");
    QJsonArray array = value.toArray();
    foreach (const QJsonValue & v, array)
    {
        newResolutions.append(qMakePair(v.toObject().value("x").toInt(), v.toObject().value("y").toInt()));
    }
    resolutions = newResolutions;

//    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

//    QJsonObject json = loadDoc.object();

//    QJsonObject resolutionsJson = json["resolutions"].toObject();

//    resolutionsTextData = "123";
//    qDebug() << loadDoc.toJson();

//    for (auto i : json["resolutions"].toArray())
//    {
//        qDebug() << i.toString();
//    }
}

void BackEnd::registerHotKeys()
{
    RegisterHotKey(NULL, upResolutionHotkeyId, MOD_ALT | MOD_SHIFT, 'O');
    RegisterHotKey(NULL, downResolutionHotkeyId, MOD_ALT | MOD_SHIFT, 'L');
}

bool BackEnd::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    MSG* msg = reinterpret_cast<MSG*>(message);

    if(msg->message == WM_HOTKEY){
        if(msg->wParam == upResolutionHotkeyId){
            upResolution();
            return true;
        }
        else if(msg->wParam == downResolutionHotkeyId){
            downResolution();
            return true;
        }
    }
    return false;
}

void BackEnd::upResolution() {
    currentResolutionIndex++;
    if (currentResolutionIndex >= resolutions.size()) {
        currentResolutionIndex = 0;
    }
    changeResolutionToCurrentIndex();
}

void BackEnd::downResolution() {
    currentResolutionIndex--;
    if (currentResolutionIndex < 0) {
        currentResolutionIndex = resolutions.size() - 1;
    }
    changeResolutionToCurrentIndex();
}

void BackEnd::changeResolutionToCurrentIndex() {
    if (!resolutions.empty())
    {
        DEVMODE deviceMode;
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmPelsWidth = resolutions[currentResolutionIndex].first;
        deviceMode.dmPelsHeight = resolutions[currentResolutionIndex].second;
        deviceMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;
        ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
    }
}

QString BackEnd::resolutionsText()
{
    return resolutionsTextData;
}

void BackEnd::setResolutionsText(QString newResolutionsText)
{
    resolutionsTextData = newResolutionsText;

}

bool BackEnd::autorunProperty()
{
    #ifdef Q_OS_WIN32
        QSettings runSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        return runSettings.contains(appName);
    #endif
    return false;
}

void BackEnd::setAutorunProperty(bool newAutorunProperty)
{

}
