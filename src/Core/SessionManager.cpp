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

#include "Core/SessionManager.hpp"
#include "../../ui/ui_appwindow.h"
#include "Core/EventLogger.hpp"
#include "Util/FileUtil.hpp"
#include "appwindow.hpp"
#include "generated/portable.hpp"
#include "mainwindow.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProgressDialog>
#include <QSizePolicy>
#include <QTimer>
#include <QVariantMap>

namespace Core
{
const static QStringList sessionFileLocations = {
#ifdef PORTABLE_VERSION
    "$BINARY/cp_editor_session.json",
#endif
    "$APPCONFIG/cp_editor_session.json"};

SessionManager::SessionManager(AppWindow *appwindow) : QObject(appwindow), app(appwindow)
{
    timer = new QTimer(this);
    progressDialog = new QProgressDialog(app);

    timer->setInterval(10000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSession()), Qt::DirectConnection);

    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowTitle(tr("Restoring Last Session"));
}

void SessionManager::updateSession()
{
    const int n = app->ui->tabWidget->count();
    QJsonObject json;
    json.insert("tabCount", n);
    json.insert("selectedTab", app->ui->tabWidget->currentIndex());

    QJsonArray arr;
    for (int t = 0; t < n; t++)
    {
        arr.push_back(QJsonDocument::fromVariant(app->windowAt(t)->toStatus().toMap()).object());
    }

    json.insert("tabs", arr);
    auto sessionText = QJsonDocument::fromVariant(json.toVariantMap()).toJson();

    Util::saveFile(Util::configFilePath(sessionFileLocations[0]), sessionText, "Editor Session", true, nullptr, true);
}

void SessionManager::restoreSession(const QString &path)
{
    if (restored)
    {
        LOG_ERR("Trying to restore session for the second time");
        return;
    }

    restored = true;

    auto text = Util::readFile(path);

    if (text.isNull())
    {
        LOG_ERR(QString("Failed to load session from [%1]").arg(path));
        return;
    }

    QJsonObject object = QJsonDocument::fromJson(text.toUtf8()).object();

    const int n = object["tabCount"].toInt();
    const int currentIndex = object["selectedTab"].toInt();
    auto tabs = object["tabs"].toArray();

    progressDialog->setMaximum(n);
    progressDialog->setValue(0);

    auto oldSize = app->size();
    app->setUpdatesEnabled(false);

    progressDialog->show();
    progressDialog->setFocus();

    int i = 1;
    for (auto const &tab : tabs)
    {
        if (progressDialog->wasCanceled())
            break;
        auto status = MainWindow::EditorStatus(tab.toObject().toVariantMap());
        progressDialog->setValue(i);
        app->openTab("");
        app->currentWindow()->loadStatus(status);
        progressDialog->setLabelText(
            QString(tr("Restoring: [%1]")).arg(app->currentWindow()->getTabTitle(true, false)));
        i++;
    }

    progressDialog->setValue(n);

    app->setUpdatesEnabled(true);
    app->resize(oldSize);

    if (currentIndex >= 0 && currentIndex < app->ui->tabWidget->count())
        app->ui->tabWidget->setCurrentIndex(currentIndex);

    progressDialog->close();
}

void SessionManager::setAutoUpdateSession(bool shouldAutoUpdate)
{
    if (shouldAutoUpdate)
        timer->start();
    else
        timer->stop();
}

void SessionManager::setAutoUpdateDuration(unsigned int duration)
{
    timer->setInterval(duration);
}

QString SessionManager::lastSessionPath()
{
    for (auto path : sessionFileLocations)
    {
        path = Util::configFilePath(path);
        if (QFile::exists(path))
        {
            return path;
        }
    }
    return QString();
}
} // namespace Core
