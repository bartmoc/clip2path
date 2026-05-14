#pragma once

#include <QObject>
#include <QString>

class RetentionService : public QObject {
    Q_OBJECT
public:
    explicit RetentionService(QObject *parent = nullptr);

    void cleanup(const QString &outputDir,
                 int maxAgeHours = 24,
                 int maxFiles = 500,
                 qint64 maxTotalSizeBytes = 1024LL * 1024 * 1024);
};
