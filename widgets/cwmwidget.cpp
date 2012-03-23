/***********************************************************************
*Copyright 2010-20XX by nijel
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
*   @author nijel (nijel8@gmail.com)
*
************************************************************************/


#include "cwmwidget.h"
#include "ui_cwmwidget.h"

CwmWidget::CwmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CwmWidget)
{
    this->settings = new SettingsWidget;
    ui->setupUi(this);

    this->sdcardTableView = new MyTableView;
    this->ui->sdphoneLayout->addWidget(this->sdcardTableView);

    this->phoneModel = new FileTableModel;

    this->phoneSortModel = new FileSortModel;

    this->phoneSortModel->setSourceModel(this->phoneModel);

    this->phoneSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    this->phoneSortModel->setDynamicSortFilter(true);

    this->phoneSortModel->setFilterKeyColumn(1);

    this->sdcardTableView->setModel(this->phoneSortModel);

    this->phone=new Phone(true);
    this->phone->setPath("/sdcard");
    this->phone->setHiddenFiles(this->settings->phoneHiddenFiles);
    this->sdcardChangeName=false;

    sdcardTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->pushButtonDirectoryUp->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogToParent));

    this->setLayout(ui->layoutCwm);

    this->sdcardTableView->horizontalHeader()->setVisible(this->settings->showPhoneColumnsHeaders);

    this->phoneMenu = NULL;

    if (phone->getPath().isEmpty())
        phone->setPath("/sdcard");


    processFind = new QProcess(this);
    process = new QProcess(this);
    processWhich = new QProcess(this);

    process->setProcessChannelMode(QProcess::MergedChannels);
    processWhich->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->process, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromProcess()));
    connect(this->process, SIGNAL(finished(int)), this, SLOT(processFinished()));
    connect(this->processWhich, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromProcessWhich()));
    connect(this->processWhich, SIGNAL(finished(int)), this, SLOT(finishedWhich()));
    connect(this->phone,SIGNAL(signalConnectionChanged(int)),this,SLOT(connectionChanged()));
    connect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(mountsUpdate()));
    connect(sdcardTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(phoneContextMenu(const QPoint &)));
    connectSignals();

    this->codec = QTextCodec::codecForLocale();

    this->buttonsEnabled();
    currentDir = QDir::currentPath();
    QFont font, fontStatus;
    font.setPointSize(14);
    fontStatus.setPointSize(10);
    font.setBold(true);
    fontStatus.setBold(true);
    this->ui->label->setFont(font);
    this->ui->plainTextEditStatus->setFont(fontStatus);
    this->ui->label_2->setFont(fontStatus);
}

CwmWidget::~CwmWidget()
{
    delete this->phone;
    if (this->phoneMenu != NULL)
        delete this->phoneMenu;
    delete ui;
}

void CwmWidget::setTabCreate(int i)
{
    if (i == 2)
        if (this->ui->tabWidget->currentIndex() > 1)
            this->ui->tabWidget->setCurrentWidget(this->ui->tabSdcardPart);
}

void CwmWidget::setTabFix(int i)
{
    if (i > 1)
        if (this->ui->tabWidget_3->currentIndex() == 2)
            this->ui->tabWidget_3->setCurrentWidget(this->ui->tabPermissions);
}

void CwmWidget::connectSignals()
{
    connect(ui->lineEditPath,SIGNAL(returnPressed()),this,SLOT(sdcardLineEdit()));
    connect(ui->lineEditPath, SIGNAL(editingFinished ()), this, SLOT(sdcardLineEdit()));
    connect(ui->tabWidget_2,SIGNAL(currentChanged(int)),this,SLOT(sdcardDisplay()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(activateButtonInsert()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(buttonsEnabled()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(setTabFix(int)));
    connect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(buttonsEnabled()));
    connect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(activateButtonInsert()));
    connect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(setTabCreate(int)));
    connect(this->sdcardTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sdcardDoubleClick()));
    connect(this->sdcardTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(activateButtonInsert()));
    connect(this->ui->radio128, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radio256, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radio512, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radio1024, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radio2048, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radio4096, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkData, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkDalvik, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkCache, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBattery, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkDefaultBackupDir, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radioUpdateZip, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->radioInstallPath, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->lineBackup, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    connect(this->ui->lineRestore, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    connect(this->ui->lineUpdate, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    connect(this->ui->lineFlash, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    connect(this->ui->lineRecovery, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBacBoot, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBacCache, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBacData, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBacSdext, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkBacSystem, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    connect(this->ui->checkAfter, SIGNAL(toggled(bool)),this,SLOT(checkFlash()));
    connect(this->ui->checkBefore, SIGNAL(toggled(bool)),this,SLOT(checkFlash()));
}

void CwmWidget::disconnectSignals()
{
    disconnect(ui->lineEditPath,SIGNAL(returnPressed()),this,SLOT(sdcardLineEdit()));
    disconnect(ui->lineEditPath, SIGNAL(editingFinished ()), this, SLOT(sdcardLineEdit()));
    disconnect(ui->tabWidget_2,SIGNAL(currentChanged(int)),this,SLOT(sdcardDisplay()));
    disconnect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(activateButtonInsert()));
    disconnect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(buttonsEnabled()));
    disconnect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(setTabFix(int)));
    disconnect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(buttonsEnabled()));
    disconnect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(activateButtonInsert()));
    disconnect(ui->tabWidget_3,SIGNAL(currentChanged(int)),this,SLOT(setTabCreate(int)));
    disconnect(this->sdcardTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sdcardDoubleClick()));
    disconnect(this->sdcardTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(activateButtonInsert()));
    disconnect(this->ui->radio128, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radio256, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radio512, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radio1024, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radio2048, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radio4096, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkData, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkDalvik, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkCache, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBattery, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkDefaultBackupDir, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radioUpdateZip, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->radioInstallPath, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->lineBackup, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->lineRestore, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->lineUpdate, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->lineFlash, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->lineRecovery, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBacBoot, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBacCache, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBacData, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBacSdext, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkBacSystem, SIGNAL(toggled(bool)),this,SLOT(buttonsEnabled()));
    disconnect(this->ui->checkAfter, SIGNAL(toggled(bool)),this,SLOT(checkFlash()));
    disconnect(this->ui->checkBefore, SIGNAL(toggled(bool)),this,SLOT(checkFlash()));
}

//void CwmWidget::mousePressEvent(QMouseEvent *event)
//{
//   if (event->button() == Qt::RightButton)
//      emit customContextMenuRequested(event->pos());
//}

void CwmWidget::connectionChanged()
{
    if (this->phone->getConnectionState() == DISCONNECTED)
    {
        emit this->phoneConnectionChanged(DISCONNECTED);
    }
    else if (this->phone->getConnectionState() == DEVICE)
    {
        emit this->phoneConnectionChanged(DEVICE);
        connectSignals();
        mountsDisable();
        mountsUpdate();
        this->sdcardDisplay();
        this->buttonsEnabled();
        QFont font, fontStatus;
        font.setPointSize(14);
        fontStatus.setPointSize(10);
        font.setBold(true);
        fontStatus.setBold(true);
        this->ui->label->setFont(font);
        this->ui->plainTextEditStatus->setFont(fontStatus);
        if (readLog == "read")
        {
            readLog = "Not Read";
            while (this == NULL && !this->isVisible())
            {
            }
            readFromLog();
        }
    }
    else if (this->phone->getConnectionState() == RECOVERY)
    {
        emit this->phoneConnectionChanged(RECOVERY);
        disconnectSignals();
        mountsEnable();
        mountsUpdate();
        this->ui->tabWidget_2->setCurrentIndex(1);
        this->buttonsDisabled();
        QFont font, fontStatus;
        font.setPointSize(14);
        fontStatus.setPointSize(10);
        font.setBold(true);
        fontStatus.setBold(true);
        this->ui->label->setFont(font);
        this->ui->plainTextEditStatus->setFont(fontStatus);
        if (commandRunning == "running")
        {
            while (this == NULL && !this->isVisible())
            {
            }
            tailLog();
        }



           // QTimer::singleShot(2000, this, SLOT(tailLog()));
    }
    else if (this->phone->getConnectionState() == FASTBOOT)
    {
        emit this->phoneConnectionChanged(FASTBOOT);
        disconnectSignals();
    }
    return;
}

void CwmWidget::on_pushButtonDirectoryUp_pressed()
{
    if (this->phone->cdUp())
    {
        this ->ui->buttonInsert->setEnabled(false);
        this->sdcardDisplay();
    }
}

void CwmWidget::phoneContextMenu(const QPoint &pos)
{
        this->phoneMenu = new QMenu;
        QSettings settings;
        int layoutDirection = settings.value("layoutDirection",0).toInt();

        if (layoutDirection == 0)
            this->phoneMenu->setLayoutDirection(Qt::LeftToRight);
        if (layoutDirection == 1)
            this->phoneMenu->setLayoutDirection(Qt::RightToLeft);
        QAction *usun,*odswiez,*nowyFolder,*zmienNazwe,*ukryte,*props, *sep;

        zmienNazwe = this->phoneMenu->addAction(QIcon(":icons/rename.png"),tr("rename", "phone right click menu"),this,SLOT(phoneRename()));
        zmienNazwe->setData(QString("rename"));
        nowyFolder = this->phoneMenu->addAction(QIcon(":icons/newDir.png"),tr("new dir", "phone right click menu"),this,SLOT(phoneNewDir()));
        nowyFolder->setData(QString("new dir"));
        odswiez = this->phoneMenu->addAction(QIcon(":icons/refresh.png"),tr("refresh", "phone right click menu"),this,SLOT(sdcardDisplay()));
        odswiez->setData(QString("refresh"));
        usun = this->phoneMenu->addAction(QIcon(":icons/remove.png"),tr("delete", "phone right click menu"),this,SLOT(phoneDelete()));
        usun->setData(QString("delete"));
        ukryte = this->phoneMenu->addAction(QIcon(":icons/hidden.png"),tr("hidden files", "phone right click menu"),this,SLOT(phoneHiddenFiles()));
        ukryte->setData(QString("hidden files"));
        sep = this->phoneMenu->addSeparator();
        props = this->phoneMenu->addAction(QIcon(":icons/info.png"),tr("Properties", "phone right click menu"),this,SLOT(propsDialog()));
        props->setData(QString("properties"));

    QPoint pos2;
    pos2.setX(pos.x());
    pos2.setY(pos.y()+20);
    this->phoneMenu->exec(sdcardTableView->mapToGlobal(pos2));
}

void CwmWidget::phoneDelete()
{
    if (this->sdcardTableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this,"",tr("Select some items first!"),QMessageBox::Ok);
        return;
    }

    if (QMessageBox::question(this, tr("Deleting:"), tr("Are you sure???"),
                              QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    QModelIndexList indexList = this->sdcardTableView->selectionModel()->selectedRows();
    QModelIndex index;
    while (!indexList.isEmpty())
    {
        index = this->phoneSortModel->mapToSource(indexList.takeFirst());
        this->phone->remove(this->phoneModel->getFile(index.row()).fileName);
    }
    this->sdcardDisplay();
}

void CwmWidget::phoneDisplay(QTableView *tableView)
{
    QSettings settings;
    tableView->setModel(this->phoneSortModel);
    tableView->setDisabled(true);
    this->phoneModel->clear();
    this->ui->lineEditPath->setText(this->phone->getPath());
    QString sdpath = this->ui->lineEditPath->text();
    if (sdpath.endsWith("/"))
        this->ui->lineEditPath->setText(sdpath.remove(sdpath.length()-1,1));
    QList<File> *fileList = NULL;
    fileList = this->phone->getFileList();
    if (fileList == NULL)
        return;
    while (!fileList->isEmpty())
    {
        this->phoneModel->insertFile(0, fileList->takeFirst());
    }
    delete fileList;
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
    int i=0;
    int rows = this->phoneModel->rowCount();
    for (i=0; i<rows; i++)
        tableView->setRowHeight(i, 20);
    tableView->sortByColumn(1, Qt::AscendingOrder);
    tableView->setDisabled(false);
    QStringList phoneColumnList = settings.value("phoneColumnList").toStringList();
    int col = this->phoneModel->columnCount(QModelIndex());
    for (i = 0; i < col; i++)
    {
         if (!phoneColumnList.contains(QString::number(i)) && !phoneColumnList.isEmpty())
             this->sdcardTableView->hideColumn(i);
         else
             this->sdcardTableView->showColumn(i);
    }
}

void CwmWidget::phoneHiddenFiles()
{
    this->phone->setHiddenFiles(!this->phone->getHiddenFilesState());
    this->sdcardDisplay();
}

void CwmWidget::phoneNewDir()
{
    if (!this->phone->makeDir("new dir"))
    {
        QMessageBox::information(this,tr("Error:"),tr("Directory was not created.\nMake sure directory doesn't exist and that you are allowed to do this!"),QMessageBox::Ok);
        return;
    }
    this->sdcardDisplay();
    int row = this->phoneModel->getRow("new dir");
    QModelIndex index = this->phoneModel->index(row, 1, QModelIndex());
    this->sdcardTableView->selectRow(this->phoneSortModel->mapFromSource(index).row());
    QModelIndexList indexList = this->sdcardTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = indexList.first();
        this->sdcardTableView->edit(index);
        connect (this->phoneModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));
    }
}

void CwmWidget::phoneRename()
{
    QModelIndexList indexList = this->sdcardTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = indexList.first();
        this->sdcardTableView->edit(index);
        connect (this->phoneModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));
    }
}

void CwmWidget::phoneRenameSlot(QModelIndex indexS, QModelIndex indexE)
{
    disconnect (this->phoneModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));

    if (indexS == indexE)
    {
        File tmpFile = this->phoneModel->getFile(indexS.row());
        QString oldFile = tmpFile.filePath;
        oldFile.remove(this->phone->getPath());
        this->phone->rename(oldFile, indexS.data(Qt::DisplayRole).toString());
        this->sdcardDisplay();
    }
}

void CwmWidget::sdcardLineEdit()
{
    if (phone->cd(ui->lineEditPath->text()))
    {
        this->phone->setPath(ui->lineEditPath->text());
        this->sdcardDisplay();
    }
}

void CwmWidget::sdcardDisplay()
{
    this->phoneModel->setColoring(this->settings->color);
    this->phone->setHiddenFiles(this->settings->phoneHiddenFiles);
    this->sdcardTableView->horizontalHeader()->setVisible(this->settings->showPhoneColumnsHeaders);
    this->phoneDisplay(this->sdcardTableView);
    this->sdcardTableView->setFocus();
    int widthTab = this->sdcardTableView->width();
    int colIcon = 25;
    int colName, colDate, colSize;

    this->sdcardTableView->resizeColumnsToContents();
    colSize = this->sdcardTableView->columnWidth(2);
    colDate = this->sdcardTableView->columnWidth(3);

    this->sdcardTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   // this->sdcardTableView->verticalScrollBar()->adjustSize();
  //  colName = widthTab - colIcon - colSize - colDate - this->sdcardTableView->verticalScrollBar()->width();

    if (this->sdcardTableView->verticalScrollBar()->width() > 17)
        this->sdcardTableView->verticalScrollBar()->adjustSize();

    if (this->sdcardTableView->verticalScrollBar() == NULL)
        colName = widthTab - colIcon - colSize - colDate;
    else
        colName = widthTab - colIcon - colSize - colDate - this->sdcardTableView->verticalScrollBar()->width();

    this->sdcardTableView->setColumnWidth(0, colIcon);
    this->sdcardTableView->setColumnWidth(1, colName -2);
    this->sdcardTableView->setColumnWidth(2, colSize);
    this->sdcardTableView->setColumnWidth(3, colDate + this->sdcardTableView->verticalScrollBar()->width());
    if (this->ui->lineEditPath->text() == "/sdcard" || this->ui->lineEditPath->text() == "/sdcard/")
        this->ui->pushButtonDirectoryUp->setDisabled(true);
    else if (this->ui->lineEditPath->text().startsWith("/sdcard"))
        this->ui->pushButtonDirectoryUp->setEnabled(true);
}

void CwmWidget::sdcardDoubleClick()
{
    if (!this->sdcardTableView->selectionModel()->selection().isEmpty())
    {
        QModelIndex index;
        QString fileName, fileType;
        index = this->sdcardTableView->selectionModel()->selection().indexes().takeFirst();
        index = this->phoneSortModel->mapToSource(index);
        fileName = this->phoneModel->getFile(index.row()).fileName;
        fileType = this->phoneModel->getFile(index.row()).fileType;
        if (fileName.contains(" -> "))
        {
            fileName = fileName.split(" -> ").at(1);
        }
        if (fileType == "dir")
            if (this->phone->cd(fileName))
            {
                this->sdcardDisplay();
                this->ui->buttonInsert->setDisabled(true);
            }

        if (fileType == "file")
            this->on_buttonInsert_pressed();
    }
}

void CwmWidget::propsDialog()
{
    QModelIndexList indexList = this->sdcardTableView->selectionModel()->selectedRows(1);
    if (indexList.isEmpty())
    {
        QMessageBox::information(this,"",tr("Select some item first!"),QMessageBox::Ok);
        return;
    }
    if (indexList.size() == 1)
    {
        fipDialog *d = new fipDialog(this);
        QModelIndex index = this->phoneSortModel->mapToSource(indexList.takeFirst());
        File tmpFile = this->phoneModel->getFile(index.row());
        QString file = tmpFile.filePath;
        d->setData(file);
        d->show();
    }
    if (indexList.size() > 1)
    {
        QMessageBox::information(this,"",tr("Single item selection only!"),QMessageBox::Ok);
        return;
    }
}

void CwmWidget::processFinished()
{
    connect(ui->tabWidget_2,SIGNAL(currentChanged(int)),this,SLOT(sdcardDisplay()));
    processFind->start("\""+this->sdk+"\""+"adb shell rm -r /cache/qtadb/");
    processFind->waitForFinished(-1);
    ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->insertPlainText("Process Finished!\n");
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    if (which == "flash recovery")
    {
        which = "";
        QString flashout = this->ui->plainTextEditStatus->toPlainText();
        if (flashout.contains("error"))
        {
            ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:red;border: 1px solid #020202;border-radius: 1px;}" );
            this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
            this->ui->plainTextEditStatus->insertPlainText("Process Failed!\n");
            this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
            this->ui->plainTextEditStatus->ensureCursorVisible();
            QMessageBox::information(this,"Flash New Recovery:","Flashing New Recovery FAILED!!!\nTry with moving img file to the root of /sdcard or changing its name to something more simple.\n\nSee Status for Detailes.");
        }
        else
        {
            commandRunning = "Not Running";
            if (QMessageBox::question(this, tr("Flash New Recovery:"),"Flashing Recovery Done!.\n\nDo you want to reboot Phone to test the New Recovey?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                this->phone->adbRebootRecovery();
        }
    }
    else if (commandRunning == "Not Running" && which != "fix permissions")
         QMessageBox::information(0,"Advanced Menu:","      Process Finished!      ");
    else if (which == "fix permissions")
    {
        commandRunning = "Not Running";
        which = "";
        if (QMessageBox::question(this, tr("Fix Permissions:"),"Fix Permissions Done!\n\nDo you want to reboot Phone Now?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            this->phone->adbReboot();
    }
    this->buttonsEnabled();
    this->ui->buttonSystem->setEnabled(true);
    if (this->phone->getConnectionState() == RECOVERY)
        mountsEnable();
}

void CwmWidget::finishedWhich()
{
    if (which == "sdbackup")
    {
        which = "sdrestore";
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("----------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("SD Card Backup Finished!\n");
        this->ui->plainTextEditStatus->insertPlainText("----------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("\nRebooting Phone...\n");
        this->ui->plainTextEditStatus->ensureCursorVisible();
        buttonsDisabled();
        QTimer::singleShot(2000, this, SLOT(createPartition()));
    }
    else if (which == "sdrestore")
    {
        which = "";
        commandRunning = "Not Running";
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("SD Card Restore Finished!\n");
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        this->ui->plainTextEditStatus->ensureCursorVisible();
        buttonsEnabled();
        this->ui->buttonSystem->setEnabled(true);
        if (this->phone->getConnectionState() == RECOVERY)
            mountsEnable();
        if (QMessageBox::question(this,tr("Restore SD Card:"),"SD Card content is now Restored!\n\nDo you want to remove \"" + currentDir.replace("/","\\") + "\\SD_Card\" Backup folder?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
        if (!this->computer->deleteDir(QDir::currentPath()+"/SD_Card/"))
            QMessageBox::information(this,tr("Delete SD Card Backup Folder:"),"Unable to remove \"" + currentDir.replace("/","\\")+"\\SD_Card\" Backup folder!\n\nRemove it manually.");
    }
    else if (which == "md5sum")
    {
        this->md5sumVerify();
        which = "";
        commandRunning = "Not Running";
        this->buttonsEnabled();
        this->ui->buttonSystem->setEnabled(true);
        if (this->phone->getConnectionState() == RECOVERY)
            mountsEnable();
    }
    this->ui->plainTextEditStatus->ensureCursorVisible();
}

void CwmWidget::on_buttonFixPermissions_pressed()
{
    which = "fix permissions";
    QString tool = "/system/bin/fix_permissions";
    processFind->start("\""+sdk+"\"" + "adb shell find " + tool);
    processFind->waitForFinished(-1);
    QString outputFind = processFind->readAll();
    if (outputFind.contains("No such file"))
    {
        tool = "/system/xbin/fix_permissions";
        processFind->start("\""+sdk+"\"" + "adb shell find " + tool);
        processFind->waitForFinished(-1);
        outputFind = processFind->readAll();
        if (outputFind.contains("No such file"))
        {
          tool = this->adbPushTool("fix_permissions");
        }
    }
    commandRunning = "Not Running";
    this->processStarted();
    QString commandFixPerm="\""+sdk+"\"" + "adb shell su -c '" + tool + "'";
    process->start(commandFixPerm);
    process->waitForReadyRead(-1);
}

void CwmWidget::printProcess(QString processOut)
{
     int i;
     for (i = 0; i < processOut.length(); i++)
     {
          if (processOut.at(i).unicode() == 13)
              processOut[i] = ' ';
          if (processOut.at(i).unicode() == 10)
              processOut[i] = '\n';
     }
     if (processOut.contains(QChar( 0x1b ), Qt::CaseInsensitive))
     {
         processOut.remove("[0m");
         processOut.remove(QChar( 0x1b ), Qt::CaseInsensitive);
         processOut.remove(QRegExp("\\[\\d;\\d+m"));
         this->ui->plainTextEditStatus->insertPlainText(processOut);
      }
      else
      {
         this->ui->plainTextEditStatus->insertPlainText(processOut);
      }
      this->ui->plainTextEditStatus->ensureCursorVisible();
}

void CwmWidget::readFromProcess()
{
    QString processOut = QString::fromUtf8(process->readAllStandardOutput());
    printProcess(processOut);
}

void CwmWidget::readFromProcessWhich()
{
    QTextStream rsyncStdoutStream(processWhich->readAllStandardOutput());
    QString processOut = rsyncStdoutStream.readAll();
    printProcess(processOut);
}

void CwmWidget::buttonsDisabled()
{
    this->ui->buttonFixPermissions->setDisabled(true);
    this->ui->buttonCreate->setDisabled(true);
    this->ui->buttonWipe->setDisabled(true);
    this->ui->buttonInstall->setDisabled(true);
    this->ui->buttonBackup->setDisabled(true);
    this->ui->buttonRestore->setDisabled(true);
    this->ui->buttonUpdate->setDisabled(true);
    this->ui->buttonFlash->setDisabled(true);
    this->ui->tabWidget_2->setTabEnabled(0,false);
    this->ui->checkBacBoot->setDisabled(true);
    this->ui->checkBacCache->setDisabled(true);
    this->ui->checkBacData->setDisabled(true);
    this->ui->checkBacSdext->setDisabled(true);
    this->ui->checkBacSystem->setDisabled(true);
    this->ui->checkAfter->setDisabled(true);
    this->ui->checkBefore->setDisabled(true);
    this->ui->buttonFixMarket->setDisabled(true);
}

QString CwmWidget::adbPushTool(QString toolName)
{
    disconnect(ui->tabWidget_2,SIGNAL(currentChanged(int)),this,SLOT(sdcardDisplay()));
    processFind->start("\""+this->sdk+"\""+"adb push \""+QDir::currentPath()+"/tools/" + toolName + "\" /cache/qtadb/"+ toolName);
    processFind->waitForFinished(-1);
    processFind->start("\""+this->sdk+"\""+"adb shell chmod 755 /cache/qtadb/" + toolName);
    processFind->waitForFinished(-1);
    processFind->start("\""+this->sdk+"\""+"adb shell sync");
    processFind->waitForFinished(-1);
    toolName.prepend("/cache/qtadb/");
    return toolName;
}

void CwmWidget::on_buttonCreate_pressed()
{
   if (QMessageBox::warning(this, tr("Partition SD Card:"),tr("All DATA on SD Card will be LOST!!! \nAre you sure?"),QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
       return;
   if (this->ui->checkBackupSdcars->isChecked())
   {
       ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:white;border: 1px solid #020202;border-radius: 1px;}" );
       this->ui->tabWidget_2->setCurrentIndex(1);
       this->ui->plainTextEditStatus->clear();
       this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
       this->ui->plainTextEditStatus->insertPlainText("SD Card Backup Started...\n");
       this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
       this->ui->plainTextEditStatus->ensureCursorVisible();
       this->buttonsDisabled();
       which = "sdbackup";
       this->backupPath = "/sdcard/LOST.DIR/";


       phone->setPath("/sdcard/");
       QList<File> *fileList = NULL;
       fileList = phone->getFileList();
       this->dialog = new dialogKopiuj(this, fileList, dialogKopiuj::PhoneToComputer,
                                           "/sdcard/", QDir::currentPath()+"/SD_Card/");
       this->dialog->closeAfterFinished();
       this->dialog->show();
       connect(this->dialog, SIGNAL(finished(int)), this, SLOT(finishedWhich()));
       connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
       connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));
       connect(this->dialog, SIGNAL(copyCanceled()), this, SLOT(sdbackupCanceled()));
   }
   else
   {
       which = "sdrestore";
       createPartition();
   }
}

void CwmWidget::createPartition()
{
   if (which == "sdrestore")
   {
       QString size, type;
       if (this->ui->radio128->isChecked())
           size = "128";
       if (this->ui->radio256->isChecked())
           size = "256";
       if (this->ui->radio512->isChecked())
           size = "512";
       if (this->ui->radio1024->isChecked())
           size = "1024";
       if (this->ui->radio2048->isChecked())
           size = "2048";
       if (this->ui->radio4096->isChecked())
           size = "4096";
       if (this->ui->radioExt2->isChecked())
           type = "ext2";
       if (this->ui->radioExt3->isChecked())
           type = "ext3";
       if (this->ui->radioExt4->isChecked())
           type = "ext4";
       this->adbPushTool("e2fsck");
       this->adbPushTool("parted");
       this->adbPushTool("sdparted");
       this->adbPushTool("tune2fs");
       this->adbPushTool("dopartition.sh");
       QString infoLine = "ui_print(\"Partitioning SD Card...\");";
       QString commandLine = "run_program(\"/cache/qtadb/dopartition.sh\", \"" + size + "M\", \"" + type + "\")";
       this->extendedcommandFile(infoLine, commandLine);
       this->extendedcommandFile("execute commands", "");
       this->phone->adbRebootRecovery();
   }
}

void CwmWidget::extendedcommandFile(QString infoLine, QString commandLine)
{
    QFile extendedcommand(QDir::currentPath()+"/tmp/extendedcommand");
    if (!commandLine.isEmpty())
    {
       if (extendedcommand.open(QIODevice::WriteOnly) | QIODevice::Text)
       {
           QTextStream text(&extendedcommand);
           QDateTime current = QDateTime::currentDateTime();
           QString time = current.toString ("ddd, MMMM d, yyyy, h:mm ap");
           text << "ui_print(\"QtADB Process Started...\");\n" << "ui_print(\"" + time + "\");\n" << infoLine + "\n" << commandLine;
           extendedcommand.close();
       }
    }
    if (infoLine == "execute commands")
    {
        processFind->start("\""+this->sdk+"\""+"adb shell rm -f /cache/recovery/command");
        processFind->waitForFinished(-1);
        processFind->start("\""+this->sdk+"\""+"adb push \""+QDir::currentPath()+"/tmp/extendedcommand\" /cache/recovery/");
        processFind->waitForFinished(-1);
        extendedcommand.remove();
        commandRunning = "running";
        readLog = "read";
        this->ui->plainTextEditStatus->clear();
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("Rebooting to Recovery...");
        this->ui->tabWidget_2->setCurrentIndex(1);
    }
}

void CwmWidget::tailLog()
{
    processStarted();
    process->start("\""+this->sdk+"\""+"adb shell tail -f -n 1000 /tmp/recovery.log");
    process->waitForReadyRead(-1);
}

void CwmWidget::readFromLog()
{
    QProcess tmpremove;
    tmpremove.start("\""+this->sdk+"\""+"adb shell rm -r /cache/qtadb/");
    tmpremove.waitForFinished(-1);
    this->ui->plainTextEditStatus->clear();
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->insertPlainText("Log Begins...\n");
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
    processFind->start("\""+this->sdk+"\""+"adb shell find /cache/recovery/log");
    processFind->waitForFinished(-1);
    QString out = processFind->readAll();
    QProcess log;
    log.setProcessChannelMode(QProcess::MergedChannels);
    if (out.contains("No such file"))
        log.start("\""+this->sdk+"\""+"adb shell cat /cache/recovery/last_log");
    else
        log.start("\""+this->sdk+"\""+"adb shell cat /cache/recovery/log");
    log.waitForFinished(-1);
    QString processOut = QString::fromUtf8(log.readAll());
    this->printProcess(processOut);
    this->threadSdcard->sleep(1);
    this->ui->plainTextEditStatus->insertPlainText("\n--------------------\n");
    this->ui->plainTextEditStatus->insertPlainText("Log End!\n");
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    if (commandRunning == "running")
    {
        commandRunning = "Not Running";
        QMessageBox::information(0,"Advanced Menu:","      Process Finished!      ");
    }
    QString sdFailed = this->ui->plainTextEditStatus->toPlainText();
    if (sdFailed.contains("Can't mount /sdcard"))
    {
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:red;border: 1px solid #020202;border-radius: 1px;}" );
        QMessageBox::warning(this, tr("Log:"),"Timed out waiting for SD Card to mount! See Log for details.\n\nYou will need to repeat this operation.");
    }
    else
    {
        if (!this->backupPath.isEmpty())
        {
            this->threadSdcard->sleep(3);
            processFind->start("\""+this->sdk+"\"" + "adb shell find /dev/block/mmc*");
            processFind->waitForFinished(-1);
            QString out = processFind->readAll();
            if (!out.contains("No such file"))
            {
                //this->ui->tabWidget->setCurrentIndex(3);
                this->threadSdcard = new ThreadSdcard;
                this->threadSdcard->backupPath = this->backupPath;
                this->threadSdcard->sdk = this->sdk;
                this->threadSdcard->start();
                connect(this->threadSdcard,SIGNAL(backupVerifiable()),this,SLOT(backupAvailable()));
                this->ui->plainTextEditStatus->insertPlainText("\nWating for SD Card availability...");
                this->ui->plainTextEditStatus->ensureCursorVisible();
            }
            else
            {
                if (QMessageBox::question(this, tr("Reboot:"),"Your phone failed to detect SD Card! Reboot is required.\n\nDo you want to Reboot it now?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                        return;
                if (!this->ui->lineBackup->text().isEmpty())
                {
                    backupPath = this->ui->lineBackup->text();
                    which = "backupVerify";
                }
                readLog == "read";
                this->phone->adbReboot();
            }

        }
    }
}
void CwmWidget::processStarted()
{
    ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:white;border: 1px solid #020202;border-radius: 1px;}" );
    this->ui->tabWidget_2->setCurrentIndex(1);
    this->ui->plainTextEditStatus->clear();
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->insertPlainText("Process Started...\n");
    this->ui->plainTextEditStatus->insertPlainText("--------------------\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    this->buttonsDisabled();
    this->mountsDisable();
    this->ui->buttonSystem->setEnabled(false);
}

void CwmWidget::buttonsEnabled()
{
    this->ui->checkLink2Sd->setEnabled(false);
    this->ui->checkBackupSdcars->setDisabled(false);
    this->ui->checkAfter->setDisabled(false);
    this->ui->checkBefore->setDisabled(false);
    this->ui->buttonFixPermissions->setEnabled(true);
    this->ui->tabWidget_2->setTabEnabled(0,true);
    this->ui->buttonCreate->setDisabled(true);
    this->ui->buttonWipe->setDisabled(true);
    this->ui->checkCache->setDisabled(false);
    this->ui->checkBattery->setDisabled(false);
    this->ui->buttonInstall->setDisabled(true);
    this->ui->buttonBackup->setDisabled(true);
    this->ui->buttonRestore->setDisabled(true);
    this->ui->buttonUpdate->setDisabled(true);
    this->ui->buttonFlash->setDisabled(true);
    this->ui->checkBefore->setEnabled(false);
    this->ui->checkAfter->setEnabled(false);
    this->ui->labelBefore->setEnabled(false);
    this->ui->labelAfter->setEnabled(false);
    this->ui->buttonFixMarket->setDisabled(false);
    this->ui->buttonRecovery->setEnabled(false);
    if (this->ui->radio128->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->radio256->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->radio512->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->radio1024->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->radio2048->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->radio4096->isChecked())
        this->ui->buttonCreate->setEnabled(true);
    if (this->ui->checkData->isChecked())
    {
        this->ui->buttonWipe->setEnabled(true);
        this->ui->checkCache->setChecked(false);
        this->ui->checkBattery->setChecked(false);
        this->ui->checkCache->setDisabled(true);
        this->ui->checkBattery->setDisabled(true);
    }
    if (this->ui->checkCache->isChecked())
        this->ui->buttonWipe->setEnabled(true);
    if (this->ui->checkDalvik->isChecked())
        this->ui->buttonWipe->setEnabled(true);
    if (this->ui->checkBattery->isChecked())
        this->ui->buttonWipe->setEnabled(true);
    if (this->ui->radioInstallPath->isChecked())
    {
        this->ui->buttonInstall->setDisabled(false);
        this ->ui->lineInstallPath->setDisabled(false);
    }
    if (this->ui->radioUpdateZip->isChecked())
    {
        this->ui->buttonInstall->setDisabled(false);
        this ->ui->lineInstallPath->setDisabled(true);
        this->ui->buttonInsert->setEnabled(false);
    }
    if (this->ui->checkDefaultBackupDir->isChecked())
    {
        if (this->backupPath.isEmpty())
        {
            QDateTime current = QDateTime::currentDateTime();
            QString time = current.toString ("ddd, MMMM d, yyyy, h:mm ap");
            time.remove(",");
            time.replace(" ","-");
            time.replace(":","_");
            this->ui->lineBackup->setText("/sdcard/clockworkmod/backup/" + time.append("/"));
        }
        this->ui->buttonInsert->setEnabled(false);
        this->ui->buttonBackup->setDisabled(false);
    }
    else
    {
        this->ui->lineBackup->clear();
        this->activateButtonInsert();
    }
    if (!this->ui->lineBackup->text().isEmpty())
        this->ui->buttonBackup->setEnabled(true);
    if (!this->ui->lineRestore->text().isEmpty())
    {
        this->ui->checkBacBoot->setEnabled(true);
        this->ui->checkBacCache->setEnabled(true);
        this->ui->checkBacData->setEnabled(true);
        this->ui->checkBacSdext->setEnabled(true);
        this->ui->checkBacSystem->setEnabled(true);
        if (this->ui->checkBacBoot->isChecked())
            this->ui->buttonRestore->setEnabled(true);
        if (this->ui->checkBacCache->isChecked())
            this->ui->buttonRestore->setEnabled(true);
        if (this->ui->checkBacData->isChecked())
            this->ui->buttonRestore->setEnabled(true);
        if (this->ui->checkBacSdext->isChecked())
            this->ui->buttonRestore->setEnabled(true);
        if (this->ui->checkBacSystem->isChecked())
            this->ui->buttonRestore->setEnabled(true);
    }
    if (!this->ui->lineUpdate->text().isEmpty())
        this->ui->buttonUpdate->setEnabled(true);
    if (!this->ui->lineFlash->text().isEmpty())
    {
        this->ui->buttonFlash->setEnabled(true);
        this->ui->checkBefore->setEnabled(true);
        this->ui->checkAfter->setEnabled(true);
        this->ui->labelBefore->setEnabled(true);
        this->ui->labelAfter->setEnabled(true);
    }
    if (this->ui->tabWidget_3->currentIndex() == 1)
    {
        QProcess down;
        down.start("\"" + sdk + "\"" + "adb shell ls -l /cache/download");
        down.waitForReadyRead(-1);
        QString out = down.readAll();
        if (out.contains("/cache/download ->"))
        {
            down.start("\"" + sdk + "\"" + "adb shell ls -l /sdcard/cache/download");
            down.waitForReadyRead(-1);
            QString out = down.readAll();
            if (!out.contains("No such file"))
            {
                this->ui->buttonFixMarket->setText("Undo Fix Market");
                this->ui->label_12->setText("<font color=\"green\">Your Market Cache Download is already moved to SD Card!</font>");
            }
        }
        else
        {
            this->ui->buttonFixMarket->setText("Fix Market");
            this->ui->label_12->setText("Fixes the \"insufficient space\" issue in downloading applications from Google Market by moving Market Download Cache to SD Card.");
        }
    }
    if (!this->ui->lineRecovery->text().isEmpty())
        this->ui->buttonRecovery->setEnabled(true);
    if (this->ui->tabWidget->currentIndex() == 5)
    {
        if (!this->ui->lineUpdate->text().isEmpty())
        {
            QProcess link;
            link.start("\"" + sdk + "\"" + "adb shell find /system/etc/init.d/11link2sd");
            link.waitForReadyRead(-1);
            QString out = link.readAll();
            if (!out.contains("No such file"))
                this->ui->checkLink2Sd->setEnabled(true);
            else
                this->ui->checkLink2Sd->setText("Link2SD not detected.");
        }
    }
}

void CwmWidget::on_buttonSdinfo_pressed()
{
    this->adbPushTool("parted");
    this->processStarted();
    process->start("\""+this->sdk+"\""+"adb shell /cache/qtadb/parted /dev/block/mmcblk0 print");
    process->waitForFinished(-1);
    this->readFromProcess();
}

void CwmWidget::on_buttonWipe_pressed()
{
    QString mess, wipeData, wipeCache, wipeDalvik, wipeBattery, command, data, cache, dalvik, battery;
    mess = wipeData = wipeCache = wipeBattery = wipeDalvik = data = cache = dalvik = battery = "";
    if (this->ui->checkData->isChecked())
    {
        wipeData = "\n---Data/Factory Reset - this will erase all user data in Recovery(NOT in Android Privacy Settings), formating /data, /cache, /sd-ext and /sdcard/.android_secure;";
        data = "assert(format(\"/data\"));\nassert(format(\"/cache\"));\nassert(format(\"/sd-ext\"));\nassert(format(\"/sdcard/.android_secure\"));\n";
    }
    if (this->ui->checkCache->isChecked())
    {
        wipeCache = "\n---Cache - this will erase /cache;";
        cache = "assert(format(\"/cache\"));\n";
    }
    if (this->ui->checkDalvik->isChecked())
    {
        wipeDalvik = "\n---Dalvik Cache - this will erase /dalvik-cache in /data, /cache and /sd-ext;";
        this->adbPushTool("dowipedalvikcache.sh");
        dalvik = "run_program(\"/cache/qtadb/dowipedalvikcache.sh\");\n";
    }
    if (this->ui->checkBattery->isChecked())
    {
        wipeBattery = "\n---Battery Stats - this will erase /data/system/batterystats.bin reseting Battery Statistics;";
        this->adbPushTool("dowipebatterystats.sh");
        battery = "run_program(\"/cache/qtadb/dowipebatterystats.sh\");\n";
    }
    mess = wipeData + wipeCache + wipeDalvik + wipeBattery;
    if (QMessageBox::warning(this, tr("Wipe:"),"Wipe:" + mess + "\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    command = dalvik+ battery + data + cache;
    this->extendedcommandFile("ui_print(\"Wiping...\");", command.trimmed());
    this->extendedcommandFile("execute commands", "");
    this->phone->adbRebootRecovery();
}

void CwmWidget::on_buttonInstall_pressed()
{
    QString installPath;
    if (this->ui->radioUpdateZip->isChecked())
        installPath = "/sdcard/update.zip";
    if (this->ui->radioInstallPath->isChecked())
    {
        installPath = this ->ui->lineInstallPath->text();
        if (installPath.isEmpty())
        {
            QMessageBox::information(this, tr("Install Zip:"),tr("Select and confirm Zip to install from /sdcard below!"));
            return;
        }
        if (!installPath.startsWith("/sdcard/"))
        {
            QMessageBox::information(this, tr("Install Zip:"),tr("Zip must be located on /sdcard !"));
            return;
        }
    }
    if (QMessageBox::question(this, tr("Install Zip:"),"Your phone will reboot in Recovery and \"" + installPath + "\" will be installed.\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    this->extendedcommandFile("ui_print(\"Installing...\");", "install_zip(\"" + installPath + "\");");
    this->extendedcommandFile("execute commands", "");
    this->phone->adbRebootRecovery();
}

void CwmWidget::on_buttonInsert_pressed()
{
    QModelIndexList indexList = sdcardTableView->selectionModel()->selectedRows(1);
    if (indexList.isEmpty())
    {
        QMessageBox::information(this,"Insert Selection:",tr("Select some item first!"),QMessageBox::Ok);
        return;
    }
    if (indexList.size() > 1)
    {
        QMessageBox::information(this,"Insert Selection:",tr("Single item selection only!"),QMessageBox::Ok);
        return;
    }
    if (indexList.size() == 1)
    {
        QModelIndex index = phoneSortModel->mapToSource(indexList.takeFirst());
        File tmpFile = phoneModel->getFile(index.row());
        QString itemPath = tmpFile.filePath;
        File::fileTypes itemType = tmpFile.fileType;
        if (this->ui->tabWidget->currentIndex() == 2 && this->ui->radioInstallPath->isChecked())
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
                this ->ui->lineInstallPath->setText(itemPath);
            else
                QMessageBox::information(this,"Insert Selection:",tr("Valid Zip file must be selected!"));
        }
        if (this->ui->tabWidget->currentIndex() == 3 && !this->ui->checkDefaultBackupDir->isChecked())
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::dir)
            {
                if (itemPath.contains(" "))
                {
                    QMessageBox::information(this,"Insert Selection:",tr("No empty space allowed in Directory Path/Name!"));
                    return;
                }
                processFind->start("\""+this->sdk+"\""+"adb shell ls \"" + this->codec->toUnicode(itemPath.toUtf8()) + "\"");
                processFind->waitForFinished(-1);
                QString output = processFind->readAll();
                if (output.contains("nandroid.md5"))
                    if (QMessageBox::question(this, tr("Insert Selection:"),"Directory \"" + itemPath + "\" seems to contain Nandroid Backup already.\nDo you want to overwrite it?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                        return;
                disconnect(this->ui->lineBackup, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
                this ->ui->lineBackup->setText(itemPath);
                this->ui->buttonBackup->setEnabled(true);
                connect(this->ui->lineBackup, SIGNAL(textChanged(QString)),this,SLOT(buttonsEnabled()));
            }
            else
                QMessageBox::information(this,"Insert Selection:",tr("Directory must be selected!"));
        }
        if (this->ui->tabWidget->currentIndex() == 4)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::dir)
            {
                this ->ui->lineRestore->setText(itemPath);
                if (QMessageBox::question(this, tr("Insert Selection:"),"Do you want to check Backup's MD5 Sum before Restore?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                    return;
                this ->ui->lineRestore->setDisabled(true);
                this->buttonsDisabled();
                this->mountsDisable();
                this->ui->buttonSystem->setEnabled(false);
                which = "md5sum";
                ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:white;border: 1px solid #020202;border-radius: 1px;}" );
                this->ui->plainTextEditStatus->clear();
                this->ui->plainTextEditStatus->insertPlainText("---------------------------\n");
                this->ui->plainTextEditStatus->insertPlainText("Backup Verify: Started...\n");
                this->ui->plainTextEditStatus->insertPlainText("---------------------------\n");
                this->ui->tabWidget_2->setCurrentIndex(1);
                this->adbPushTool("domd5sum.sh");
                processWhich->start("\""+sdk+"\"" + "adb shell /cache/qtadb/domd5sum.sh " + this->codec->toUnicode(itemPath.toUtf8()));
                processWhich->waitForReadyRead(-1);
            }
        }
        if (this->ui->tabWidget->currentIndex() == 5)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
                this ->ui->lineUpdate->setText(itemPath);
            else
                QMessageBox::information(this,"Insert Selection:",tr("Valid ROM file must be selected!"));
        }
        if (this->ui->tabWidget->currentIndex() == 6)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
            {
                if (!this->ui->checkBefore->isChecked() && !this->ui->checkAfter->isChecked())
                    this ->ui->lineFlash->setText(itemPath);
                else
                {
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle("Insert Selection:");
                    msgBox.setText("File: " + itemPath);
                    msgBox.setInformativeText("What is this file for:");
                    QPushButton *before = msgBox.addButton(tr("  Install BEFORE  "), QMessageBox::AcceptRole);
                    QPushButton *rom = msgBox.addButton(tr("   ROM to Flash   "),QMessageBox::DestructiveRole);
                    QPushButton *after = msgBox.addButton(tr("  Install  AFTER  "),QMessageBox::ActionRole);
                    msgBox.addButton(QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Cancel);
                    if (!this->ui->checkBefore->isChecked())
                        msgBox.removeButton(before);
                    if (!this->ui->checkAfter->isChecked())
                        msgBox.removeButton(after);
                    msgBox.exec();
                    if (msgBox.clickedButton() == before)
                        this ->ui->labelBefore->setText("<font color=\"green\">" + itemPath + "</font>");
                    else if (msgBox.clickedButton() == rom)
                       this ->ui->lineFlash->setText(itemPath);
                    else if (msgBox.clickedButton() == after)
                       this ->ui->labelAfter->setText("<font color=\"green\">" + itemPath + "</font>");
                }
            }
            else
                QMessageBox::information(this,"Insert Selection:",tr("Valid ROM file must be selected!"));
        }
        if (this->ui->tabWidget_3->currentIndex() == 2)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".img"))
                this ->ui->lineRecovery->setText(itemPath);
            else
                QMessageBox::information(this,"Insert Selection:",tr("Valid IMG file must be selected!"));
        }
    }
}

void CwmWidget::activateButtonInsert()
{
    QModelIndexList indexList = sdcardTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = phoneSortModel->mapToSource(indexList.takeFirst());
        File tmpFile = phoneModel->getFile(index.row());
        QString itemPath = tmpFile.filePath;
        File::fileTypes itemType = tmpFile.fileType;
        this->ui->lineEditPath->setText(itemPath);
        if (this->ui->tabWidget->currentIndex() == 2 && this->ui->radioInstallPath->isChecked())
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
            {
                this ->ui->buttonInsert->setEnabled(true);
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
        if (this->ui->tabWidget->currentIndex() == 3 && !this->ui->checkDefaultBackupDir->isChecked())
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::dir)
            {
                this ->ui->buttonInsert->setEnabled(true);
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
        if (this->ui->tabWidget->currentIndex() == 4)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::dir)
            {
                processFind->start("\""+this->sdk+"\""+"adb shell ls \"" + this->codec->toUnicode(itemPath.toUtf8()) + "\"");
                processFind->waitForFinished(-1);
                QString output = processFind->readAll();
                if (output.contains("nandroid.md5"))
                {
                    this ->ui->buttonInsert->setEnabled(true);
                }
                else
                {
                    this ->ui->buttonInsert->setEnabled(false);
                }
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
        if (this->ui->tabWidget->currentIndex() == 5)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
            {
                this ->ui->buttonInsert->setEnabled(true);
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
        if (this->ui->tabWidget->currentIndex() == 6)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".zip"))
            {
                this ->ui->buttonInsert->setEnabled(true);
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
        if (this->ui->tabWidget_3->currentIndex() == 2)
        {
            if (itemPath.startsWith("/sdcard/") && itemType == File::file && itemPath.endsWith(".img"))
            {
                this ->ui->buttonInsert->setEnabled(true);
            }
            else
            {
                this ->ui->buttonInsert->setEnabled(false);
            }
        }
    }
    else
    {
        this ->ui->buttonInsert->setEnabled(false);
    }
    if (this ->ui->buttonInsert->isEnabled())
        this->ui->lineEditPath->setStyleSheet( "QLineEdit {background-color:white;color:green;}" );
    else
        this->ui->lineEditPath->setStyleSheet( "QLineEdit {background-color:white;color:red;}" );
}

void CwmWidget::on_buttonBackup_pressed()
{
    backupPath = this->ui->lineBackup->text();
    if (backupPath.isEmpty())
    {
        QMessageBox::information(this, tr("Backup ROM:"),tr("Select and confirm Directory for Nandroid Backup from /sdcard below!"));
        return;
    }
    if (!backupPath.startsWith("/sdcard/"))
    {
        QMessageBox::information(this, tr("Backup ROM:"),tr("The Directory for Nandroid Backup must be located on /sdcard !"));
        return;
    }
    if (QMessageBox::question(this, tr("Backup ROM:"),"Your phone will reboot in Recovery and Nandroid Backup will be made in \"" + backupPath + "\".\n\nDo you want to proceed?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    which = "backupVerify";
    this->extendedcommandFile("ui_print(\"Backing up...\");", "assert(backup_rom(\"" + backupPath + "\"));");
    this->extendedcommandFile("execute commands", "");
    this->phone->adbRebootRecovery();
}

void CwmWidget::md5sumVerify()
{
    QString result = this->ui->plainTextEditStatus->toPlainText();
    if (result.contains("FAILED") || result.contains("WARNING"))
    {
        this ->ui->lineRestore->clear();
        this ->ui->lineRestore->setDisabled(false);
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:red;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("-------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("Backup Verify: FAILED!!!\n");
        this->ui->plainTextEditStatus->insertPlainText("-------------------------\n");
        QMessageBox::warning(this,"Insert Selection:",tr("This Nandroid Backup is CORRUPT!\nSee result for details."),QMessageBox::Ok);
    }
    else
    {
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("Backup Verify: SUCCESS!!!\n");
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        QMessageBox::information(this,"Insert Selection:",tr("This Nandroid Backup is GOOD."),QMessageBox::Ok);
        this ->ui->lineRestore->setDisabled(false);
        this ->ui->lineBackup->clear();
        this ->ui->checkDefaultBackupDir->setChecked(false);
    }
    if (!this->backupPath.isEmpty())
        this->backupPath = this->threadSdcard->backupPath = "";
}

void CwmWidget::on_buttonRestore_pressed()
{
    if (QMessageBox::question(this, tr("Restore Backup:"),"Your phone will reboot in Recovery and the selected Nandroid Backup will be restored.\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    QString no, no1, no2, no3, no4;
    no = no1 = no2 = no3 = no4 = "no";
    if (this->ui->checkBacBoot->isChecked())
        no = "";
    if (this->ui->checkBacSystem->isChecked())
        no1 = "";
    if (this->ui->checkBacData->isChecked())
        no2 = "";
    if (this->ui->checkBacCache->isChecked())
        no3 = "";
    if (this->ui->checkBacSdext->isChecked())
        no4 = "";
    QString restorePath = this->ui->lineRestore->text();
    QString arg = "\"" + no + "boot\"" + ", \"" + no1 + "system\"" + ", \"" + no2 + "data\"" + ", \"" + no3 + "cache\"" + ", \"" + no4 + "sd-ext\"";
    this->extendedcommandFile("ui_print(\"Restoring Nandroid Backup...\");", "assert(restore_rom(\"" + restorePath + "\", " + arg + "));");
    this->extendedcommandFile("execute commands", "");
    this->phone->adbRebootRecovery();
}

void ThreadSdcard::run()
{
    QProcess sd;
    do
    {
        this->sleep(5);
        sd.start("\""+this->sdk+"\"" + "adb shell find \"" + this->backupPath + "\"");
        sd.waitForFinished(-1);
        QString outsd = sd.readAll();
        if (!outsd.contains("No such file"))
            break;
    }
    while (1 < 2);
    emit backupVerifiable();
}

void CwmWidget::backupAvailable()
{
    this->threadSdcard->quit();
    disconnect(this->threadSdcard,SIGNAL(backupVerifiable()),this,SLOT(backupAvailable()));
    QTextCursor tc = this->ui->plainTextEditStatus->textCursor();
    tc.movePosition( QTextCursor::StartOfLine );
    tc.select( QTextCursor::LineUnderCursor );
    tc.removeSelectedText();
    this->ui->plainTextEditStatus->insertPlainText("SD Card available...\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    if (which == "sdrestore" && QDir(QDir::currentPath()+"/SD_Card").exists())
    {
        if (!this->threadSdcard->backupPath.isEmpty())
            this->threadSdcard->backupPath = this->backupPath = "";
        this->backupPath = "";
        if (QMessageBox::question(this, tr("Create /sd-ext:"),"SD Card is repartitioned now.\n\nDo you want to Restore it's content to the newly created first fat32 partition?\n\nYou can answer \"No\" and do it manually later. The SD Card content is located in \n\"" + currentDir.replace("/","\\")+"\\SD_Card\".",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            which = "";
            return;
        }
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:white;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->tabWidget_2->setCurrentIndex(1);
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("SD Card Restore Started...\n");
        this->ui->plainTextEditStatus->insertPlainText("-----------------------------\n");
        this->ui->plainTextEditStatus->ensureCursorVisible();
        processStarted();
        QList<File> *fileList = NULL;
        fileList = this->getFileList();
            this->dialog = new dialogKopiuj(this, fileList, dialogKopiuj::ComputerToPhone,
                                             QDir::currentPath()+"/SD_Card/","/sdcard/" );
        this->dialog->closeAfterFinished();
        this->dialog->show();
        connect(this->dialog, SIGNAL(finished(int)), this, SLOT(finishedWhich()));
        connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
        connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));
        connect(this->dialog, SIGNAL(copyCanceled()), this, SLOT(sdrestoreCanceled()));
    }
    else if (which == "")
        return;
    else if (which == "backupVerify")
    {
        this->ui->lineBackup->setText(this->threadSdcard->backupPath);
        if (QMessageBox::question(this, tr("Backup ROM:"),tr("Do yuo want to verify MD5 Sum of the Backup you just did?\n\n You can press \"No\" and do it later by selecting it in \"Restore Backup\" tab."),QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
        this->buttonsDisabled();
        mountsDisable();
        which = "md5sum";
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:white;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->clear();
        this->ui->plainTextEditStatus->insertPlainText("---------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("Backup Verify: Started...\n");
        this->ui->plainTextEditStatus->insertPlainText("---------------------------\n");
        this->ui->tabWidget_2->setCurrentIndex(1);
        this->adbPushTool("domd5sum.sh");
        processWhich->start("\""+sdk+"\"" + "adb shell /cache/qtadb/domd5sum.sh \"" + this->threadSdcard->backupPath + "\"" );
        processWhich->waitForReadyRead(-1);
    }
}

void CwmWidget::on_buttonUpdate_pressed()
{
   QString updatePath = this->ui->lineUpdate->text();
   if (QMessageBox::question(this, tr("Update Current ROM:"),"Your phone will reboot in Recovery and the current ROM will be updated with \"" + updatePath + "\".\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
           return;
   if (this->ui->checkLink2Sd->isChecked())
   {
       this->adbPushTool("dowipedalviklink2sd.sh");
       this->adbPushTool("dolink2sd.sh");
       this->extendedcommandFile("ui_print(\"Updating Current ROM...\");", "run_program(\"/cache/qtadb/dowipedalviklink2sd.sh\");\nassert(format(\"/cache\"));\ninstall_zip(\"" + updatePath + "\");\nrun_program(\"/sbin/dolink2sd.sh\");");
   }
   else
   {
   this->adbPushTool("dowipedalvikcache.sh");
   this->extendedcommandFile("ui_print(\"Updating Current ROM...\");", "run_program(\"/cache/qtadb/dowipedalvikcache.sh\");\nassert(format(\"/cache\"));\ninstall_zip(\"" + updatePath + "\");");
   }
   this->extendedcommandFile("execute commands", "");
   this->phone->adbRebootRecovery();
}

void CwmWidget::checkFlash()
{
    QString before = this->ui->labelBefore->text();
    QString after = this->ui->labelAfter->text();
    if (this->ui->checkBefore->isChecked())
    {
        if (before == "(like some wiper...)")
        {
            this->ui->labelBefore->setText( "<font color=\"red\">Select Zip from /sdcard below...</font>" );
        }
    }
    else
        this->ui->labelBefore->setText( "(like some wiper...)" );
    if (this->ui->checkAfter->isChecked())
    {
        if (after == "(like Google Apps, Theme...)")
        {
            this->ui->labelAfter->setText( "<font color=\"red\">Select Zip from /sdcard below...</font>" );
        }
    }
    else
        this->ui->labelAfter->setText( "(like Google Apps, Theme...)" );
}

void CwmWidget::on_buttonFlash_pressed()
{
    QTextDocument docBefore, docAfter;
    docBefore.setHtml(this->ui->labelBefore-> text());
    docAfter.setHtml(this->ui->labelAfter-> text());
    QString beforePath = docBefore.toPlainText();
    QString romPath = this->ui->lineFlash->text();
    QString afterPath = docAfter.toPlainText();
    QString msgBefore, msgAfter, commandBefore, commandAfter;
    msgBefore = msgAfter = commandBefore = commandAfter = "";
    if (this->ui->checkBefore->isChecked())
    {
        if (beforePath.startsWith("/sdcard/"))
        {
            msgBefore = "\n---\"" + beforePath + "\" will be installed;";
            commandBefore = "install_zip(\"" + beforePath + "\");\n";
        }
        else
        {
            QMessageBox::information(this,"Flash New ROM:",tr("Did you check \"Install Zip...\" option(s)?\n\nSelect Zip(s) from /sdcard below and press [Confirm Selection]!"));
            return;
        }
    }
    if (this->ui->checkAfter->isChecked())
    {
        if (afterPath.startsWith("/sdcard/"))
        {
            msgAfter = "\n---\"" + afterPath + "\" will be installed;";
            commandAfter = "install_zip(\"" + afterPath + "\");";
        }
        else
        {
            QMessageBox::information(this,"Flash New ROM:",tr("Select Zip(s) to install from /sdcard below and press [Confirm Selection]!"));
            return;
        }
    }
    if (QMessageBox::question(this, tr("Flash ROM:"),"Your phone will reboot in Recovery and the following actions will be performed:\n---dalvik-cache will be wiped;\n---/boot, /system, /data and /cache partitions will be formated;" + msgBefore + "\n---\"" + romPath + "\" will be flashed;" + msgAfter + "\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
         return;
    this->adbPushTool("dowipedalvikcache.sh");
    QString dalvik = "run_program(\"/cache/qtadb/dowipedalvikcache.sh\");\nassert(format(\"/data\"));\nassert(format(\"/cache\"));\nassert(format(\"/boot\"));\nassert(format(\"/system\"));\n";
    this->extendedcommandFile("ui_print(\"Flashing New ROM...\");", "run_program(\"/cache/qtadb/dowipedalvikcache.sh\");\nassert(format(\"/data\"));\nassert(format(\"/cache\"));\nassert(format(\"/boot\"));\nassert(format(\"/system\"));\n" + commandBefore + "install_zip(\"" + romPath + "\");\n" + commandAfter);
    this->extendedcommandFile("execute commands", "");
    this->phone->adbRebootRecovery();
}

void CwmWidget::on_buttonFixMarket_pressed()
{
    QString market;
    commandRunning = "Not Running";
    if (this->ui->buttonFixMarket->text() == "Undo Fix Market")
        market = this->adbPushTool("undofixmarket.sh");
    else if (this->ui->buttonFixMarket->text() == "Fix Market")
        market = this->adbPushTool("dofixmarket.sh");
    this->processStarted();
    process->start("\"" + sdk + "\"" + "adb shell " + market);
    process->waitForReadyRead(-1);
}

void CwmWidget::on_buttonRecovery_pressed()
{
    if (QMessageBox::question(this, tr("Flash New Recovery:"),"New Recovery is about to be flashed.\n\nAre you sure?",QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    which = "flash recovery";
    QString imgPath = this->ui->lineRecovery->text();
    QString tool = "/system/bin/flash_image";
    processFind->start("\""+sdk+"\"" + "adb shell find " + tool);
    processFind->waitForFinished(-1);
    QString outputFind = processFind->readAll();
    if (outputFind.contains("No such file"))
          tool = this->adbPushTool("flash_image");
    this->processStarted();
    process->start("\""+sdk+"\"" + "adb shell su -c \"" + tool + " recovery '" + this->codec->toUnicode(imgPath.toUtf8()) + "'\"");
    process->waitForReadyRead(-1);
}

void CwmWidget::sdbackupCanceled()
{
    if (!this->computer->deleteDir(QDir::currentPath()+"/SD_Card/"))
        QMessageBox::information(this,tr("Delete SD Card Backup Folder:"),"Unable to remove \"" + currentDir.replace("/","\\")+"\\SD_Card\" Backup folder!\n\nRemove it manually.");
    if (QMessageBox::question(this, tr("Backup SD Card:"),"Backup SD Card was Canceled.\n\nDo you want to continue with Partitioning the SD Card?\nALL SD Card DATA WILL BE LOST!!!",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
        if (!this->threadSdcard->backupPath.isEmpty())
            this->threadSdcard->backupPath = this->backupPath = "";
        this->backupPath = "";
        which = "";
        commandRunning = "Not Running";
        ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
        this->ui->plainTextEditStatus->insertPlainText("----------------------------------\n");
        this->ui->plainTextEditStatus->insertPlainText("SD Card Backup Canceled!\n");
        this->ui->plainTextEditStatus->insertPlainText("SD Card Partitioning Canceled!\n");
        this->ui->plainTextEditStatus->insertPlainText("----------------------------------\n");
        this->ui->plainTextEditStatus->ensureCursorVisible();
        buttonsEnabled();
        this->ui->buttonSystem->setEnabled(true);
        if (this->phone->getConnectionState() == RECOVERY)
            mountsEnable();
        return;
    }
    finishedWhich();
}

void CwmWidget::sdrestoreCanceled()
{
    if (!this->threadSdcard->backupPath.isEmpty())
        this->threadSdcard->backupPath = this->backupPath = "";
    this->backupPath = "";
    which = "";
    commandRunning = "Not Running";
    ui->plainTextEditStatus->setStyleSheet( "QPlainTextEdit {background-color:black;color:lightgreen;border: 1px solid #020202;border-radius: 1px;}" );
    this->ui->plainTextEditStatus->insertPlainText("-----------------------------------\n");
    this->ui->plainTextEditStatus->insertPlainText("SD Card Partitioning Finished!\n");
    this->ui->plainTextEditStatus->insertPlainText("SD Card Restore Canceled!\n");
    this->ui->plainTextEditStatus->insertPlainText("-----------------------------------\n");
    this->ui->plainTextEditStatus->ensureCursorVisible();
    buttonsEnabled();
    this->ui->buttonSystem->setEnabled(true);
    if (this->phone->getConnectionState() == RECOVERY)
        mountsEnable();
    if (QMessageBox::question(this, tr("Restore SD Card:"),"Restore SD Card was Canceled.\n\nDo you want to remove \"" + currentDir.replace("/","\\")+"\\SD_Card\" Backup folder?",QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;
    if (!this->computer->deleteDir(QDir::currentPath()+"/SD_Card/"))
        QMessageBox::information(this,tr("Delete SD Card Backup Folder:"),"Unable to remove \"" + currentDir.replace("/","\\")+"\\SD_Card\" Backup folder!\n\nRemove it manually.");
}

QList<File> *CwmWidget::getFileList()
{
    QDir directory;
    directory.setPath(QDir::currentPath()+"/SD_Card/");
    QFileInfoList fileInfoList = directory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfo fileInfo;
    File file;
    QList<File> *fileList = new QList<File>;
    QFileIconProvider *provider = new QFileIconProvider;
    while (!fileInfoList.isEmpty())
    {
        fileInfo = fileInfoList.takeFirst();
        file.fileIcon = provider->icon(fileInfo);
        file.fileName = fileInfo.fileName();
        file.fileSize = QString::number(fileInfo.size());
        file.fileDate = fileInfo.lastModified().toString("MMM dd yyyy");
        file.filePath = fileInfo.absoluteFilePath();
        file.filePermissions = 0;
        file.fileOwner = fileInfo.owner();
        if (fileInfo.isDir())
            file.fileType = File::dir;
        else
            file.fileType = File::file;
        fileList->append(file);
    }
    delete provider;
    return fileList;
}

void CwmWidget::mountsEnable()
{
    this->ui->buttonCache->setEnabled(true);
    this->ui->buttonData->setEnabled(true);
    this->ui->buttonSystem->setEnabled(true);
    this->ui->buttonSdext->setEnabled(true);
    this->ui->buttonSdcard->setEnabled(true);
}

void CwmWidget::mountsDisable()
{
    this->ui->buttonCache->setEnabled(false);
    this->ui->buttonData->setEnabled(false);
  //  this->ui->buttonSystem->setEnabled(false);
    this->ui->buttonSdext->setEnabled(false);
    this->ui->buttonSdcard->setEnabled(false);
}

void CwmWidget::mountsUpdate()
{
    if (this->ui->tabWidget_3->currentIndex() == 3)
    {
        QProcess df;
        df.start("\""+sdk+"\"" + "adb shell df");
        df.waitForFinished(-1);
        QString out = df.readAll();
        if (out.contains("/cache"))
            this->ui->buttonCache-> setText("Unmount /cache");
        else
            this->ui->buttonCache-> setText("Mount /cache");
        if (out.contains("/data"))
            this->ui->buttonData-> setText("Unmount /data");
        else
            this->ui->buttonData-> setText("Mount /data");
        if (this->phone->getConnectionState() == RECOVERY)
        {
            if (out.contains("/system"))
                this->ui->buttonSystem-> setText("Unmount /system");
            else
                this->ui->buttonSystem-> setText("Mount /system");
        }
        if (this->phone->getConnectionState() == DEVICE)
        {
            df.start("\""+sdk+"\"" + "adb shell mount");
            df.waitForFinished(-1);
            out = df.readAll();
            if (out.contains("/system type yaffs2 (ro"))
                this->ui->buttonSystem-> setText("Remount /system Read/Write");
            else if (out.contains("/system type yaffs2 (rw"))
                this->ui->buttonSystem-> setText("Remount /system Read-Only");
        }
        if (out.contains("/sdext") || out.contains("/sd-ext"))
            this->ui->buttonSdext-> setText("Unmount /sd-ext");
        else
            this->ui->buttonSdext-> setText("Mount /sd-ext");
        if (out.contains("/sdcard"))
            this->ui->buttonSdcard-> setText("Unmount /sdcard");
        else
            this->ui->buttonSdcard-> setText("Mount /sdcard");
    }
}

void CwmWidget::on_buttonCache_pressed()
{
    QString command = "";
    if (this->ui->buttonCache->text() == "Unmount /cache")
        command = "umount";
    else if (this->ui->buttonCache->text() == "Mount /cache")
        command = "mount";
    processFind->start("\""+sdk+"\"" + "adb shell " + command + " /cache");
    processFind->waitForFinished(-1);
    mountsUpdate();
}

void CwmWidget::on_buttonData_pressed()
{
    QString command = "";
    if (this->ui->buttonData->text() == "Unmount /data")
        command = "umount";
    else if (this->ui->buttonData->text() == "Mount /data")
        command = "mount";
    processFind->start("\""+sdk+"\"" + "adb shell " + command + " /data");
    processFind->waitForFinished(-1);
    mountsUpdate();
}

void CwmWidget::on_buttonSystem_pressed()
{
    QString command = "";
    if (this->ui->buttonSystem->text() == "Unmount /system")
        command = "umount";
    else if (this->ui->buttonSystem->text() == "Mount /system")
        command = "mount";
    else if (this->ui->buttonSystem->text() == "Remount /system Read/Write")
        command = "mount -o remount,rw";
    else if (this->ui->buttonSystem->text() == "Remount /system Read-Only")
        command = "mount -o remount,ro,noatime";
    processFind->start("\""+sdk+"\"" + "adb shell " + command + " /system");
    processFind->waitForFinished(-1);
    mountsUpdate();
}

void CwmWidget::on_buttonSdext_pressed()
{
    QString command = "";
    if (this->ui->buttonSdext->text() == "Unmount /sd-ext")
        command = "umount";
    else if (this->ui->buttonSdext->text() == "Mount /sd-ext")
        command = "mount";
    processFind->start("\""+sdk+"\"" + "adb shell " + command + " /sd-ext");
    processFind->waitForFinished(-1);
    mountsUpdate();
}

void CwmWidget::on_buttonSdcard_pressed()
{
    QString command = "";
    if (this->ui->buttonSdcard->text() == "Unmount /sdcard")
        command = "umount";
    else if (this->ui->buttonSdcard->text() == "Mount /sdcard")
        command = "mount";
    processFind->start("\""+sdk+"\"" + "adb shell " + command + " /sdcard");
    processFind->waitForFinished(-1);
    mountsUpdate();
}
