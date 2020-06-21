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

#include "mainwindow.hpp"

#include "Core/Checker.hpp"
#include "Core/Compiler.hpp"
#include "Core/EventLogger.hpp"
#include "Core/MessageLogger.hpp"
#include "Core/Runner.hpp"
#include "Extensions/CFTool.hpp"
#include "Extensions/ClangFormatter.hpp"
#include "Extensions/CompanionServer.hpp"
#include "Settings/FileProblemBinder.hpp"
#include "Util/FileUtil.hpp"
#include "Util/QCodeEditorUtil.hpp"
#include "Widgets/TestCases.hpp"
#include <QCodeEditor>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFontDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QRegularExpression>
#include <QSaveFile>
#include <QScrollBar>
#include <QShortcut>
#include <QSyntaxStyle>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTimer>
#include <generated/SettingsHelper.hpp>
#include <generated/version.hpp>

#include "../ui/ui_mainwindow.h"

// ***************************** RAII  ****************************

MainWindow::MainWindow(const QString &fileOpen, int index, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), untitledIndex(index), fileWatcher(new QFileSystemWatcher(this)),
      autoSaveTimer(new QTimer(this))
{
    LOG_INFO(INFO_OF(fileOpen) << INFO_OF(index));

    ui->setupUi(this);
    setupCore();
    setTestCases();
    setEditor();
    connect(fileWatcher, SIGNAL(fileChanged(const QString &)), this, SLOT(onFileWatcherChanged(const QString &)));
    connect(
        autoSaveTimer, &QTimer::timeout, autoSaveTimer, [this] { saveFile(IgnoreUntitled, tr("Auto Save"), false); },
        Qt::DirectConnection);
    applySettings("", true);
    loadFile(fileOpen);
    if (testcases->count() == 0)
        testcases->addTestCase();
    QTimer::singleShot(0, [this] { setLanguage(language); }); // See issue #187 for more information
}

MainWindow::~MainWindow()
{
    killProcesses();

    if (cftool != nullptr)
        delete cftool;
    if (tmpDir != nullptr)
        delete tmpDir;

    delete ui;
    delete autoSaveTimer;
    delete testcases;
    delete formatter;
    delete fileWatcher;
    delete editor;
    delete log;
}

// ************************* RAII HELPER *****************************

void MainWindow::setTestCases()
{
    testcases = new Widgets::TestCases(log, this);
    ui->test_cases_layout->addWidget(testcases);
    connect(testcases, SIGNAL(checkerChanged()), this, SLOT(updateChecker()));
    connect(testcases, SIGNAL(requestRun(int)), this, SLOT(runTestCase(int)));
}

void MainWindow::setEditor()
{
    editor = new QCodeEditor();
    editor->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    editor->setAcceptDrops(false);

    ui->verticalLayout_8->addWidget(editor);

    connect(editor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    // cursorPositionChanged() does not imply selectionChanged() if you press Left with
    // a selection (and the cursor is at the begin of the selection)
    connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorInfo()));
    connect(editor, SIGNAL(selectionChanged()), this, SLOT(updateCursorInfo()));
}

void MainWindow::setupCore()
{
    log = new MessageLogger();
    log->setContainer(ui->compiler_edit);
    formatter = new Extensions::ClangFormatter(SettingsHelper::getClangFormatPath(),
                                               SettingsHelper::getClangFormatStyle(), log);
}

void MainWindow::compile()
{
    if (SettingsHelper::isSaveFileOnCompilation())
        saveFile(IgnoreUntitled, tr("Compiler"), true);

    killProcesses();

    compiler = new Core::Compiler();

    auto path = tmpPath();
    if (path.isEmpty())
        return;

    if (language == "Python")
    {
        onCompilationFinished("");
        return;
    }
    else if (language != "C++" && language != "Java")
    {
        log->warn(tr("Compiler"), tr("Please set the language"));
        return;
    }
    connect(compiler, SIGNAL(compilationStarted()), this, SLOT(onCompilationStarted()));
    connect(compiler, SIGNAL(compilationFinished(const QString &)), this, SLOT(onCompilationFinished(const QString &)));
    connect(compiler, SIGNAL(compilationErrorOccurred(const QString &)), this,
            SLOT(onCompilationErrorOccurred(const QString &)));
    connect(compiler, SIGNAL(compilationKilled()), this, SLOT(onCompilationKilled()));
    compiler->start(path, filePath, SettingsManager::get(QString("%1/Compile Command").arg(language)).toString(),
                    language);
}

void MainWindow::run()
{
    if (SettingsHelper::isSaveFileOnExecution())
        saveFile(IgnoreUntitled, tr("Runner"), true);

    LOG_INFO("Requesting run of testcases");
    killProcesses();
    testcases->clearOutput();

    if (!QStringList({"C++", "Java", "Python"}).contains(language))
    {
        log->warn(tr("Runner"), tr("Wrong language, please set the language"));
        return;
    }

    for (int i = 0; i < testcases->count(); ++i)
    {
        if ((!testcases->input(i).trimmed().isEmpty() || SettingsHelper::isRunOnEmptyTestcase()) &&
            testcases->isShow(i))
        {
            run(i);
        }
    }

    if (runner.empty())
        log->warn(tr("Runner"), tr("All inputs are empty, nothing to run"));
}

void MainWindow::run(int index)
{
    auto tmp = new Core::Runner(index);
    connect(tmp, SIGNAL(runStarted(int)), this, SLOT(onRunStarted(int)));
    connect(tmp, SIGNAL(runFinished(int, const QString &, const QString &, int, int)), this,
            SLOT(onRunFinished(int, const QString &, const QString &, int, int)));
    connect(tmp, SIGNAL(failedToStartRun(int, const QString &)), this, SLOT(onFailedToStartRun(int, const QString &)));
    connect(tmp, SIGNAL(runTimeout(int)), this, SLOT(onRunTimeout(int)));
    connect(tmp, SIGNAL(runOutputLimitExceeded(int, const QString &)), this,
            SLOT(onRunOutputLimitExceeded(int, const QString &)));
    connect(tmp, SIGNAL(runKilled(int)), this, SLOT(onRunKilled(int)));
    tmp->run(tmpPath(), filePath, language, SettingsManager::get(QString("%1/Run Command").arg(language)).toString(),
             SettingsManager::get(QString("%1/Run Arguments").arg(language)).toString(), testcases->input(index),
             SettingsHelper::getTimeLimit());
    runner.push_back(tmp);
}

void MainWindow::runTestCase(int index)
{
    LOG_INFO(INFO_OF(index));
    killProcesses();
    testcases->clearOutput();
    log->clear();

    if (!QStringList({"C++", "Java", "Python"}).contains(language))
    {
        log->warn(tr("Runner"), tr("Wrong language, please set the language"));
        return;
    }

    run(index);
}

void MainWindow::loadTests()
{
    if (!isUntitled() && SettingsHelper::isSaveTests())
        testcases->loadFromSavedFiles(filePath);
}

void MainWindow::saveTests(bool safe)
{
    if (!isUntitled() && SettingsHelper::isSaveTests())
        testcases->saveToFiles(filePath, safe);
}

void MainWindow::setCFToolUI()
{
    if (submitToCodeforces == nullptr)
    {
        submitToCodeforces = new QPushButton(tr("Submit"), this);
        cftool = new Extensions::CFTool(cftoolPath, log);
        connect(cftool, SIGNAL(requestToastMessage(const QString &, const QString &)), this,
                SIGNAL(requestToastMessage(const QString &, const QString &)));
        ui->compile_and_run_buttons->addWidget(submitToCodeforces);
        connect(submitToCodeforces, &QPushButton::clicked, this, [this] {
            emit confirmTriggered(this);
            auto response = QMessageBox::warning(
                this, tr("Sure to submit"),
                tr("Are you sure you want to submit this solution to Codeforces?\n\n URL: %1\n Language: %2")
                    .arg(problemURL, language),
                QMessageBox::Yes | QMessageBox::No);

            if (response == QMessageBox::Yes)
            {
                auto path = tmpPath();
                if (path.isEmpty())
                {
                    QMessageBox::warning(this, tr("CF Tool"),
                                         tr("Failed to save the temp file, and the solution is not submitted."));
                }
                else
                {
                    log->clear();
                    cftool->submit(tmpPath(), problemURL);
                }
            }
        });
    }
    if (!Extensions::CFTool::check(cftoolPath))
    {
        submitToCodeforces->setEnabled(false);
        log->error(tr("CF Tool"),
                   tr("You will not be able to submit code to Codeforces because CF Tool is not installed or is "
                      "not on SYSTEM PATH. You can set it manually in settings."));
    }
}

int MainWindow::getUntitledIndex() const
{
    return untitledIndex;
}

QString MainWindow::getFileName() const
{
    if (!isUntitled())
        return QFileInfo(filePath).fileName();
    if (!problemURL.isEmpty())
        return QRegularExpression(R"(.*/([^\?#].*?)/?$)").match(problemURL).captured(1);
    return tr("Untitled-%1").arg(untitledIndex);
}

QString MainWindow::getFilePath() const
{
    return filePath;
}

QString MainWindow::getProblemURL() const
{
    return problemURL;
}

QString MainWindow::getCompleteTitle() const
{
    if (!isUntitled())
        return filePath;
    else if (!problemURL.isEmpty())
        return problemURL;
    else
        return getFileName();
}

QString MainWindow::getTabTitle(bool complete, bool star, int removeLength)
{
    QString tabTitle;
    if (!complete || (isUntitled() && problemURL.isEmpty()))
        tabTitle = getFileName();
    else if (!isUntitled())
        tabTitle = getFileName() + " - " + QFileInfo(filePath).path().remove(0, removeLength);
    else
        tabTitle = getFileName() + " - " + getProblemURL().remove(0, removeLength);
    if (star && isTextChanged())
        tabTitle += " *";
    return tabTitle;
}

QCodeEditor *MainWindow::getEditor() const
{
    return editor;
}

bool MainWindow::isUntitled() const
{
    return filePath.isEmpty();
}

void MainWindow::setFilePath(QString path)
{
    if (QFile::exists(path))
        path = QFileInfo(path).canonicalFilePath();
    LOG_INFO(INFO_OF(path));
    if (filePath == path)
    {
        updateWatcher();
        return;
    }
    filePath = path;
    FileProblemBinder::set(path, problemURL);
    emit editorFileChanged();
    updateWatcher();
}

void MainWindow::setProblemURL(const QString &url)
{
    LOG_INFO(INFO_OF(url));
    if (problemURL == url)
        return;
    problemURL = url;
    FileProblemBinder::set(filePath, url);
    if (problemURL.contains("codeforces.com"))
        setCFToolUI();
    emit editorFileChanged();
}

void MainWindow::setUntitledIndex(int index)
{
    untitledIndex = index;
}

#define FROMSTATUS(x) x = status[#x]
MainWindow::EditorStatus::EditorStatus(const QMap<QString, QVariant> &status)
{
    LOG_INFO("Window status from map");
    FROMSTATUS(isLanguageSet).toInt();
    FROMSTATUS(filePath).toString();
    FROMSTATUS(savedText).toString();
    FROMSTATUS(problemURL).toString();
    FROMSTATUS(editorText).toString();
    FROMSTATUS(language).toString();
    FROMSTATUS(editorCursor).toInt();
    FROMSTATUS(editorAnchor).toInt();
    FROMSTATUS(horizontalScrollBarValue).toInt();
    FROMSTATUS(verticalScrollbarValue).toInt();
    FROMSTATUS(untitledIndex).toInt();
    FROMSTATUS(checkerIndex).toInt();
    FROMSTATUS(input).toStringList();
    FROMSTATUS(expected).toStringList();
    FROMSTATUS(customCheckers).toStringList();
    FROMSTATUS(testcasesIsShow).toList();
}
#undef FROMSTATUS

#define TOSTATUS(x) status[#x] = x
QMap<QString, QVariant> MainWindow::EditorStatus::toMap() const
{
    LOG_INFO("Window status to hashmap");
    QMap<QString, QVariant> status;
    TOSTATUS(isLanguageSet);
    TOSTATUS(filePath);
    TOSTATUS(savedText);
    TOSTATUS(problemURL);
    TOSTATUS(editorText);
    TOSTATUS(language);
    TOSTATUS(editorCursor);
    TOSTATUS(editorAnchor);
    TOSTATUS(horizontalScrollBarValue);
    TOSTATUS(verticalScrollbarValue);
    TOSTATUS(untitledIndex);
    TOSTATUS(checkerIndex);
    TOSTATUS(input);
    TOSTATUS(expected);
    TOSTATUS(customCheckers);
    TOSTATUS(testcasesIsShow);
    return status;
}
#undef TOSTATUS

MainWindow::EditorStatus MainWindow::toStatus() const
{
    EditorStatus status;

    status.isLanguageSet = isLanguageSet;
    status.filePath = filePath;
    status.problemURL = problemURL;
    status.language = language;
    status.untitledIndex = untitledIndex;
    status.checkerIndex = testcases->checkerIndex();
    status.customCheckers = testcases->customCheckers();
    status.editorText = editor->toPlainText();
    status.editorCursor = editor->textCursor().position();
    status.editorAnchor = editor->textCursor().anchor();
    status.horizontalScrollBarValue = editor->horizontalScrollBar()->value();
    status.verticalScrollbarValue = editor->verticalScrollBar()->value();
    status.input = testcases->inputs();
    status.expected = testcases->expecteds();
    for (int i = 0; i < testcases->count(); ++i)
        status.testcasesIsShow.push_back(testcases->isShow(i));

    return status;
}

void MainWindow::loadStatus(const EditorStatus &status)
{
    LOG_INFO("Requesting loadStatus");
    setProblemURL(status.problemURL);
    if (status.isLanguageSet)
        setLanguage(status.language);
    untitledIndex = status.untitledIndex;
    testcases->addCustomCheckers(status.customCheckers);
    testcases->setCheckerIndex(status.checkerIndex);
    setFilePath(status.filePath);
    savedText = status.savedText;
    editor->setPlainText(status.editorText);
    auto cursor = editor->textCursor();
    cursor.setPosition(status.editorAnchor);
    cursor.setPosition(status.editorCursor, QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);
    editor->horizontalScrollBar()->setValue(status.horizontalScrollBarValue);
    editor->verticalScrollBar()->setValue(status.verticalScrollbarValue);
    testcases->loadStatus(status.input, status.expected);
    for (int i = 0; i < status.testcasesIsShow.count() && i < testcases->count(); ++i)
        testcases->setShow(i, status.testcasesIsShow[i].toBool());
}

void MainWindow::applyCompanion(const Extensions::CompanionData &data)
{
    LOG_INFO("Requesting apply from companion");

    if (isUntitled() && !isTextChanged())
    {
        QString meta = data.toMetaString();
        meta.prepend("\n");
        meta.append("Powered by CP Editor (https://github.com/cpeditor/cpeditor)");

        if (language == "Python")
            meta.replace('\n', "\n# ");
        else
            meta.replace('\n', "\n// ");

        meta.append("\n\n");

        auto cursor = editor->textCursor();
        int cursorPos = cursor.position();
        editor->setPlainText(meta + editor->toPlainText());
        cursor.setPosition(cursorPos + meta.length());
        editor->setTextCursor(cursor);
    }

    testcases->clear();

    for (int i = 0; i < data.testcases.size(); ++i)
        testcases->addTestCase(data.testcases[i].input, data.testcases[i].output);

    setProblemURL(data.url);
}

void MainWindow::applySettings(const QString &pagePath, bool shouldPerformDigonistic)
{
    LOG_INFO(INFO_OF(pagePath) << BOOL_INFO_OF(shouldPerformDigonistic));

    if (pagePath.isEmpty() || pagePath == "Extensions/Clang Format")
    {
        formatter->updateBinary(SettingsHelper::getClangFormatPath());
        formatter->updateStyle(SettingsHelper::getClangFormatStyle());
    }

    if (pagePath.isEmpty() || pagePath == "Extensions/CF Tool")
    {
        cftoolPath = SettingsHelper::getCFPath();

        if (cftool != nullptr && Extensions::CFTool::check(cftoolPath))
        {
            cftool->updatePath(cftoolPath);
            if (submitToCodeforces != nullptr)
                submitToCodeforces->setEnabled(true);
        }
    }

    if (pagePath.isEmpty() || pagePath == "Code Edit" || pagePath == "Appearance" ||
        pagePath == QString("Language/%1/%1 Parentheses").arg(language))
        Util::applySettingsToEditor(editor, language);

    if (!isLanguageSet && (pagePath.isEmpty() || pagePath == "Language/General"))
    {
        setLanguage(SettingsHelper::getDefaultLanguage());
    }

    if (shouldPerformDigonistic && (pagePath.isEmpty() || pagePath == QString("Language/%1/%1 Commands").arg(language)))
    {
        performCompileAndRunDiagonistics();
    }

    if (pagePath.isEmpty() || pagePath == "Appearance")
    {
        ui->compiler_edit->setFont(SettingsHelper::getMessageLoggerFont());
        testcases->setTestCaseEditFont(SettingsHelper::getTestCasesFont());
        if (SettingsHelper::isShowCompileAndRunOnly())
        {
            ui->compile->hide();
            ui->runOnly->hide();
        }
        else
        {
            ui->compile->show();
            ui->runOnly->show();
        }
    }

    if (pagePath.isEmpty() || pagePath == "Language/C++/C++ Commands")
        updateChecker();

    if (pagePath.isEmpty() || pagePath == "Actions/Auto Save")
    {
        if (SettingsHelper::isAutoSave())
        {
            autoSaveTimer->setSingleShot(SettingsHelper::getAutoSaveIntervalType() != "Without modification");
            autoSaveTimer->setInterval(SettingsHelper::getAutoSaveInterval());
            autoSaveTimer->start();
        }
        else
            autoSaveTimer->stop();
    }
}

bool MainWindow::save(bool force, const QString &head, bool safe)
{
    LOG_INFO("Save " << BOOL_INFO_OF(force) << INFO_OF(head) << BOOL_INFO_OF(safe));
    return saveFile(force ? AlwaysSave : IgnoreUntitled, head, safe);
}

void MainWindow::saveAs()
{
    LOG_INFO("Save as clicked");
    saveFile(SaveAs, tr("Save as"), true);
}

void MainWindow::on_compile_clicked()
{
    LOG_INFO("Compile button clicked");
    compileOnly();
}

void MainWindow::on_runOnly_clicked()
{
    LOG_INFO("Run only button clicked");
    runOnly();
}

void MainWindow::on_run_clicked()
{
    LOG_INFO("Run button clicked");
    compileAndRun();
}

void MainWindow::compileOnly()
{
    LOG_INFO("Requesting Compile Only");
    emit compileOrRunTriggered();
    afterCompile = Nothing;
    log->clear();
    compile();
}

void MainWindow::runOnly()
{
    LOG_INFO("Requesting Run only");
    emit compileOrRunTriggered();
    log->clear();
    run();
}

void MainWindow::compileAndRun()
{
    LOG_INFO("Requested Compile and Run");
    emit compileOrRunTriggered();
    afterCompile = Run;
    log->clear();
    compile();
}

void MainWindow::formatSource()
{
    LOG_INFO("Requested code format");
    formatter->format(editor, filePath, language, true);
}

void MainWindow::setLanguage(const QString &lang)
{
    LOG_INFO(INFO_OF(lang));
    if (!QFile::exists(filePath))
    {
        QString templateContent;
        if (!language.isEmpty())
            templateContent = Util::readFile(SettingsManager::get(QString("%1/Template Path").arg(language)).toString(),
                                             tr("Open %1 Template").arg(language), log);
        if (templateContent == editor->toPlainText())
        {
            language = lang;
            loadFile(filePath);
        }
    }
    language = lang;
    if (language != "Python" && language != "Java")
        language = "C++";
    Util::applySettingsToEditor(editor, language);
    ui->changeLanguageButton->setText(language);
    performCompileAndRunDiagonistics();
    isLanguageSet = true;
    emit editorLanguageChanged(this);
}

QString MainWindow::getLanguage()
{
    return language;
}

MessageLogger *MainWindow::getLogger()
{
    return log;
}

void MainWindow::insertText(const QString &text)
{
    editor->insertPlainText(text);
}

void MainWindow::setViewMode(const QString &mode)
{
    if (mode == "code")
    {
        ui->left_widget->show();
        ui->right_widget->hide();
    }
    else if (mode == "io")
    {
        ui->left_widget->hide();
        ui->right_widget->show();
    }
    else
    {
        ui->left_widget->show();
        ui->right_widget->show();
        ui->splitter->restoreState(SettingsHelper::getSplitterSize());
    }
}

void MainWindow::detachedExecution()
{
    LOG_INFO("Executing in detached mode");
    afterCompile = RunDetached;
    log->clear();
    compile();
}

void MainWindow::killProcesses()
{
    LOG_INFO("Killing all processes");

    if (compiler != nullptr)
    {
        delete compiler;
        compiler = nullptr;
    }

    for (auto &t : runner)
    {
        if (t != nullptr)
        {
            delete t;
        }
    }
    runner.clear();

    if (detachedRunner != nullptr)
    {
        delete detachedRunner;
        detachedRunner = nullptr;
    }
}

//***************** HELPER FUNCTIONS *****************

void MainWindow::setText(const QString &text, bool keep)
{
    if (keep)
    {
        auto cursor = editor->textCursor();
        int old_pos = cursor.position();
        cursor.select(QTextCursor::Document);
        cursor.insertText(text);
        cursor.setPosition(old_pos);
        editor->setTextCursor(cursor);
    }
    else
        editor->setPlainText(text);
}

void MainWindow::updateWatcher()
{
    if (fileWatcher->files() == QStringList(filePath))
        return;
    if (!fileWatcher->files().isEmpty())
        fileWatcher->removePaths(fileWatcher->files());
    if (!isUntitled())
        fileWatcher->addPath(filePath);
}

void MainWindow::loadFile(const QString &loadPath)
{
    LOG_INFO(INFO_OF(loadPath));

    auto path = loadPath;

    bool samePath = !isUntitled() && filePath == path;
    setFilePath(path);

    bool isTemplate = false;

    if (!QFile::exists(path))
    {
        QString templatePath = SettingsManager::get(QString("%1/Template Path").arg(language)).toString();

        QFile f(templatePath);

        if (!templatePath.isEmpty() && f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            isTemplate = true;
            path = templatePath;
        }
        else
        {
            setText("");
            return;
        }
    }

    auto content = Util::readFile(path, tr("Open File"), log);

    if (content.isNull())
        return;

    savedText = content;
    if (content.length() > SettingsHelper::getOpenFileLengthLimit())
    {
        log->error(tr("Open File"),
                   tr("The file [%1] contains more than %2 characters, so it's not opened. You can change the "
                      "open file length limit in Preferences->Advanced->Limits->Open File Length Limit")
                       .arg(path)
                       .arg(SettingsHelper::getOpenFileLengthLimit()));
        setText("");
        setFilePath("");
        return;
    }

    if (!isUntitled() && SettingsHelper::isRestoreOldProblemUrl() && FileProblemBinder::containsFile(filePath))
        setProblemURL(FileProblemBinder::getProblemForFile(filePath));

    setText(content, samePath);

    if (isTemplate)
    {
        auto match = QRegularExpression(SettingsManager::get(language + "/Template Cursor Position Regex").toString())
                         .match(content);
        if (match.hasMatch())
        {
            int pos = SettingsManager::get(language + "/Template Cursor Position Offset Type").toString() == "start"
                          ? match.capturedStart()
                          : match.capturedEnd();
            pos += SettingsManager::get(language + "/Template Cursor Position Offset Characters").toInt();
            pos = qMax(pos, 0);
            pos = qMin(pos, content.length());
            auto cursor = editor->textCursor();
            cursor.setPosition(pos);
            editor->setTextCursor(cursor);
        }
        else
        {
            editor->moveCursor(QTextCursor::End);
        }
    }

    loadTests();
}

bool MainWindow::saveFile(SaveMode mode, const QString &head, bool safe)
{
    LOG_INFO(INFO_OF(mode) << INFO_OF(head) << BOOL_INFO_OF(safe));
    if (SettingsHelper::isAutoFormat())
        formatter->format(editor, filePath, language, false);

    if (mode == SaveAs || (isUntitled() && mode == AlwaysSave))
    {
        QString defaultPath;

        if (!isUntitled())
        {
            defaultPath = filePath;
        }
        else
        {
            if (!problemURL.isEmpty())
            {
                auto rules = SettingsHelper::getDefaultFilePathsForProblemURLs();
                for (auto rule : rules)
                {
                    if (rule.toStringList().front().isEmpty())
                        continue;
                    auto url = QRegularExpression(rule.toStringList().front());
                    auto match = url.match(problemURL);
                    if (match.hasMatch())
                    {
                        defaultPath = match.captured().replace(url, rule.toStringList().back());
                        break;
                    }
                }
            }
            if (defaultPath.isEmpty())
                defaultPath = QDir(SettingsHelper::getSavePath()).filePath(getTabTitle(false, false));
            if (language == "C++")
                defaultPath += ".cpp";
            else if (language == "Java")
                defaultPath += ".java";
            else if (language == "Python")
                defaultPath += ".py";
        }

        // Create the parent directories of the default path if they don't exist,
        // and remove them if they are empty after saving.

        QStringList madePaths;
        if (!QDir(QFileInfo(defaultPath).absolutePath()).exists())
        {
            QDir dir(QFileInfo(defaultPath).absolutePath());
            while (!dir.exists() && !dir.isRoot())
            {
                madePaths.push_back(dir.path());
                dir.setPath(QDir::cleanPath(dir.filePath("..")));
            }
            QDir().mkpath(QFileInfo(defaultPath).absolutePath());
        }
        auto beforeReturn = [&](bool ret) {
            for (auto path : madePaths)
                if (!QDir().rmdir(path))
                    break;
            return ret;
        };

        emit confirmTriggered(this);
        auto newFilePath =
            QFileDialog::getSaveFileName(this, tr("Save File"), defaultPath, Util::fileNameFilter(true, true, true));
        if (newFilePath.isEmpty())
            return beforeReturn(false);

        if (!Util::saveFile(newFilePath, editor->toPlainText(), head, safe, log, true))
            return beforeReturn(false);

        savedText = editor->toPlainText();

        setFilePath(newFilePath);

        SettingsHelper::setSavePath(QFileInfo(filePath).canonicalPath());

        auto suffix = QFileInfo(filePath).suffix();
        if (Util::cppSuffix.contains(suffix))
            setLanguage("C++");
        else if (Util::javaSuffix.contains(suffix))
            setLanguage("Java");
        else if (Util::pythonSuffix.contains(suffix))
            setLanguage("Python");

        beforeReturn(true);
    }
    else if (!isUntitled())
    {
        if (!Util::saveFile(filePath, editor->toPlainText(), head, safe, log, true))
            return false;

        savedText = editor->toPlainText();
    }
    else
    {
        return false;
    }

    setFilePath(filePath); // make sure that the file path is the canonical file path and the file watcher is working
    emit editorTextChanged(this); // make sure that the tab title is updated

    saveTests(safe);

    return true;
}

QString MainWindow::tmpPath()
{
    bool created = false;
    if (tmpDir == nullptr || !tmpDir->isValid() || !QDir(tmpDir->path()).exists())
    {
        if (tmpDir)
            delete tmpDir;
        tmpDir = new QTemporaryDir();
        if (!tmpDir->isValid())
        {
            LOG_ERR("Failed to create the temporary directory");
            log->error(tr("Temp File"), tr("Failed to create the temporary directory"));
            return QString();
        }
        created = true;
    }
    QString name = "sol.";
    if (language == "C++")
        name += Util::cppSuffix.first();
    else if (language == "Java")
        name += Util::javaSuffix.first();
    else if (language == "Python")
        name += Util::pythonSuffix.first();
    else
    {
        log->error(tr("Temp File"), tr("Please set the language"));
        return "";
    }
    QString path = tmpDir->filePath(name);
    if (!Util::saveFile(path, editor->toPlainText(), tr("Temp File"), false, log))
        return QString();
    if (created)
        emit editorTmpPathChanged(this, path);
    return path;
}

bool MainWindow::isTextChanged() const
{
    if (isUntitled())
    {
        auto content = Util::readFile(SettingsManager::get(QString("%1/Template Path").arg(language)).toString(),
                                      tr("Read %1 Template").arg(language), log);
        if (content.isNull())
            return !editor->toPlainText().isEmpty();
        else
            return editor->toPlainText() != content;
    }
    else
    {
        auto content = Util::readFile(filePath);

        if (content.isNull())
            return true;

        return editor->toPlainText() != content;
    }
}

bool MainWindow::closeConfirm()
{
    LOG_INFO("Confirming to close this window");
    bool confirmed = !isTextChanged();
    if (!confirmed)
    {
        emit confirmTriggered(this);
        auto res = QMessageBox::warning(
            this, tr("Save changes"),
            tr("Save changes to [%1] before closing?").arg(isUntitled() ? tr("New File") : getFileName()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
        if (res == QMessageBox::Save)
            confirmed = saveFile(AlwaysSave, tr("Save"), true);
        else if (res == QMessageBox::Discard)
            confirmed = true;
    }
    LOG_INFO(BOOL_INFO_OF(confirmed));
    return confirmed;
}

void MainWindow::on_clear_messages_button_clicked()
{
    log->clear();
}

void MainWindow::on_changeLanguageButton_clicked()
{
    LOG_INFO("Window language change button requested");
    bool ok = false;
    int curr = 0;

    if (language == "C++")
        curr = 0;
    else if (language == "Java")
        curr = 1;
    else if (language == "Python")
        curr = 2;

    auto response = QInputDialog::getItem(this, tr("Set Tab language"), tr("Set the language to use in this Tab"),
                                          {"C++", "Java", "Python"}, curr, false, &ok);

    if (ok)
        setLanguage(response);
}

void MainWindow::onFileWatcherChanged(const QString &path)
{
    LOG_INFO(INFO_OF(path));

    emit editorTextChanged(this);

    auto currentText = editor->toPlainText();

    auto fileText = Util::readFile(path);

    if (!fileText.isNull())
    {
        if (fileText == savedText)
            return;

        if (fileText == currentText)
        {
            savedText = fileText;
            return;
        }

        if (savedText == currentText)
        {
            loadFile(path);
            return;
        }

        if (!reloading)
        {
            reloading = true;

            emit confirmTriggered(this);
            auto reload = QMessageBox::question(
                this, tr("Reload"), tr("[%1]\n\nhas been changed on disk.\nDo you want to reload it?").arg(filePath));

            reloading = false;

            if (reload == QMessageBox::StandardButton::Yes)
            {
                loadFile(path);
                return;
            }
        }
    }
}

void MainWindow::onTextChanged()
{
    if (SettingsHelper::isAutoSave() && SettingsHelper::getAutoSaveIntervalType() != "Without modification" &&
        (!autoSaveTimer->isActive() || SettingsHelper::getAutoSaveIntervalType() == "After the last modification"))
    {
        autoSaveTimer->start();
    }
    emit editorTextChanged(this);
}

void MainWindow::updateCursorInfo()
{
    auto cursor = editor->textCursor();
    auto selection = cursor.selectedText();
    QString info;
    if (selection.isEmpty())
    {
        auto line = editor->document()->findBlockByNumber(cursor.blockNumber()).text();
        int column = cursor.columnNumber();
        int col = 0;
        for (int i = 0; i < column; ++i)
        {
            if (line[i] != '\t')
                ++col;
            else
                col += SettingsHelper::getTabWidth() - col % SettingsHelper::getTabWidth();
        }
        info = tr("Line %1, Column %2").arg(cursor.blockNumber() + 1).arg(col + 1);
    }
    else
    {
        int selectionStart = cursor.selectionStart();
        int selectionEnd = cursor.selectionEnd();
        cursor.setPosition(selectionStart);
        int lineStart = cursor.blockNumber();
        cursor.setPosition(selectionEnd);
        int lineEnd = cursor.blockNumber();
        int selectionLines = lineEnd - lineStart + 1;
        if (selectionLines > 1)
            info = tr("%1 lines, %2 charachters selected").arg(selectionLines).arg(selection.length());
        else
            info = tr("%1 characters selected").arg(selection.length());
    }
    ui->cursor_info->setText(info);
}

void MainWindow::updateChecker()
{
    if (checker)
        delete checker;
    if (testcases->checkerType() == Core::Checker::Custom)
        checker = new Core::Checker(testcases->checkerText(), log, this);
    else
        checker = new Core::Checker(testcases->checkerType(), log, this);
    connect(checker, &Core::Checker::checkFinished, testcases, &Widgets::TestCases::setVerdict);
    checker->prepare(SettingsManager::get(QString("C++/Compile Command")).toString());
}

QSplitter *MainWindow::getSplitter()
{
    return ui->splitter;
}

QSplitter *MainWindow::getRightSplitter()
{
    return ui->right_splitter;
}

void MainWindow::performCompileAndRunDiagonistics()
{
    bool compilerResult = true;
    bool runResult = true;

    if (language == "C++" || language == "Java")
        compilerResult =
            Core::Compiler::check(SettingsManager::get(QString("%1/Compile Command").arg(language)).toString());

    if (language == "Java" || language == "Python")
        runResult = Core::Compiler::check(SettingsManager::get(QString("%1/Run Command").arg(language)).toString());

    if (!compilerResult)
        log->error(tr("Compiler"),
                   tr("The compile command for %1 is invalid. Is the compiler in the system PATH?").arg(language));

    if (!runResult)
        log->error(tr("Runner"),
                   tr("The run command for %1 is invalid. Is the runner in the system Path?").arg(language));
}

// -------------------- COMPILER SLOTS ---------------------------

void MainWindow::onCompilationStarted()
{
    log->info(tr("Compiler"), tr("Compilation has started"));
}

void MainWindow::onCompilationFinished(const QString &warning)
{
    if (language != "Python")
    {
        log->info(tr("Compiler"), tr("Compilation has finished"));
        if (!warning.trimmed().isEmpty())
        {
            log->warn(tr("Compile Warnings"), warning);
        }
    }

    if (afterCompile == Run)
    {
        run();
    }
    else if (afterCompile == RunDetached)
    {
        if (SettingsHelper::isSaveFileOnExecution())
            saveFile(IgnoreUntitled, tr("Runner"), true);

        killProcesses();

        if (!QStringList({"C++", "Java", "Python"}).contains(language))
        {
            log->warn(tr("Runner"), tr("Wrong language, please set the language"));
            return;
        }

        detachedRunner = new Core::Runner(-1);
        connect(detachedRunner, SIGNAL(runStarted(int)), this, SLOT(onRunStarted(int)));
        connect(detachedRunner, SIGNAL(failedToStartRun(int, const QString &)), this,
                SLOT(onFailedToStartRun(int, const QString &)));
        connect(detachedRunner, SIGNAL(runKilled(int)), this, SLOT(onRunKilled(int)));
        detachedRunner->runDetached(tmpPath(), filePath, language,
                                    SettingsManager::get(QString("%1/Run Command").arg(language)).toString(),
                                    SettingsManager::get(QString("%1/Run Arguments").arg(language)).toString());
    }
}

void MainWindow::onCompilationErrorOccurred(const QString &error)
{
    log->error(tr("Compiler"), tr("Error occurred while compiling"));
    if (!error.trimmed().isEmpty())
        log->error(tr("Compile Errors"), error);
}

void MainWindow::onCompilationKilled()
{
    log->error(tr("Compiler"), tr("Compilation is killed"));
}

// --------------------- RUNNER SLOTS ----------------------------

QString MainWindow::getRunnerHead(int index)
{
    if (index == -1)
        return tr("Detached Runner");
    return tr("Runner[%1]").arg(index + 1);
}

void MainWindow::onRunStarted(int index)
{
    log->info(getRunnerHead(index), tr("Execution has started"));
}

void MainWindow::onRunFinished(int index, const QString &out, const QString &err, int exitCode, int timeUsed)
{
    auto head = getRunnerHead(index);

    if (exitCode == 0)
    {
        log->info(head, tr("Execution for test case #%1 has finished in %2ms").arg(index + 1).arg(timeUsed));
    }

    else
    {
        log->error(head, tr("Execution for test case #%1 has finished with non-zero exitcode %2 in %3ms")
                             .arg(index + 1)
                             .arg(exitCode)
                             .arg(timeUsed));
    }

    if (!err.trimmed().isEmpty())
        log->error(head + tr("/stderr"), err);
    testcases->setOutput(index, out);
    if ((!out.isEmpty() && !testcases->expected(index).isEmpty()) ||
        (SettingsHelper::isCheckOnTestcasesWithEmptyOutput() && exitCode == 0))
        checker->reqeustCheck(index, testcases->input(index), out, testcases->expected(index));
}

void MainWindow::onFailedToStartRun(int index, const QString &error)
{
    log->error(getRunnerHead(index), error);
}

void MainWindow::onRunTimeout(int index)
{
    log->warn(getRunnerHead(index), tr("Time Limit Exceeded"));
}

void MainWindow::onRunOutputLimitExceeded(int index, const QString &type)
{
    log->warn(getRunnerHead(index),
              tr("The %1 of the process running on the testcase #%2 contains more than %3 characters, which is longer "
                 "than the output length limit, so the process is killed. You can change the output length limit "
                 "in Preferences->Advanced->Limits->Output Length Limit")
                  .arg(type)
                  .arg(index + 1)
                  .arg(SettingsHelper::getOutputLengthLimit()));
}

void MainWindow::onRunKilled(int index)
{
    log->error(getRunnerHead(index),
               tr("%1 has been killed")
                   .arg(index == -1 ? tr("Detached runner") : tr("Runner for testcase #%1").arg(index + 1)));
}
