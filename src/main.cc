/*
 * Copyright (C) 2019 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CPEditor.
 *
 * CPEditor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CPEditor behaves in unexpected way and
 * causes your ratings to go down and or loose any important contest.
 *
 * Believe Software is "Software" and it isn't not immune to bugs.
 *
 */

#include <QApplication>

#include "appwindow.hpp"
#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();
    QVector<MainWindow *> windows;
    if (args.size() > 1)
        for (int t = 1; t < args.size(); t++)
            windows.push_back(new MainWindow(t - 1, args[t]));

    AppWindow w(windows);
    w.setWindowTitle("CP Editor: Competitive Programmers Editor");
    w.show();
    return a.exec();
}
