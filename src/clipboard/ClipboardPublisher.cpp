#include "ClipboardPublisher.hpp"
#include "logging/Log.hpp"
#include <QBuffer>
#include <QClipboard>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QUrl>

ClipboardPublisher::ClipboardPublisher(int guardTimeoutMs, QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
    , m_guardTimer(new QTimer(this))
{
    m_guardTimer->setSingleShot(true);
    m_guardTimer->setInterval(guardTimeoutMs);
    connect(m_guardTimer, &QTimer::timeout, this, &ClipboardPublisher::releaseGuard);
}

void ClipboardPublisher::publish(const QImage &image, const QString &savedImagePath, PublishMode mode)
{
    qCInfo(lcClipboard) << "Publishing" << (mode == PublishMode::Composite ? "composite" :
                                               mode == PublishMode::PathOnly ? "path-only" : "image-only")
                        << "→" << savedImagePath;

    emit publishing();

    auto *mimeData = new QMimeData();

    mimeData->setData("application/x-clip2path-marker", QByteArray("1"));
    mimeData->setData("application/x-clip2path-path", savedImagePath.toUtf8());

    if (mode == PublishMode::PathOnly || mode == PublishMode::Composite) {
        mimeData->setText(savedImagePath);
    }

    if (mode == PublishMode::Composite) {
        mimeData->setUrls({QUrl::fromLocalFile(savedImagePath)});
    }

    if (mode == PublishMode::ImageOnly || mode == PublishMode::Composite) {
        mimeData->setImageData(image);

        QByteArray pngBytes;
        QBuffer buffer(&pngBytes);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        buffer.close();

        mimeData->setData("image/png", pngBytes);
    }

    m_clipboard->setMimeData(mimeData, QClipboard::Clipboard);

    m_guardTimer->start();
    emit pathPublished(savedImagePath);
}

void ClipboardPublisher::publishPathOnly(const QString &path)
{
    emit publishing();

    auto *mimeData = new QMimeData();
    mimeData->setData("application/x-clip2path-marker", QByteArray("1"));
    mimeData->setText(path);

    m_clipboard->setMimeData(mimeData, QClipboard::Clipboard);

    m_guardTimer->start();
    emit pathPublished(path);
}

void ClipboardPublisher::releaseGuard()
{
}
