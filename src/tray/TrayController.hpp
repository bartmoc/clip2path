#pragma once

#include "config/Config.hpp"

#include <QIcon>
#include <QObject>
#include <QSystemTrayIcon>

class QAction;
class QActionGroup;
class QMenu;

class TrayController : public QObject {
    Q_OBJECT
public:
    explicit TrayController(QObject *parent = nullptr);

    void setEnabled(bool enabled);
    void setLastPath(const QString &path);
    void setMode(PublishMode mode);

    bool isAvailable() const;

signals:
    void monitoringToggled();
    void modeChanged(PublishMode mode);
    void openFolderRequested();
    void copyLastPathRequested();
    void clearFilesRequested();
    void quitRequested();

private:
    void createMenu();
    static QIcon createTrayIcon();

    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_menu = nullptr;
    QMenu *m_modeMenu = nullptr;
    QActionGroup *m_modeGroup = nullptr;
    QAction *m_monitoringAction = nullptr;
    QAction *m_compositeAction = nullptr;
    QAction *m_pathOnlyAction = nullptr;
    QAction *m_imageOnlyAction = nullptr;
    QAction *m_openFolderAction = nullptr;
    QAction *m_copyLastPathAction = nullptr;
    QAction *m_clearFilesAction = nullptr;
    QAction *m_quitAction = nullptr;
    bool m_enabled = true;
};
