#include "Application.hpp"
#include "clipboard/ClipboardMonitor.hpp"
#include "clipboard/ClipboardClassifier.hpp"
#include "clipboard/ClipboardPublisher.hpp"
#include "image/ImageExtractor.hpp"
#include "image/ImageStore.hpp"
#include "tray/TrayController.hpp"
#include "config/ConfigService.hpp"
#include "config/Config.hpp"
#include "retention/RetentionService.hpp"
#include "logging/Log.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QMimeData>
#include <QRegularExpression>
#include <QUrl>

Application::Application(QApplication &app, QObject *parent)
    : QObject(parent)
    , m_qtApp(app)
{
}

Application::~Application() = default;

void Application::setConfig(const Config &config)
{
    m_config = config;
    setupComponents();
    wireSignals();
}

void Application::setupComponents()
{
    m_monitor = new ClipboardMonitor(this);
    m_monitor->setDebounceMs(m_config.debounceMs);
    m_monitor->setEnabled(m_config.enabled);

    m_classifier = new ClipboardClassifier();

    m_publisher = new ClipboardPublisher(500, this);

    m_extractor = new ImageExtractor;

    m_store = new ImageStore(m_config.outputDir, m_config.format);

    m_configService = new ConfigService(this);

    m_retention = new RetentionService(this);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_tray = new TrayController(this);
        m_tray->setEnabled(m_config.enabled);
        m_tray->setMode(m_config.publishMode);
    } else {
        qCWarning(lcApp) << "System tray not available; running without tray icon";
    }
}

void Application::wireSignals()
{
    connect(m_monitor, &ClipboardMonitor::debounced, this, &Application::processClipboard);

    if (m_tray) {
        connect(m_tray, &TrayController::monitoringToggled, this, &Application::toggleMonitoring);
        connect(m_tray, &TrayController::modeChanged, this, &Application::setPublishMode);
        connect(m_tray, &TrayController::openFolderRequested, this, &Application::openOutputFolder);
        connect(m_tray, &TrayController::copyLastPathRequested, this, &Application::copyLastPath);
        connect(m_tray, &TrayController::clearFilesRequested, this, &Application::clearGeneratedFiles);
        connect(m_tray, &TrayController::quitRequested, &m_qtApp, &QApplication::quit);
    }
}

void Application::run()
{
    qCInfo(lcApp) << "clip2path starting, output dir:" << m_config.outputDir;

    runRetentionCleanup();
}

void Application::processClipboard()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (!mimeData) {
        return;
    }

    auto result = m_classifier->classify(mimeData);
    if (result != ClipboardClassifier::Result::ImageFound) {
        return;
    }

    auto image = m_extractor->extractImage(mimeData);
    if (!image.has_value()) {
        return;
    }

    auto path = m_store->storeImage(image.value());
    if (!path.has_value()) {
        qCWarning(lcApp) << "Failed to save image, not modifying clipboard";
        return;
    }

    m_publisher->publish(image.value(), path.value(), m_config.publishMode);

    if (m_tray) {
        m_tray->setLastPath(path.value());
    }
}

void Application::toggleMonitoring()
{
    m_config.enabled = !m_config.enabled;
    m_monitor->setEnabled(m_config.enabled);

    if (m_tray) {
        m_tray->setEnabled(m_config.enabled);
    }

    m_configService->save(m_config);
}

void Application::setPublishMode(PublishMode mode)
{
    m_config.publishMode = mode;
    m_configService->save(m_config);
    qCInfo(lcApp) << "Publish mode set to" << (mode == PublishMode::Composite ? "composite" :
                                                mode == PublishMode::PathOnly ? "path-only" : "image-only");
}

void Application::openOutputFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_config.outputDir));
}

void Application::copyLastPath()
{
    QString path = m_store->lastPath();
    if (!path.isEmpty()) {
        m_publisher->publishPathOnly(path);
    }
}

void Application::clearGeneratedFiles()
{
    QDir dir(m_config.outputDir);
    if (!dir.exists()) {
        return;
    }

    QRegularExpression datePattern(R"(\d{8}_\d{6}_\d{3}\.)");
    QFileInfoList files = dir.entryInfoList(QDir::Files);
    int removed = 0;

    for (const auto &info : files) {
        if (datePattern.match(info.fileName()).hasMatch()) {
            if (QFile::remove(info.absoluteFilePath())) {
                removed++;
            }
        }
    }

    qCInfo(lcApp) << "Cleared" << removed << "generated files from" << m_config.outputDir;
}

void Application::runRetentionCleanup()
{
    if (!m_config.retentionEnabled) {
        return;
    }

    m_retention->cleanup(
        m_config.outputDir,
        m_config.maxAgeHours,
        m_config.maxFiles,
        static_cast<qint64>(m_config.maxTotalSizeMb) * 1024 * 1024);
}
