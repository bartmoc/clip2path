#pragma once

#include "config/Config.hpp"

#include <QObject>
#include <QString>

class QApplication;
class ClipboardMonitor;
class ClipboardClassifier;
class ClipboardPublisher;
class ImageExtractor;
class ImageStore;
class TrayController;
class ConfigService;
class RetentionService;

class Application : public QObject {
    Q_OBJECT
public:
    explicit Application(QApplication &app, QObject *parent = nullptr);
    ~Application() override;

    void setConfig(const Config &config);
    void run();

    void processClipboard();
    void toggleMonitoring();
    void setPublishMode(PublishMode mode);
    void openOutputFolder();
    void copyLastPath();
    void clearGeneratedFiles();

private:
    void setupComponents();
    void wireSignals();
    void runRetentionCleanup();

    QApplication &m_qtApp;
    Config m_config;

    ClipboardMonitor *m_monitor = nullptr;
    ClipboardClassifier *m_classifier = nullptr;
    ClipboardPublisher *m_publisher = nullptr;
    ImageExtractor *m_extractor = nullptr;
    ImageStore *m_store = nullptr;
    TrayController *m_tray = nullptr;
    ConfigService *m_configService = nullptr;
    RetentionService *m_retention = nullptr;
};
