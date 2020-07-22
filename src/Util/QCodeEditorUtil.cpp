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

#include "Util/QCodeEditorUtil.hpp"
#include "Core/EventLogger.hpp"
#include "Extensions/EditorTheme.hpp"
#include "Settings/SettingsManager.hpp"
#include "generated/SettingsHelper.hpp"
#include <QCodeEditor>

#include <QDebug>
#include <Util/Singleton.hpp>
#include <definition.h>
#include <syntaxhighlighter.h>
#include <theme.h>

namespace Util
{
void applySettingsToEditor(KSyntaxHighlighting::SyntaxHighlighter *highlighter, QCodeEditor *editor,
                           const QString &language)
{
    LOG_INFO("Applying settings to QCodeEditor");

    editor->setTabReplace(SettingsHelper::isReplaceTabs());
    editor->setTabReplaceSize(SettingsHelper::getTabWidth());
    editor->setAutoIndentation(SettingsHelper::isAutoIndent());

    editor->setFont(SettingsHelper::getEditorFont());

    const int tabStop = SettingsHelper::getTabWidth();
    QFontMetrics metric(editor->font());
    editor->setTabReplaceSize(tabStop);

    if (SettingsHelper::isWrapText())
        editor->setWordWrapMode(QTextOption::WordWrap);
    else
        editor->setWordWrapMode(QTextOption::NoWrap);

    auto style = Extensions::EditorTheme::query(SettingsHelper::getEditorTheme());
    if (!style)
        style = Extensions::EditorTheme::query("Light");
    editor->setSyntaxStyle(style);

    editor->setExtraBottomMargin(SettingsHelper::isExtraBottomMargin());

    if (language.isEmpty())
        return;

    highlighter->setDefinition(Util::Singleton::getSyntaxHighlightingRepository().definitionForName(language));
    highlighter->setTheme(
        Util::Singleton::getSyntaxHighlightingRepository().theme(SettingsHelper::getSyntaxHighlightingStyle()));
    highlighter->rehighlight();

    if (language == "Python")
    {
        editor->setCompleter(nullptr);
    }
    else if (language == "Java")
    {
        editor->setCompleter(nullptr);
    }
    else
    {
        editor->setCompleter(nullptr);
    }

    QVector<QCodeEditor::Parenthesis> parentheses;

    auto list = SettingsManager::get(language + "/Parentheses").toList();

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

        auto getFlag = [](Qt::CheckState state, bool def) {
            switch (state)
            {
            case Qt::Checked:
                return true;
            case Qt::PartiallyChecked:
                return def;
            case Qt::Unchecked:
                return false;
            default:
                Q_UNREACHABLE();
            }
        };

        bool autoComplete = getFlag(Qt::CheckState(li[2].toInt()), SettingsHelper::isAutoCompleteParentheses());
        bool autoRemove = getFlag(Qt::CheckState(li[3].toInt()), SettingsHelper::isAutoRemoveParentheses());
        bool tabJumpOut = getFlag(Qt::CheckState(li[4].toInt()), SettingsHelper::isTabJumpOutParentheses());

        parentheses.push_back({left, right, autoComplete, autoRemove, tabJumpOut});
    }

    editor->setParentheses(parentheses);
}
} // namespace Util
