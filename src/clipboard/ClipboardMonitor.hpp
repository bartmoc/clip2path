#pragma once

#include <QObject>
#include <QTimer>

class QClipboard;

class ClipboardMonitor : public QObject {
    Q_OBJECT
public:
    explicit ClipboardMonitor(QObject *parent = nullptr);

    void setDebounceMs(int ms);
    void setEnabled(bool enabled);

    bool isEnabled() const;

signals:
    void clipboardChanged();
    void debounced();

private:
    void onDataChanged();

    QClipboard *m_clipboard = nullptr;
    QTimer *m_debounceTimer = nullptr;
    bool m_enabled = true;
};
