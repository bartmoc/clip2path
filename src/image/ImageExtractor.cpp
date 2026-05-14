#include "ImageExtractor.hpp"
#include "logging/Log.hpp"
#include <QClipboard>
#include <QGuiApplication>

std::optional<QImage> ImageExtractor::extractImage(const QMimeData *mimeData) const
{
    if (!mimeData) {
        return std::nullopt;
    }

    QImage image = qvariant_cast<QImage>(mimeData->imageData());
    if (!image.isNull()) {
        qCDebug(lcImage) << "Extracted image from MIME data, size:" << image.size();
        return image;
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    image = clipboard->image();
    if (!image.isNull()) {
        qCDebug(lcImage) << "Extracted image from clipboard, size:" << image.size();
        return image;
    }

    qCWarning(lcImage) << "Failed to extract image from clipboard";
    return std::nullopt;
}
