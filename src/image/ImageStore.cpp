#include "ImageStore.hpp"
#include "logging/Log.hpp"
#include <QBuffer>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>

#include <sys/stat.h>

namespace {
QString normalizeDirectoryPath(const QString &path)
{
    return path.isEmpty() ? QDir::currentPath() : QDir::cleanPath(path);
}
}

ImageStore::ImageStore(const QString &outputDir, const QString &format)
    : m_outputDir(normalizeDirectoryPath(outputDir))
    , m_format(format)
{
}

std::optional<QString> ImageStore::storeImage(const QImage &image)
{
    if (image.isNull()) {
        qCWarning(lcStorage) << "Cannot store null image";
        return std::nullopt;
    }

    if (!ensureDirectoryExists()) {
        return std::nullopt;
    }

    QString filename = generateFilename();
    QString finalPath = pathInOutputDir(filename);
    QString tmpPath = pathInOutputDir("." + filename + ".tmp");

    QByteArray imageData;
    {
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        QString fmt = m_format.toUpper();
        if (fmt == "JPG") fmt = "JPEG";
        if (!image.save(&buffer, fmt.toUtf8().constData(), 80)) {
            qCWarning(lcStorage) << "Failed to encode image as" << m_format;
            return std::nullopt;
        }
    }

    QFile tmpFile(tmpPath);
    if (!tmpFile.open(QIODevice::WriteOnly)) {
        qCWarning(lcStorage) << "Failed to open temp file for writing:" << tmpPath;
        return std::nullopt;
    }

    qint64 written = tmpFile.write(imageData);
    if (written != imageData.size()) {
        qCWarning(lcStorage) << "Failed to write complete image data:" << written << "/" << imageData.size();
        tmpFile.remove();
        return std::nullopt;
    }
    tmpFile.close();

    if (!QFile::rename(tmpPath, finalPath)) {
        qCWarning(lcStorage) << "Atomic rename failed:" << tmpPath << "→" << finalPath;
        QFile::remove(tmpPath);
        return std::nullopt;
    }

    chmod(finalPath.toUtf8().constData(), 0600);

    m_lastPath = finalPath;
    qCInfo(lcStorage) << "Image saved:" << finalPath;
    return finalPath;
}

QString ImageStore::pathInOutputDir(const QString &fileName) const
{
    return QDir::cleanPath(QDir(m_outputDir).filePath(fileName));
}

bool ImageStore::ensureDirectoryExists() const
{
    QDir dir(m_outputDir);
    if (dir.exists()) {
        return true;
    }

    if (dir.mkpath(".")) {
        chmod(m_outputDir.toUtf8().constData(), 0700);
        qCInfo(lcStorage) << "Created output directory:" << m_outputDir;
        return true;
    }

    qCWarning(lcStorage) << "Failed to create output directory:" << m_outputDir;
    return false;
}

QString ImageStore::generateFilename() const
{
    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString("yyyyMMdd_HHmmss_zzz");

    QString suffix = m_format.toLower();
    if (suffix == "jpeg") suffix = "jpg";

    return timestamp + "." + suffix;
}
