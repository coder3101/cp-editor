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

#include "Settings/PreferencesPageTemplate.hpp"
#include "Settings/SettingsInfo.hpp"
#include "Settings/SettingsManager.hpp"
#include "Settings/ValueWrapper.hpp"
#include <QCheckBox>
#include <QDebug>

PreferencesPageTemplate::PreferencesPageTemplate(QStringList opts, bool alignTop, QWidget *parent)
    : PreferencesGridPage(alignTop, parent), options(opts)
{
    for (const QString &name : options)
    {
        auto si = SettingsInfo::findSetting(name);
#ifdef QT_DEBUG
        if (name != si.name)
            qDebug() << "Unknown option" << name;
#endif
        if (si.type == "QString")
        {
            Wrapper<QString> *wrapper = createStringWrapper(si.ui);
            wrapper->init(this, si.param);
            addRow(wrapper, si.tip, si.help, si.desc);
            widgets.push_back(wrapper);
        }
        else if (si.type == "bool")
        {
            Wrapper<bool> *wrapper = createBoolWrapper(si.ui);
            wrapper->init(si.desc, this, si.param);
            addRow(wrapper, si.tip, si.help);
            widgets.push_back(wrapper);
        }
        else if (si.type == "int")
        {
            Wrapper<int> *wrapper = createIntWrapper(si.ui);
            wrapper->init(this, si.param);
            addRow(wrapper, si.tip, si.help, si.desc);
            widgets.push_back(wrapper);
        }
        else if (si.type == "QFont")
        {
            Wrapper<QFont> *wrapper = createFontWrapper(si.ui);
            wrapper->init(this, si.param);
            addRow(wrapper, si.tip, si.help, si.desc);
            widgets.push_back(wrapper);
        }
        else if (si.type == "QVariantList")
        {
            Wrapper<QVariantList> *wrapper = createStringListsWrapper(si.ui);
            wrapper->init(this, si.param);
            addRow(wrapper, si.tip, si.help, si.desc);
            widgets.push_back(wrapper);
        }
    }

    for (const QString &name : options)
    {
        auto si = SettingsInfo::findSetting(name);
        if (si.depends.isEmpty())
            continue;
        auto currentWidget = widgets[options.indexOf(name)]->coreWidget();
        for (auto depend : si.depends)
        {
            if (!options.contains(depend))
            {
                qDebug() << name << " depends on unknown option " << depend;
                continue;
            }
            auto dependInfo = SettingsInfo::findSetting(depend);
            if (dependInfo.type != "bool")
            {
                qDebug() << name << " depends on " << depend << " which is not a bool option";
                continue;
            }
            auto dependWidget = qobject_cast<QCheckBox *>(widgets[options.indexOf(depend)]->coreWidget());
            depends[currentWidget].push_back(dependWidget);
            connect(dependWidget, &QCheckBox::toggled,
                    [this, si, currentWidget] { onDependencyUpdated(currentWidget, si.requireAllDepends); });
        }
        onDependencyUpdated(currentWidget, si.requireAllDepends);
    }
}

QStringList PreferencesPageTemplate::content()
{
    QStringList ret = options;
    for (auto opt : options)
    {
        auto si = SettingsInfo::findSetting(opt);
        if (!si.desc.isEmpty())
            ret += si.desc;
        if (!si.tip.isEmpty())
            ret += si.tip;
        if (!si.help.isEmpty())
            ret += si.help;
    }
    return ret;
}

bool PreferencesPageTemplate::areSettingsChanged()
{
    for (int i = 0; i < options.size(); ++i)
    {
        ValueWidget *widget = widgets[i];
        auto si = SettingsInfo::findSetting(options[i]);
        if (widget->getVariant() != SettingsManager::get(si.name))
            return true;
    }
    return false;
}

void PreferencesPageTemplate::makeUITheSameAsDefault()
{
    for (int i = 0; i < options.size(); ++i)
    {
        ValueWidget *widget = widgets[i];
        auto si = SettingsInfo::findSetting(options[i]);
        widget->setVariant(SettingsManager::get(si.name, true));
    }
}

void PreferencesPageTemplate::makeUITheSameAsSettings()
{
    for (int i = 0; i < options.size(); ++i)
    {
        ValueWidget *widget = widgets[i];
        auto si = SettingsInfo::findSetting(options[i]);
        widget->setVariant(SettingsManager::get(si.name));
    }
}

void PreferencesPageTemplate::makeSettingsTheSameAsUI()
{
    for (int i = 0; i < options.size(); ++i)
    {
        ValueWidget *widget = widgets[i];
        auto si = SettingsInfo::findSetting(options[i]);
        SettingsManager::set(si.name, widget->getVariant());
    }
}

void PreferencesPageTemplate::onDependencyUpdated(QWidget *widget, bool requireAllDepends)
{
    if (requireAllDepends)
    {
        for (auto checkBox : depends[widget])
        {
            if (!checkBox->isChecked())
            {
                widget->setEnabled(false);
                return;
            }
        }
        widget->setEnabled(true);
    }
    else
    {
        for (auto checkBox : depends[widget])
        {
            if (checkBox->isChecked())
            {
                widget->setEnabled(true);
                return;
            }
        }
        widget->setEnabled(false);
    }
}
