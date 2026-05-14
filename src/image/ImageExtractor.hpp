#pragma once

#include <QImage>
#include <QMimeData>
#include <optional>

class ImageExtractor {
public:
    std::optional<QImage> extractImage(const QMimeData *mimeData) const;
};
