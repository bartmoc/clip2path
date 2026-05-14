#pragma once

#include "config/Config.hpp"

#include <QObject>
#include <QTimer>

class QClipboard;
class QImage;

class ClipboardPublisher : public QObject {
    Q_OBJECT
public:
    explicit ClipboardPublisher(int guardTimeoutMs = 500, QObject *parent = nullptr);

    void publish(const QImage &image, const QString &savedImagePath, PublishMode mode);
    void publishPathOnly(const QString &path);

signals:
    void pathPublished(const QString &path);
    void publishing();

private:
    void releaseGuard();

    QClipboard *m_clipboard = nullptr;
    QTimer *m_guardTimer = nullptr;
};
