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


#include "logcatdialog.h"
#include "ui_logcatdialog.h"

LogcatDialog::LogcatDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QSettings settings;
    this->bufferLimit = settings.value("logcatBufferLimit",0).toInt();
    this->spinBoxBufferLimit->setValue(this->bufferLimit);
    this->sdk = settings.value("sdkPath").toString();

    checkBoxAutoScroll->setChecked(settings.value("logcatAutoScroll",true).toBool());
    this->tableView->setContextMenuPolicy(Qt::CustomContextMenu);


    this->contextMenu = new QMenu();

    this->contextMenu->addAction(tr("clear logcat"),this,SLOT(on_pushButtonClearLogcat_pressed()));
    this->contextMenu->addAction(tr("copy selected to clipboard"),this,SLOT(copySelectedToClipboard()));
    this->contextMenu->addAction(tr("export selected to file"),this,SLOT(exportSelectedToFile()));


    this->restoreGeometry(settings.value("logcatDialogGeometry").toByteArray());

    setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    this->logcatModel = new LogcatModel;
    this->filterModel = new SortFilterProxyModel;
    this->filterModel->setSourceModel(this->logcatModel);
    this->filterModel->setFilterKeyColumn(0);
    this->filterModel->setDynamicSortFilter(true);
    this->tableView->setSelectionMode(QTableView::ExtendedSelection);
    this->tableView->setSelectionBehavior(QTableView::SelectRows);

    this->setLayout(layoutLogcat);
    this->proces=new QProcess(this);
    proces->setProcessChannelMode(QProcess::MergedChannels);
    this->setWindowTitle("Logcat");
    this->proces->start("\""+sdk+"\""+"adb logcat");

    this->tableView->setModel(this->filterModel);
    this->textBrowser->hide();
    connect(this->proces, SIGNAL(readyRead()), this, SLOT(read()));
    connect(this->checkBoxDebug, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxErrors, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxVerbose, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxInformations, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxWarnings, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(filter()));
    connect(this->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filter()));
    connect(this->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    //setAttribute(Qt::WA_DeleteOnClose);
}

LogcatDialog::~LogcatDialog()
{
    QSettings settings;
    settings.setValue("logcatDialogGeometry",this->saveGeometry());
    this->proces->close();
}

void LogcatDialog::showContextMenu(QPoint point)
{
    QPoint pos2;
    pos2.setX(point.x());
    pos2.setY(point.y()+20);

    this->contextMenu->exec(this->tableView->mapToGlobal(pos2));
}

void LogcatDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("logcatDialogGeometry",this->saveGeometry());
    QWidget::closeEvent(event);
    this->proces->close();
}

void LogcatDialog::read()
{
    QString tmp = QString::fromUtf8(this->proces->readAll());
    QStringList list=tmp.split("\n");

    qApp->processEvents();
    while (list.length() > 0)
    {
        tmp=list.takeFirst();
        tmp.remove(QRegExp("\\s+$"));
        if (tmp.isEmpty())
            continue;
        LogcatMessage logcatMessage;
        if (tmp.startsWith("D"))
            logcatMessage.type = "Debug";
        else if (tmp.startsWith("W"))
            logcatMessage.type = "Warning";
        else if (tmp.startsWith("E"))
            logcatMessage.type = "Error";
        else if (tmp.startsWith("I"))
            logcatMessage.type = "Info";
        else if (tmp.startsWith("V"))
            logcatMessage.type = "Verbose";

        tmp.remove(0, 2);
        logcatMessage.sender = tmp.left(tmp.indexOf("("));
        tmp.remove(0, tmp.indexOf("(") + 1);
        logcatMessage.sender.remove(QRegExp("\\s+$"));
        logcatMessage.pid = tmp.left(tmp.indexOf(")"));
        tmp.remove(0, tmp.indexOf(":") + 1);
        logcatMessage.message = tmp;
        logcatMessage.pid.remove(QRegExp("\\s+$"));
        logcatMessage.pid.remove(QRegExp("^\\s+"));

        QDateTime time;
        logcatMessage.timestamp = time.currentDateTime().toString("hh:mm:ss.zzz");

//        logcatMessage.number=QString::number(logcatModel->rowCount());

        this->logcatModel->insertMessage(logcatMessage);
    }

    this->tableView->resizeColumnToContents(0);
    this->tableView->resizeColumnToContents(1);
    this->tableView->resizeColumnToContents(2);
    this->tableView->resizeColumnToContents(3);
    if (!this->tableView->horizontalScrollBar()->isVisible())
            this->tableView->setColumnWidth(4,tableView->width() - tableView->columnWidth(0) - tableView->columnWidth(1) - tableView->columnWidth(2) - tableView->columnWidth(3) - 20);

    executeBufferLimitation();

    if (checkBoxAutoScroll->isChecked())
        this->tableView->scrollToBottom();

}

void LogcatDialog::filter()
{

    this->filterModel->clearFilters();
    QString pattern;
    if (this->checkBoxDebug->isChecked())
    {
        pattern.append("^Debug$");
    }
    if (this->checkBoxErrors->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Error$");
    }
    if (this->checkBoxInformations->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Info$");
    }
    if (this->checkBoxVerbose->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Verbose$");
    }
    if (this->checkBoxWarnings->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Warning$");
    }

    QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp);

    this->filterModel->addFilterRegExp(1,regExp);

    pattern = this->lineEdit->text();
    QRegExp regExp2(pattern, Qt::CaseInsensitive, QRegExp::RegExp);
    if (this->comboBox->currentIndex() == 0)
        this->filterModel->addFilterRegExp(2,regExp2);
    else
        this->filterModel->addFilterRegExp(4,regExp2);

    if (checkBoxAutoScroll->isChecked())
        this->tableView->scrollToBottom();
}

void LogcatDialog::startLogcat()
{
    if (this->proces->isOpen())
        this->proces->close();
    this->proces->start("\""+sdk+"\""+"adb logcat");
}

void LogcatDialog::on_pushButtonClearLogcat_pressed()
{
    this->logcatModel->clear();
}

void LogcatDialog::executeBufferLimitation()
{
    if (this->bufferLimit == 0)
        return;
    int rowCount = this->logcatModel->rowCount();
    if (rowCount <= this->bufferLimit)
        return;
    this->logcatModel->removeRows(0,rowCount-this->bufferLimit,QModelIndex());
}

void LogcatDialog::on_spinBoxBufferLimit_editingFinished()
{
    this->bufferLimit = this->spinBoxBufferLimit->value();
    QSettings settings;
    settings.setValue("logcatBufferLimit",this->bufferLimit);
    this->executeBufferLimitation();
}

void LogcatDialog::on_checkBoxAutoScroll_toggled(bool checked)
{
    QSettings settings;
    settings.setValue("logcatAutoScroll", checkBoxAutoScroll->isChecked());
}

void LogcatDialog::copySelectedToClipboard()
{
    QModelIndexList indexList = this->tableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<LogcatMessage> list;
    while (!indexList.isEmpty())
    {
        index = this->filterModel->mapToSource(indexList.takeFirst());
        list.append(this->logcatModel->getRow(index.row()));
    }
    QString output;
    foreach (LogcatMessage item, list)
    {
        output.append(item.timestamp+" "+item.type+" "+item.sender+" "+item.pid+" "+item.message+"\n");
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(output);
}

void LogcatDialog::exportSelectedToFile()
{
    QModelIndexList indexList = this->tableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<LogcatMessage> list;
    while (!indexList.isEmpty())
    {
        index = this->filterModel->mapToSource(indexList.takeFirst());
        list.append(this->logcatModel->getRow(index.row()));
    }
    QString output;
    foreach (LogcatMessage item, list)
    {
        output.append(item.timestamp+" "+item.type+" "+item.sender+" "+item.pid+" "+item.message+"\n");
    }

    QFile file;
    file.setFileName(QFileDialog::getSaveFileName(this, tr("Save File..."), "./logcat.txt", tr("txt file")+" (*.txt)"));
    if (file.fileName().isEmpty())
        return;
    if (file.open(QFile::WriteOnly))
    {
        file.write(output.toLatin1());
        file.close();
    }
}
