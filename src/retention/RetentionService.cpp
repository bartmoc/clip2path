#include "RetentionService.hpp"
#include "logging/Log.hpp"
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

RetentionService::RetentionService(QObject *parent)
    : QObject(parent)
{
}

void RetentionService::cleanup(const QString &outputDir,
                                int maxAgeHours,
                                int maxFiles,
                                qint64 maxTotalSizeBytes)
{
    QDir dir(outputDir);
    if (!dir.exists()) {
        return;
    }

    QRegularExpression datePattern(R"(\d{8}_\d{6}_\d{3}\.)");
    QFileInfoList files = dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);

    struct {
        bool operator()(const QFileInfo &a, const QFileInfo &b) const {
            return a.lastModified() < b.lastModified();
        }
    } compareByTime;

    std::sort(files.begin(), files.end(), compareByTime);

    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-maxAgeHours * 3600);
    qint64 totalSize = 0;
    int deleted = 0;

    for (const auto &info : files) {
        totalSize += info.size();
    }

    for (const auto &info : files) {
        QString name = info.fileName();

        if (!datePattern.match(name).hasMatch()) {
            continue;
        }

        bool shouldDelete = false;

        if (maxAgeHours > 0 && info.lastModified() < cutoff) {
            shouldDelete = true;
        }

        if (!shouldDelete) {
            totalSize -= info.size();
            files.size();
        }

        if (shouldDelete) {
            QFile::remove(info.absoluteFilePath());
            deleted++;
            totalSize -= info.size();
        }
    }

    int fileCount = 0;
    qint64 runningSize = 0;
    for (int i = files.size() - 1; i >= 0; --i) {
        const auto &info = files[i];
        if (!info.exists()) {
            continue;
        }
        fileCount++;
        runningSize += info.size();

        if (fileCount > maxFiles || runningSize > maxTotalSizeBytes) {
            QFile::remove(info.absoluteFilePath());
            deleted++;
            runningSize -= info.size();
        }
    }

    if (deleted > 0) {
        qCInfo(lcStorage) << "Retention cleanup removed" << deleted << "files from" << outputDir;
    }
}
