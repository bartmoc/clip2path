#pragma once

#include "Config.hpp"
#include <QObject>

class ConfigService : public QObject {
    Q_OBJECT
public:
    explicit ConfigService(QObject *parent = nullptr);

    Config load() const;
    void save(const Config &config);

private:
    QString configFilePath() const;
};
