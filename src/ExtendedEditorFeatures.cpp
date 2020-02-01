/*
 * Copyright (C) 2019-2020 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CPEditor.
 *
 * CPEditor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CPEditor behaves in an unexpected way and
 * causes your bad performance and / or lose any contest.
 *
 * Believe Software is "Software" and it isn't immune to bugs.
 *
 */

#include <ExtendedEditorFeatures.hpp>
#include <QDebug>
#include <QString>

namespace Core
{

ExtendedQCodeEditorFeatures::ExtendedQCodeEditorFeatures(QCodeEditor *editor, int tabSize)
{
    this->editor_ = editor;
    this->tabSize_ = tabSize;
}

QString ExtendedQCodeEditorFeatures::language()
{
    return this->language_;
}

int ExtendedQCodeEditorFeatures::tabSize()
{
    return this->tabSize_;
}

void ExtendedQCodeEditorFeatures::setLanguage(QString lang)
{
    this->language_ = lang;
}

void ExtendedQCodeEditorFeatures::setTabSize(int tab)
{
    this->tabSize_ = tab;
}

void ExtendedQCodeEditorFeatures::setEditor(QCodeEditor *editor)
{
    this->editor_ = editor;
}

void ExtendedQCodeEditorFeatures::toggleSelectionComment(bool _)
{
    if (editor_->textCursor().hasSelection())
    {
        // todo(coder3101): This just checks if first line is comment and then works its way out.
        // make it toggle line by line instead
        auto sps = editor_->textCursor();
        sps.setPosition(sps.selectionStart());
        sps.select(QTextCursor::SelectionType::LineUnderCursor);
        auto text = sps.selectedText();
        if (text.startsWith("// ") || text.startsWith("# "))
        {

            if (language_ != "Python")
            {

                selectionRemoveFirstCharIf_({"/", " "});
                selectionRemoveFirstCharIf_({"/", " "});
                selectionRemoveFirstCharIf_({"/", " "});
            }
            else
            {
                selectionRemoveFirstCharIf_({"#", " "});
                selectionRemoveFirstCharIf_({"#", " "});
            }
        }
        else
        {
            if (language_ == "Python")
                selectionPrepend_("# ");
            else
                selectionPrepend_("// ");
        }
    }
    else
    {
        // This replacement can be done via regex too.
        auto cursor = editor_->textCursor();
        cursor.select(QTextCursor::SelectionType::LineUnderCursor);
        QString thisLine = cursor.selectedText();
        if (language_ == "Python")
        {
            if (thisLine.startsWith("# "))
                thisLine.remove("# ");
            else
                thisLine.prepend("# ");
            cursor.removeSelectedText();
            cursor.insertText(thisLine);
        }
        else
        {
            if (thisLine.startsWith("// "))
                thisLine.remove("// ");
            else
                thisLine.prepend("// ");
            cursor.removeSelectedText();
            cursor.insertText(thisLine);
        }
    }
}

bool ExtendedQCodeEditorFeatures::selectionTab()
{
    return selectionPrepend_("\t");
}

void ExtendedQCodeEditorFeatures::selectionUnTab()
{
    selectionRemoveFirstCharIf_({"\t", " "});
}

void ExtendedQCodeEditorFeatures::moveSelectionUp()
{
    if (editor_->textCursor().hasSelection())
    {
        // Not working. The selection cursor behaves in differently when selection is swapped with an empty line.
        /*
        auto cursor = editor_->textCursor();
        int start = cursor.selectionStart();
        int stop = cursor.selectionEnd();

        if (!cursor.atStart())
        {
            QTextCursor fullCursor = cursor;
            fullCursor.clearSelection();
            fullCursor.setPosition(start);
            fullCursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
            fullCursor.setPosition(stop, QTextCursor::KeepAnchor);
            fullCursor.movePosition(QTextCursor::MoveOperation::EndOfLine, QTextCursor::KeepAnchor);

            QString selection = fullCursor.selectedText();
            fullCursor.removeSelectedText();

            QTextCursor upCursor = cursor;
            upCursor.clearSelection();
            upCursor.movePosition(QTextCursor::MoveOperation::Up);

            upCursor.select(QTextCursor::SelectionType::LineUnderCursor);
            QString thatLine = upCursor.selectedText();


            fullCursor.insertText(thatLine);
            upCursor.removeSelectedText();
            upCursor.insertText(selection);

            int last_pos = upCursor.selectionEnd();
            upCursor.clearSelection();
            fullCursor.clearSelection();

            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::MoveOperation::Up);
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.setPosition(last_pos, QTextCursor::KeepAnchor);


            editor_->setTextCursor(cursor);
        } */
    }
    else
    {
        auto cursor = editor_->textCursor();

        if (!cursor.atStart())
        {
            cursor.select(QTextCursor::SelectionType::LineUnderCursor);
            QString thisLine = cursor.selectedText();
            cursor.removeSelectedText();

            QTextCursor upCursor = cursor;
            upCursor.movePosition(QTextCursor::MoveOperation::Up);
            upCursor.select(QTextCursor::SelectionType::LineUnderCursor);
            QString thatLine = upCursor.selectedText();

            cursor.insertText(thatLine);
            upCursor.removeSelectedText();
            upCursor.insertText(thisLine);

            editor_->setTextCursor(upCursor);
        }
    }
}
void ExtendedQCodeEditorFeatures::moveSelectionDown()
{
    if (editor_->textCursor().hasSelection())
    {
        // Not implemented
    }
    else
    {
        auto cursor = editor_->textCursor();

        if (!cursor.atEnd())
        {
            cursor.select(QTextCursor::SelectionType::LineUnderCursor);
            QString thisLine = cursor.selectedText();
            cursor.removeSelectedText();

            QTextCursor downCursor = cursor;
            downCursor.movePosition(QTextCursor::MoveOperation::Down);
            downCursor.select(QTextCursor::SelectionType::LineUnderCursor);
            QString thatLine = downCursor.selectedText();

            cursor.insertText(thatLine);
            downCursor.removeSelectedText();
            downCursor.insertText(thisLine);

            editor_->setTextCursor(downCursor);
        }
    }
}

bool ExtendedQCodeEditorFeatures::selectionPrepend_(QString content)
{
    if (editor_->textCursor().hasSelection())
    {
        auto cursor = editor_->textCursor();
        QTextCursor originalCursor = cursor;
        originalCursor.clearSelection();
        originalCursor.setPosition(cursor.selectionStart());
        originalCursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
        originalCursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
        originalCursor.movePosition(QTextCursor::MoveOperation::EndOfLine, QTextCursor::KeepAnchor);

        int start = cursor.selectionStart();
        int stop = cursor.selectionEnd();

        cursor.setPosition(start);

        cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);

        editor_->setTextCursor(cursor);
        editor_->insertPlainText(content);

        cursor.movePosition(QTextCursor::MoveOperation::Down);
        int i = content.size();
        while (cursor.position() < stop + i)
        {
            i += content.size();
            editor_->setTextCursor(cursor);
            editor_->insertPlainText(content);
            cursor.movePosition(QTextCursor::MoveOperation::Down);
        }

        editor_->setTextCursor(originalCursor);

        return true;
    }
    return false;
}

bool ExtendedQCodeEditorFeatures::selectionRemoveFirstCharIf_(QStringList deleteText)
{
    if (editor_->textCursor().hasSelection())
    {
        auto cursor = editor_->textCursor();
        QTextCursor originalCursor = cursor;

        int start = cursor.selectionStart();
        int stop = cursor.selectionEnd();
        cursor.clearSelection();

        cursor.setPosition(start);

        cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
        cursor.movePosition(QTextCursor::MoveOperation::Right);

        QTextCursor tmp;
        int i = 0;
        tmp = cursor;
        tmp.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::KeepAnchor);
        if (deleteText.contains(tmp.selectedText()))
        {
            i++;
            cursor.deletePreviousChar();
        }

        editor_->setTextCursor(cursor);

        cursor.movePosition(QTextCursor::MoveOperation::Down);
        cursor.movePosition(QTextCursor::MoveOperation::Right);

        while (cursor.position() < stop - i)
        {
            tmp = cursor;
            tmp.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::KeepAnchor);
            if (deleteText.contains(tmp.selectedText()))
            {
                cursor.deletePreviousChar();
                i++;
            }

            editor_->setTextCursor(cursor);

            cursor.movePosition(QTextCursor::MoveOperation::Down);
            cursor.movePosition(QTextCursor::MoveOperation::Right);
        }

        editor_->setTextCursor(originalCursor);
        return true;
    }
    return false;
}

} // namespace Core