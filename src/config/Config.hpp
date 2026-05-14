#pragma once

#include <QString>

enum class PublishMode {
    PathOnly,
    Composite,
    ImageOnly
};

struct Config {
    bool enabled = true;
    bool startMinimized = true;
    bool showNotifications = true;

    int debounceMs = 150;
    PublishMode publishMode = PublishMode::Composite;

    QString outputDir;
    QString format = "png";
    int jpegQuality = 92;
    int pngCompression = 6;

    bool atomicWrite = true;

    bool retentionEnabled = true;
    int maxAgeHours = 24;
    int maxFiles = 500;
    int maxTotalSizeMb = 1024;

    QString logLevel = "info";
    bool logToFile = false;
    QString logFile;
};
