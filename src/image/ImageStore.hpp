#pragma once

#include <QImage>
#include <QString>
#include <optional>

class ImageStore {
public:
    explicit ImageStore(const QString &outputDir, const QString &format = "png");

    std::optional<QString> storeImage(const QImage &image);

    QString outputDir() const { return m_outputDir; }
    QString lastPath() const { return m_lastPath; }

private:
    QString pathInOutputDir(const QString &fileName) const;
    QString generateFilename() const;
    bool ensureDirectoryExists() const;

    QString m_outputDir;
    QString m_format;
    QString m_lastPath;
};
