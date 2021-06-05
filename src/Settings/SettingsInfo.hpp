/*
 * Copyright (C) 2019-2021 Ashar Khan <ashar786khan@gmail.com>
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

#ifndef SETTINGSINFO_HPP
#define SETTINGSINFO_HPP

#include <QCoreApplication>
#include <QVariant>
#include <functional>

class ValueWidget;

class SettingsInfo
{
    Q_DECLARE_TR_FUNCTIONS(SettingsInfo)

  public:
    class SettingInfo;

    struct SettingIter
    {
        const SettingInfo *info;
        QString pre;

        SettingIter() = default;
        SettingIter(const SettingInfo *i) : info(i)
        {
        }
        SettingIter &child(QString key, QString name);
        QString key() const
        {
            return pre + info->key();
        }
    };

    class SettingInfo
    {
      public:
        QString name;             // key
        QString desc;             // translated description
        QString untrDesc;         // untranslated description
        QString type;             // int, bool, QString, etc.
        QString ui;               // type of the widget
        QString tip;              // translated tooltips
        QString untrTip;          // untranslated tooltips
        QString docAnchor;        // the anchor of the documentation
        bool requireAllDepends{}; // false for one of the depends, true for all depends
        bool immediatelyApply{};
        std::function<void(const SettingInfo *, ValueWidget *, QWidget *)> onApply;
        QList<QPair<QString, std::function<bool(const QVariant &)>>> depends;
        QList<QString> old; // the old keys of this setting
        QVariant def;
        QVariant param;
        QList<SettingInfo> child;

        QString key() const
        {
            return name.toLower().replace("c++", "cpp").replace(' ', '_');
        }
        int findChild(QString name) const
        {
            for (int i = 0; i < child.size(); i++)
            {
                if (child[i].name == name)
                {
                    return i;
                }
            }
            return -1;
        }
    };

    static void updateSettingInfo();

    static const SettingInfo &findSetting(const QString &name)
    {
        return findSetting(name, getSettings());
    }

    static const SettingInfo &findSetting(const QString &name, const QList<SettingInfo> &infos)
    {
        static SettingInfo empty;
        for (const SettingInfo &si : infos)
            if (si.name == name)
                return si;
        return empty;
    }

    static const QList<SettingInfo> &getSettings()
    {
        return fakeRoot.child;
    }

    static SettingIter beginIter(const QString &name)
    {
        return SettingIter(&findSetting(name, getSettings()));
    }

    static SettingIter fakeRootIter()
    { // This fake root has not type. Only used for it's child.
        return SettingIter(&fakeRoot);
    }

  private:
    static SettingInfo fakeRoot;
    static QList<SettingInfo> &settings;

    friend class SettingsUpdater;
};

inline SettingsInfo::SettingIter &SettingsInfo::SettingIter::child(QString key, QString name)
{
    if (!info || info->type != "Object")
    {
        throw "Getting child on a value";
    }
    for (const auto &c : info->child)
    {
        if (c.name == name)
        {
            info = &c;
            pre = QString("%1%2%3").arg(pre, key, key != "" ? "/" : "");
            return *this;
        }
    }
    throw "Getting a child which is not exist";
}

#endif // SETTINGSINFO_HPP
