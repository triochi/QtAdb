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


#ifndef LOGCATDIALOG_H
#define LOGCATDIALOG_H

#include <QDialog>
#include <QtGui>
#include "../classes/models/logcatmodel.h"
#include "ui_logcatdialog.h"

class LogcatDialog : public QDialog, public Ui::LogcatDialog
{
    Q_OBJECT

public:
    LogcatDialog(QWidget *parent = 0);
    ~LogcatDialog();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QProcess *proces;
    QString sdk;
    LogcatModel *logcatModel;
    SortFilterProxyModel *filterModel;
    void executeBufferLimitation();
    int bufferLimit;
    QMenu *contextMenu;
public slots:
    void read();
    void filter();
    void startLogcat();
private slots:
    void on_pushButtonClearLogcat_pressed();
    void on_spinBoxBufferLimit_editingFinished();
    void on_checkBoxAutoScroll_toggled(bool checked);
    void showContextMenu(QPoint point);
    void copySelectedToClipboard();
    void exportSelectedToFile();
};

#endif // LOGCATDIALOG_H
