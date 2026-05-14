#include "TrayController.hpp"
#include "logging/Log.hpp"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopServices>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QUrl>

TrayController::TrayController(QObject *parent)
    : QObject(parent)
    , m_trayIcon(new QSystemTrayIcon(this))
    , m_menu(new QMenu())
{
    createMenu();

    QIcon icon = createTrayIcon();
    m_trayIcon->setIcon(icon);
    QApplication::setWindowIcon(icon);
    m_trayIcon->setContextMenu(m_menu);
    m_trayIcon->setToolTip("Clipboard Image To Path: monitoring enabled");
    m_trayIcon->show();

    qCInfo(lcTray) << "Tray icon visible:" << m_trayIcon->isVisible();
}

bool TrayController::isAvailable() const
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

void TrayController::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (m_monitoringAction) {
        m_monitoringAction->setText(enabled ? "Monitoring enabled" : "Monitoring disabled");
    }
    m_trayIcon->setToolTip(
        enabled
            ? "Clipboard Image To Path: monitoring enabled"
            : "Clipboard Image To Path: monitoring disabled");
}

void TrayController::setLastPath(const QString &path)
{
    if (m_copyLastPathAction) {
        m_copyLastPathAction->setEnabled(!path.isEmpty());
    }
}

void TrayController::setMode(PublishMode mode)
{
    switch (mode) {
    case PublishMode::Composite:
        if (m_compositeAction) m_compositeAction->setChecked(true);
        break;
    case PublishMode::PathOnly:
        if (m_pathOnlyAction) m_pathOnlyAction->setChecked(true);
        break;
    case PublishMode::ImageOnly:
        if (m_imageOnlyAction) m_imageOnlyAction->setChecked(true);
        break;
    }
}

void TrayController::createMenu()
{
    m_monitoringAction = m_menu->addAction("Monitoring enabled");
    m_monitoringAction->setCheckable(true);
    m_monitoringAction->setChecked(true);
    connect(m_monitoringAction, &QAction::triggered, this, &TrayController::monitoringToggled);

    m_menu->addSeparator();

    m_modeMenu = m_menu->addMenu("Clipboard mode");
    m_modeGroup = new QActionGroup(m_modeMenu);
    m_modeGroup->setExclusive(true);

    m_compositeAction = m_modeMenu->addAction("Composite: image + path");
    m_compositeAction->setCheckable(true);
    m_compositeAction->setChecked(true);
    m_modeGroup->addAction(m_compositeAction);
    connect(m_compositeAction, &QAction::triggered, this, [this] {
        emit modeChanged(PublishMode::Composite);
    });

    m_pathOnlyAction = m_modeMenu->addAction("Path only");
    m_pathOnlyAction->setCheckable(true);
    m_modeGroup->addAction(m_pathOnlyAction);
    connect(m_pathOnlyAction, &QAction::triggered, this, [this] {
        emit modeChanged(PublishMode::PathOnly);
    });

    m_imageOnlyAction = m_modeMenu->addAction("Image only");
    m_imageOnlyAction->setCheckable(true);
    m_modeGroup->addAction(m_imageOnlyAction);
    connect(m_imageOnlyAction, &QAction::triggered, this, [this] {
        emit modeChanged(PublishMode::ImageOnly);
    });

    m_menu->addSeparator();

    m_openFolderAction = m_menu->addAction("Open pictures folder");
    connect(m_openFolderAction, &QAction::triggered, this, &TrayController::openFolderRequested);

    m_copyLastPathAction = m_menu->addAction("Copy last image path");
    m_copyLastPathAction->setEnabled(false);
    connect(m_copyLastPathAction, &QAction::triggered, this, &TrayController::copyLastPathRequested);

    m_clearFilesAction = m_menu->addAction("Clear generated files");
    connect(m_clearFilesAction, &QAction::triggered, this, &TrayController::clearFilesRequested);

    m_menu->addSeparator();

    m_quitAction = m_menu->addAction("Quit");
    connect(m_quitAction, &QAction::triggered, this, &TrayController::quitRequested);
}

QIcon TrayController::createTrayIcon()
{
    QIcon icon;
    for (const int size : {16, 24, 32, 48, 64, 128}) {
        const qreal s = static_cast<qreal>(size);
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(Qt::NoPen);

        const qreal pad = s * 0.10;
        const qreal corner = s * 0.20;
        const qreal bodyR = s * 0.06;
        const qreal outerX = pad;
        const qreal outerY = pad + corner;
        const qreal outerW = s - 2.0 * pad;
        const qreal outerH = s - 2.0 * pad - corner;

        QColor body(70, 130, 200);
        p.setBrush(body);
        QPainterPath path;
        path.addRoundedRect(outerX, outerY, outerW, outerH, bodyR, bodyR);
        p.drawPath(path);

        QPainterPath fold;
        QColor foldColor = body.lighter(130);
        p.setBrush(foldColor);
        fold.moveTo(outerX + outerW - corner, outerY);
        fold.lineTo(outerX + outerW, outerY);
        fold.lineTo(outerX + outerW, outerY + corner);
        fold.closeSubpath();
        p.drawPath(fold);

        QColor lineColor = body.lighter(170);
        p.setBrush(lineColor);
        const qreal lineX = s * 0.22;
        const qreal lineH = s * 0.05;
        const qreal lineW = s * 0.50;
        const qreal lineR = lineH * 0.4;

        const qreal line1Y = s * 0.32;
        p.drawRoundedRect(lineX, line1Y, lineW, lineH, lineR, lineR);

        const qreal line2Y = s * 0.43;
        p.drawRoundedRect(lineX, line2Y, lineW, lineH, lineR, lineR);

        const qreal line3Y = s * 0.54;
        const qreal line3W = s * 0.35;
        p.drawRoundedRect(lineX, line3Y, line3W, lineH, lineR, lineR);

        icon.addPixmap(pixmap);
    }
    return icon;
}
