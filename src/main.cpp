#include "app/Application.hpp"
#include "config/ConfigService.hpp"
#include "config/Config.hpp"
#include "logging/Log.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QCoreApplication cliApp(argc, argv);
    cliApp.setApplicationName("clip2path");
    cliApp.setApplicationVersion("0.1.0");
    cliApp.setOrganizationName("clip2path");

    QCommandLineParser parser;
    parser.setApplicationDescription("Clipboard Image To Path — converts clipboard images to file paths");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption enableOption("enable", "Enable clipboard monitoring");
    QCommandLineOption disableOption("disable", "Disable clipboard monitoring");
    QCommandLineOption toggleOption("toggle", "Toggle clipboard monitoring");
    QCommandLineOption noTrayOption("no-tray", "Run without system tray icon");
    QCommandLineOption outputDirOption("output-dir", "Output directory for images", "path");
    QCommandLineOption formatOption("format", "Image format (png, jpg, webp)", "format", "png");
    QCommandLineOption onceOption("once", "Process clipboard once and exit");
    QCommandLineOption printConfigOption("print-config", "Print current config path and exit");

    parser.addOption(enableOption);
    parser.addOption(disableOption);
    parser.addOption(toggleOption);
    parser.addOption(noTrayOption);
    parser.addOption(outputDirOption);
    parser.addOption(formatOption);
    parser.addOption(onceOption);
    parser.addOption(printConfigOption);

    parser.process(cliApp);

    if (parser.isSet(printConfigOption)) {
        ConfigService cs;
        qInfo() << "Config file:" << cs.load().outputDir;
        return 0;
    }

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QLoggingCategory::setFilterRules("clip2path.*.debug=true");

    ConfigService configService;
    Config config = configService.load();

    if (parser.isSet(outputDirOption)) {
        config.outputDir = parser.value(outputDirOption);
    }
    if (parser.isSet(formatOption)) {
        config.format = parser.value(formatOption);
    }
    if (parser.isSet(enableOption)) {
        config.enabled = true;
    }
    if (parser.isSet(disableOption)) {
        config.enabled = false;
    }
    if (parser.isSet(toggleOption)) {
        config.enabled = !config.enabled;
    }

    Application application(app);
    if (parser.isSet(noTrayOption)) {
        config.enabled = true;
    }
    application.setConfig(config);
    application.run();

    if (parser.isSet(onceOption)) {
        application.processClipboard();
        return 0;
    }

    return app.exec();
}
