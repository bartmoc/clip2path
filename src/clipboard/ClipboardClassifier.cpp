#include "ClipboardClassifier.hpp"
#include "logging/Log.hpp"

ClipboardClassifier::ClipboardClassifier()
{
}

ClipboardClassifier::Result ClipboardClassifier::classify(const QMimeData *mimeData) const
{
    if (!mimeData) {
        return Result::NoImage;
    }

    if (mimeData->hasFormat("application/x-clip2path-marker")) {
        qCDebug(lcClipboard) << "Detected own clip2path marker, ignoring";
        return Result::OwnGeneratedPath;
    }

    if (mimeData->hasImage()) {
        qCDebug(lcClipboard) << "Image detected in clipboard";
        return Result::ImageFound;
    }

    return Result::NoImage;
}
