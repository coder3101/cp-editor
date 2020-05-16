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

#ifndef FILEUTIL_HPP
#define FILEUTIL_HPP

#include <QStringList>

class MessageLogger;

namespace Util
{
const static QStringList cppSuffix = {"cpp", "hpp", "h", "cc", "cxx", "c"};
const static QStringList javaSuffix = {"java"};
const static QStringList pythonSuffix = {"py", "py3"};
#ifdef Q_OS_WIN
const static QString exeSuffix = ".exe";
#else
const static QString exeSuffix = "";
#endif

QString fileNameFilter(bool cpp, bool java, bool python);

bool saveFile(const QString &path, const QString &content, const QString &head = "Save File", bool safe = true,
              MessageLogger *log = nullptr, bool createDirectory = false);

/**
 * @brief get the content of a file
 * @param path the path to the file
 * @param head the head of the log
 * @param log the MessageLogger to display the messages
 * @param notExistWarning whether to make a warning when the file doesn't exist
 * @returns a null QString if failed to open the file, the content of the file otherwise
 * @note The warning is sent to *log* only if the file exists but we can't open it.
 * @note If the content of the file is a null QString, this function will return a non-null empty QString.
 */
QString readFile(const QString &path, const QString &head = "Read File", MessageLogger *log = nullptr,
                 bool notExistWarning = false);
} // namespace Util

#endif // FILEUTIL_HPP
