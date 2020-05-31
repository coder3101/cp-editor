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

#include "Settings/ParenthesesPage.hpp"
#include "Core/EventLogger.hpp"
#include "Settings/SettingsManager.hpp"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

ParenthesisWidget::ParenthesisWidget(const QString &language, QChar leftParenthesis, QChar rightParenthesis,
                                     Qt::CheckState autoComplete, Qt::CheckState autoRemove, Qt::CheckState tabJumpOut,
                                     QWidget *parent)
    : QWidget(parent), lang(language), left(leftParenthesis), right(rightParenthesis)
{
    mainLayout = new QVBoxLayout(this);

    nameLabel = new QLabel("Parenthesis: " + parenthesis());
    auto labelFont = font();
    labelFont.setPointSizeF(labelFont.pointSizeF() * 1.3);
    nameLabel->setFont(labelFont);
    mainLayout->addWidget(nameLabel);

    mainLayout->addSpacing(20);

    stretchLayout = new QHBoxLayout();
    mainLayout->addLayout(stretchLayout);

    stretchLayout->addStretch();

    checkBoxesLayout = new QVBoxLayout();
    stretchLayout->addLayout(checkBoxesLayout);

    auto addOption = [this](QCheckBox *&checkBox, const QString &name, Qt::CheckState state) {
        checkBox = new QCheckBox(name);
        checkBox->setTristate(true);
        checkBox->setCheckState(state);
        checkBox->setToolTip(
            QString("Enable %1 for %2 in %3.\nIf it's partially checked, the global setting in Code Edit will be used.")
                .arg(name.toLower())
                .arg(parenthesis())
                .arg(lang));
        connect(checkBox, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));
        checkBoxesLayout->addWidget(checkBox);
    };

    addOption(autoCompleteCheckBox, "Auto Complete", autoComplete);
    addOption(autoRemoveCheckBox, "Auto Remove", autoRemove);
    addOption(tabJumpOutCheckBox, "Tab Jump Out", tabJumpOut);

    stretchLayout->addStretch();

    mainLayout->addStretch();
}

QVariantList ParenthesisWidget::toList() const
{
    return {left, right, autoCompleteCheckBox->checkState(), autoRemoveCheckBox->checkState(),
            tabJumpOutCheckBox->checkState()};
}

QString ParenthesisWidget::parenthesis() const
{
    return QString(left) + right;
}

ParenthesesPage::ParenthesesPage(const QString &language, QWidget *parent) : PreferencesPage(parent), lang(language)
{
    setTitle(language + " Parentheses");

    splitter = new QSplitter();
    addWidget(splitter);

    leftWidget = new QWidget();
    splitter->addWidget(leftWidget);

    leftLayout = new QVBoxLayout(leftWidget);

    listWidget = new QListWidget();
    connect(listWidget, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(switchToParenthesis(QListWidgetItem *)));
    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(switchToParenthesis(QListWidgetItem *)));
    leftLayout->addWidget(listWidget);

    buttonsLayout = new QHBoxLayout();
    leftLayout->addLayout(buttonsLayout);

    addButton = new QPushButton("Add");
    addButton->setShortcut({"Ctrl+N"});
    connect(addButton, SIGNAL(clicked()), this, SLOT(addParenthesis()));
    buttonsLayout->addWidget(addButton);

    delButton = new QPushButton("Del");
    delButton->setShortcut({"Ctrl+W"});
    delButton->setEnabled(false);
    connect(delButton, SIGNAL(clicked()), this, SLOT(deleteCurrentParenthesis()));
    buttonsLayout->addWidget(delButton);

    stackedWidget = new QStackedWidget();
    splitter->addWidget(stackedWidget);

    noParenthesisWidget = new QWidget();
    stackedWidget->addWidget(noParenthesisWidget);
    stackedWidget->setCurrentWidget(noParenthesisWidget);

    noParenthesisLayout = new QVBoxLayout(noParenthesisWidget);

    noParenthesisLayout->addStretch();

    noParenthesisStretchLayout = new QHBoxLayout();
    noParenthesisLayout->addLayout(noParenthesisStretchLayout);

    noParenthesisStretchLayout->addStretch();

    noParenthesisLabel = new QLabel("No Parenthesis Selected");
    noParenthesisStretchLayout->addWidget(noParenthesisLabel);

    noParenthesisStretchLayout->addStretch();

    noParenthesisLayout->addStretch();

    splitter->setSizes({100000, 200000});
}

bool ParenthesesPage::areSettingsChanged()
{
    return SettingsManager::get(settingsKey()).toList() != toList();
}

void ParenthesesPage::makeUITheSameAsDefault()
{
    loadList(SettingsManager::get(settingsKey(), true).toList());
}

void ParenthesesPage::makeUITheSameAsSettings()
{
    loadList(SettingsManager::get(settingsKey()).toList());
}

void ParenthesesPage::makeSettingsTheSameAsUI()
{
    SettingsManager::set(settingsKey(), toList());
}

void ParenthesesPage::addParenthesis()
{
    QString parenthesis;

    while (parenthesis.length() != 2)
    {
        parenthesis = QInputDialog::getText(this, "New Parenthesis", "Enter a parenthesis (e.g. {}):");
        if (parenthesis.isNull())
            return;
    }

    addParenthesis(parenthesis[0], parenthesis[1]);
}

void ParenthesesPage::deleteCurrentParenthesis()
{
    int index = stackedWidget->currentIndex();
    Q_ASSERT(index > 0);
    auto currentParenthesis = parenthesisWidget(index)->parenthesis();
    if (QMessageBox::question(this, "Delete Parenthesis",
                              QString("Do you really want to delete the parenthesis %1?").arg(currentParenthesis),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;
    if (index + 1 < stackedWidget->count())
        switchToParenthesis(index + 1);
    else
        switchToParenthesis(index - 1);
    deleteParenthesis(index);
}

void ParenthesesPage::switchToParenthesis(QListWidgetItem *item)
{
    if (item == nullptr)
        switchToParenthesis(0);
    else
        switchToParenthesis(listWidget->row(item) + 1);
}

void ParenthesesPage::addParenthesis(QChar left, QChar right, Qt::CheckState autoComplete, Qt::CheckState autoRemove,
                                     Qt::CheckState tabJumpOut)
{
    auto parenthesis = new ParenthesisWidget(lang, left, right, autoComplete, autoRemove, tabJumpOut, this);
    connect(parenthesis, SIGNAL(changed()), this, SLOT(updateButtons()));
    stackedWidget->addWidget(parenthesis);
    listWidget->addItem(parenthesis->parenthesis());
    switchToParenthesis(stackedWidget->count() - 1);
    updateButtons();
}

void ParenthesesPage::deleteParenthesis(int index)
{
    Q_ASSERT(index > 0 && index < stackedWidget->count());
    auto widget = stackedWidget->widget(index);
    stackedWidget->removeWidget(widget);
    delete widget;
    auto item = listWidget->item(index - 1);
    delete item;
    updateButtons();
}

void ParenthesesPage::switchToParenthesis(int index)
{
    Q_ASSERT(index >= 0 && index < stackedWidget->count());
    stackedWidget->setCurrentIndex(index);
    delButton->setEnabled(index > 0);
}

ParenthesisWidget *ParenthesesPage::parenthesisWidget(int index) const
{
    Q_ASSERT(index > 0 && index < stackedWidget->count());
    return qobject_cast<ParenthesisWidget *>(stackedWidget->widget(index));
}

QString ParenthesesPage::settingsKey() const
{
    return lang + "/Parentheses";
}

QVariantList ParenthesesPage::toList() const
{
    QVariantList list;
    for (int i = 1; i < stackedWidget->count(); ++i)
        list.push_back(parenthesisWidget(i)->toList());
    return list;
}

void ParenthesesPage::loadList(const QVariantList &list)
{
    while (stackedWidget->count() > 1)
        deleteParenthesis(1);
    for (auto var : list)
    {
        auto li = var.toList();
        if (li.length() != 5)
        {
            LOG_ERR(INFO_OF(li.length()));
            continue;
        }
        auto left = li[0].toChar();
        auto right = li[1].toChar();
        auto autoComplete = Qt::CheckState(li[2].toInt());
        auto autoRemove = Qt::CheckState(li[3].toInt());
        auto tabJumpOut = Qt::CheckState(li[4].toInt());
        addParenthesis(left, right, autoComplete, autoRemove, tabJumpOut);
    }
}
