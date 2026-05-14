#include "ClipboardMonitor.hpp"
#include "logging/Log.hpp"
#include <QClipboard>
#include <QGuiApplication>

ClipboardMonitor::ClipboardMonitor(QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
    , m_debounceTimer(new QTimer(this))
{
    m_debounceTimer->setSingleShot(true);
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardMonitor::onDataChanged);
    connect(m_debounceTimer, &QTimer::timeout, this, &ClipboardMonitor::debounced);
}

void ClipboardMonitor::setDebounceMs(int ms)
{
    m_debounceTimer->setInterval(ms);
}

void ClipboardMonitor::setEnabled(bool enabled)
{
    m_enabled = enabled;
    qCInfo(lcClipboard) << "Monitoring" << (enabled ? "enabled" : "disabled");
}

bool ClipboardMonitor::isEnabled() const
{
    return m_enabled;
}

void ClipboardMonitor::onDataChanged()
{
    if (!m_enabled) {
        return;
    }

    qCDebug(lcClipboard) << "Clipboard data changed";
    emit clipboardChanged();

    m_debounceTimer->start();
}
