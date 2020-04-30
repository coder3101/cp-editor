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

/*
 * This is the home page of the preferences window.
 * You can go to the basic settings from this page.
 */

#ifndef PREFERENCESHOMEPAGE_HPP
#define PREFERENCESHOMEPAGE_HPP

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;

class PreferencesHomePage : public QWidget
{
    Q_OBJECT

  public:
    /**
     * @brief construct a PreferencesHomePage
     */
    explicit PreferencesHomePage(QWidget *parent = nullptr);

  signals:
    /**
     * @brief tell the preferences window to switch to a page
     * @param page the path to the page to switch to
     */
    void requestPage(const QString &pagePath);

  private:
    /**
     * @breif add a button that switch to a page
     * @param page the path to the page to switch to
     * @param text the text for this button
     */
    void addButton(const QString &pagePath, const QString &text);

    QVBoxLayout *layout = nullptr;  // the main layout
    QLabel *iconLabel = nullptr;    // the application icon
    QLabel *welcomeLabel = nullptr; // the welcome QLabel
    QLabel *manualLabel = nullptr;  // the QLabel that lead people to the Manual
};

#endif // PREFERENCESHOMEPAGE_HPP