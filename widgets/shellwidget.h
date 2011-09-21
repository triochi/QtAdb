/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


#ifndef SHELLWIDGET_H
#define SHELLWIDGET_H

#include <QtGui>

class ShellWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit ShellWidget(QWidget *parent = 0);
    ~ShellWidget();
    void setProcessPath(QString processPath);
    QString getProcessPath();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    int insertedChars;
    int cursorPosition;
    QString command;
    QProcess process;
    QString processPath;
    QTextCursor cursor;

    QColor fontColor;
    QString sdk;

    QStringList commandHistory;
    QStringList commands;
    int commandHistoryPosition;

signals:
    void returnPressed(QString command);

private slots:
    void readFromProcess();
    void executeCommand(QString command);
};

/*

namespace Ui {
    class ShellWidget;
}

class ShellWidget : public QWidget
{
    Q_OBJECT

public:
    ShellWidget(QWidget *parent = 0);
    ~ShellWidget();

protected:
    void changeEvent(QEvent *e);
    virtual void keyPressEvent( QKeyEvent *e );

private:
    Ui::ShellWidget *ui;
    QProcess *procesShell;
    bool processShellIsRunning;
    QString sdk;
    QCompleter *completer;
    QStringList commandList;
    QStringListModel *commandModel;


private slots:
    void readShell();
    void writeShell();
    void shellFinished();
};
*/
#endif // SHELLWIDGET_H
