#include "ConfigService.hpp"
#include "logging/Log.hpp"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <unistd.h>

static PublishMode parsePublishMode(const QString &val)

{
    if (val == "path-only") return PublishMode::PathOnly;
    if (val == "image-only") return PublishMode::ImageOnly;
    return PublishMode::Composite;
}

static QString publishModeToString(PublishMode mode)
{
    switch (mode) {
    case PublishMode::PathOnly:  return "path-only";
    case PublishMode::ImageOnly: return "image-only";
    default:                     return "composite";
    }
}

ConfigService::ConfigService(QObject *parent)
    : QObject(parent)
{
}

QString ConfigService::configFilePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
           + "/clip2path/config.ini";
}

Config ConfigService::load() const
{
    Config config;
    QString path = configFilePath();

    if (!QFile::exists(path)) {
        qCInfo(lcApp) << "No config file found at" << path << ", using defaults";
        uid_t uid = getuid();
        config.outputDir = QString("/tmp/agent_pictures_%1/").arg(uid);
        return config;
    }

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("general");
    config.enabled = settings.value("enabled", config.enabled).toBool();
    config.startMinimized = settings.value("startMinimized", config.startMinimized).toBool();
    config.showNotifications = settings.value("showNotifications", config.showNotifications).toBool();
    settings.endGroup();

    settings.beginGroup("clipboard");
    config.debounceMs = settings.value("debounceMs", config.debounceMs).toInt();
    config.publishMode = parsePublishMode(settings.value("publishMode", "composite").toString());
    settings.endGroup();

    settings.beginGroup("storage");
    uid_t uid = getuid();
    config.outputDir = settings.value("outputDir", QString("/tmp/agent_pictures_%1/").arg(uid)).toString();
    config.format = settings.value("format", config.format).toString();
    config.jpegQuality = settings.value("jpegQuality", config.jpegQuality).toInt();
    config.pngCompression = settings.value("pngCompression", config.pngCompression).toInt();
    config.atomicWrite = settings.value("atomicWrite", config.atomicWrite).toBool();
    settings.endGroup();

    settings.beginGroup("retention");
    config.retentionEnabled = settings.value("enabled", config.retentionEnabled).toBool();
    config.maxAgeHours = settings.value("maxAgeHours", config.maxAgeHours).toInt();
    config.maxFiles = settings.value("maxFiles", config.maxFiles).toInt();
    config.maxTotalSizeMb = settings.value("maxTotalSizeMb", config.maxTotalSizeMb).toInt();
    settings.endGroup();

    settings.beginGroup("logging");
    config.logLevel = settings.value("level", config.logLevel).toString();
    config.logToFile = settings.value("logToFile", config.logToFile).toBool();
    config.logFile = settings.value("logFile", "").toString();
    settings.endGroup();

    qCInfo(lcApp) << "Config loaded from" << path;
    return config;
}

void ConfigService::save(const Config &config)
{
    QString path = configFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("general");
    settings.setValue("enabled", config.enabled);
    settings.setValue("startMinimized", config.startMinimized);
    settings.setValue("showNotifications", config.showNotifications);
    settings.endGroup();

    settings.beginGroup("clipboard");
    settings.setValue("debounceMs", config.debounceMs);
    settings.setValue("publishMode", publishModeToString(config.publishMode));
    settings.endGroup();

    settings.beginGroup("storage");
    settings.setValue("outputDir", config.outputDir);
    settings.setValue("format", config.format);
    settings.setValue("jpegQuality", config.jpegQuality);
    settings.setValue("pngCompression", config.pngCompression);
    settings.setValue("atomicWrite", config.atomicWrite);
    settings.endGroup();

    settings.beginGroup("retention");
    settings.setValue("enabled", config.retentionEnabled);
    settings.setValue("maxAgeHours", config.maxAgeHours);
    settings.setValue("maxFiles", config.maxFiles);
    settings.setValue("maxTotalSizeMb", config.maxTotalSizeMb);
    settings.endGroup();

    settings.beginGroup("logging");
    settings.setValue("level", config.logLevel);
    settings.setValue("logToFile", config.logToFile);
    settings.setValue("logFile", config.logFile);
    settings.endGroup();

    settings.sync();
    qCInfo(lcApp) << "Config saved to" << path;
}
