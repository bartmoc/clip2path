#pragma once

#include <QObject>
#include <QMimeData>

class ClipboardClassifier {
public:
    enum class Result {
        NoImage,
        ImageFound,
        OwnGeneratedPath,
        Unsupported
    };

    explicit ClipboardClassifier();

    Result classify(const QMimeData *mimeData) const;
};
