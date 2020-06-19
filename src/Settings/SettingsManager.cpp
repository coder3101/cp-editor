/*
 * Copyright (C) 2019-2020 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CP Editor.
 *
 * CP Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CP Editor behaves in unexpected way and
 * causes your ratings to go down and or lose any important contest.
 *
 * Believe Software is "Software" and it isn't immune to bugs.
 *
 */

#include "Settings/SettingsManager.hpp"
#include "Core/EventLogger.hpp"
#include "Settings/FileProblemBinder.hpp"
#include "generated/SettingsInfo.hpp"
#include "generated/portable.hpp"
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QRect>
#include <QSettings>
#include <QStandardPaths>

QVariantMap *SettingsManager::cur = nullptr;
QVariantMap *SettingsManager::def = nullptr;

static QStringList configFileLocation = {
#ifdef PORTABLE_VERSION
    "$BINARY/cp_editor_settings.ini",
#endif
    "$APPCONFIG/cp_editor_settings.ini", "$HOME/.cp_editor_settings.ini", "$HOME/cp_editor_settings.ini"};

static const QStringList noUnknownKeyWarning = {"C++/Run Command", "Python/Compile Command"};

void SettingsManager::init()
{
    for (QString &location : configFileLocation)
        location = location.replace("$APPCONFIG", QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                       .replace("$HOME", QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                       .replace("$BINARY", QCoreApplication::applicationDirPath());

    // find exist config file
    QString loadPath;
    for (auto path : configFileLocation)
    {
        if (QFile::exists(path))
        {
            loadPath = path;
            break;
        }
    }

    loadSettings(loadPath);
}

void SettingsManager::deinit()
{
    saveSettings(configFileLocation[0]);

    delete cur;
    delete def;
    cur = def = nullptr;
}

void SettingsManager::loadSettings(const QString &path)
{
    LOG_INFO("Start loading settings from " + path);

    if (cur)
        delete cur;
    if (def)
        delete def;

    cur = new QVariantMap();
    def = new QVariantMap();

    // default settings
    for (const SettingInfo &si : settingInfo)
        def->insert(si.name, si.def);

    if (!path.isEmpty())
    {
        QSettings setting(path, QSettings::IniFormat);

        // load most of settings
        for (const SettingInfo &si : settingInfo)
        {
            if (si.type.startsWith("QMap("))
            {
                int depth = si.type.mid(5, si.type.length() - 6).toInt();
                std::function<void(QString, int)> load;
                load = [&](QString prefix, int depth) {
                    if (depth == 0)
                        for (const QString &key : setting.childKeys())
                            set(QString("%1/%2").arg(prefix, key), setting.value(key));
                    else
                        for (const QString &key : setting.childGroups())
                        {
                            setting.beginGroup(key);
                            load(QString("%1/%2").arg(prefix, key), depth - 1);
                            setting.endGroup();
                        }
                };
                setting.beginGroup(si.key());
                load(si.name, depth - 1);
                setting.endGroup();
            }
            else if (setting.contains(si.key()) && setting.value(si.key()).isValid())
                set(si.name, setting.value(si.key()));
            else
                for (const QString &old : si.old)
                    if (setting.contains(old) && setting.value(old).isValid())
                    {
                        set(si.name, setting.value(old));
                        break;
                    }
        }

        // load file problem binding
        FileProblemBinder::fromVariant(setting.value("file_problem_binding"));

        // rename themes
        QString theme = get("Editor Theme")
                            .toString()
                            .replace("Monkai", "Monokai")
                            .replace("Drakula", "Dracula")
                            .replace("Solarised", "Solarized");
        set("Editor Theme", theme);
    }

    LOG_INFO("Settings have been loaded from " + path);
}

void SettingsManager::saveSettings(const QString &path)
{
    LOG_INFO("Start saving settings to " + path);

    QSettings setting(path, QSettings::IniFormat);
    setting.clear(); // Otherwise SettingsManager::remove won't work

    // save most settings
    for (const SettingInfo &si : settingInfo)
        if (si.type.startsWith("QMap("))
            for (const QString &key : keyStartsWith(QString("%1/").arg(si.name)))
                setting.setValue(QString("%1/%2").arg(si.key(), key.mid(si.name.length() + 1)), get(key));
        else
            setting.setValue(si.key(), get(si.name));

    // save file problem binding
    setting.setValue("file_problem_binding", FileProblemBinder::toVariant());

    setting.sync();

    LOG_INFO("Settings have been saved to " + path);
}

QVariant SettingsManager::get(QString key, bool alwaysDefault)
{
    if (!alwaysDefault && cur->contains(key))
        return cur->value(key);
    else if (def->contains(key))
        return def->value(key);
    else
    {
#ifdef QT_DEBUG
        if (!noUnknownKeyWarning.contains(key))
            qDebug() << "Settings: getting unknown key: " << key;
#endif
        LOG_WARN("SettingsManager::getting unknown key: " << key);
        return QVariant();
    }
}

bool SettingsManager::contains(const QString &key)
{
    return cur->contains(key);
}

void SettingsManager::set(const QString &key, QVariant value)
{
    LOG_INFO_IF(!key.startsWith("Snippets/"), INFO_OF(key) << INFO_OF(value.toString()));
    cur->insert(key, value);
}

void SettingsManager::remove(QStringList keys)
{
    for (const QString &key : keys)
        cur->remove(key);
}

void SettingsManager::reset()
{
    *cur = *def;
}

QStringList SettingsManager::keyStartsWith(const QString &head)
{
    QStringList keys = cur->keys();
    keys.erase(std::remove_if(keys.begin(), keys.end(), [head](QString s) { return !s.startsWith(head); }), keys.end());
    return keys;
}
