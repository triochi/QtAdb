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


#include "filewidget.h"
#include "ui_filewidget.h"

quint32 qbytearrayToInt32(QByteArray array)
{
    QDataStream stream(array);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint32 x;
    stream>>x;
    return x;
}

quint16 qbytearrayToInt16(QByteArray array)
{
    QDataStream stream(array);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint16 x;
    stream>>x;
    return x;
}

bool FileWidget::unpack(QString inFile,QString outPath,QString fileToUnpack,QString outName)
{
    QFile * file = new QFile(inFile);
    if (!file->open(QIODevice::ReadOnly))
    {
        delete file;
        return false;
    }
    if (inFile.isEmpty() || outPath.isEmpty() || fileToUnpack.isEmpty() || outName.isEmpty())
    {
//        file->remove();
        file->close();
        delete file;
        return false;
    }
    //ZIP end of central directory record
    QByteArray endCentralSig, numOfDisk, centralStarts, numberOfRecords, totalNumberOfRecords, sizeOfCentral, offsetOfCentral, zipComment;
    //ZIP central directory file header
    QByteArray centralSig, versionMade, minimumVer, generalPurpose, compMethod, fileModTime, fileModDate, crc, compSize, uncompSize;
    QByteArray fileNameLength, extraFieldLength, fileCommentLength, diskNum, inFileAtt, extFileAtt, fileOffset, fileName, extraField, fileComment;
    //File header
    QByteArray fSig, fVer, fGen, fCom, fModTime, fModDate, fCrc, fComp, fUncomp, fNameL, fExtraL, fName, fExtra;
    QByteArray fileArray;
    QByteArray signature = QByteArray::fromHex("504B0506");


    int filePos, sigPos, arraySize;



    file->seek(file->size() - 500);
    fileArray = file->read(500);
    while (!fileArray.contains(signature))
    {
        arraySize = fileArray.size();
        filePos = file->pos();
        if (!file->seek(filePos - 1000))
            break;
        fileArray.prepend(file->read(500));
    }
    sigPos = fileArray.indexOf(signature);
    sigPos = file->size() - (fileArray.size() - sigPos);
    if (file->seek(sigPos))
    {
        filePos = file->pos();
        endCentralSig = file->read(4);
        numOfDisk = file->read(2);
        centralStarts = file->read(2);
        numberOfRecords = file->read(2);
        totalNumberOfRecords = file->read(2);
        sizeOfCentral = file->read(4);
        offsetOfCentral = file->read(4);
        zipComment = file->read(2);
    }
    if (file->seek(qbytearrayToInt32(offsetOfCentral)))
    {
        do{
            filePos = file->pos();
            centralSig = file->read(4);
            versionMade = file->read(2);
            minimumVer = file->read(2);
            generalPurpose = file->read(2);
            compMethod = file->read(2);
            fileModTime = file->read(2);
            fileModDate = file->read(2);
            crc = file->read(4);
            compSize = file->read(4);
            uncompSize = file->read(4);
            fileNameLength = file->read(2);
            extraFieldLength = file->read(2);
            fileCommentLength = file->read(2);
            diskNum = file->read(2);
            inFileAtt = file->read(2);
            extFileAtt = file->read(4);
            fileOffset = file->read(4);
            fileName = file->read(qbytearrayToInt16(fileNameLength));
            extraField = file->read(qbytearrayToInt16(extraFieldLength));
            fileComment = file->read(qbytearrayToInt16(fileCommentLength));
            file->seek(filePos + 46 + qbytearrayToInt16(fileNameLength) + qbytearrayToInt16(extraFieldLength) + qbytearrayToInt16(fileCommentLength));
        } while (fileName != fileToUnpack);
        if (file->seek(qbytearrayToInt32(fileOffset)))
        {

            fSig = file->read(4);
            fVer = file->read(2);
            fGen = file->read(2);
            fCom = file->read(2);
            fModTime = file->read(2);
            fModDate = file->read(2);
            fCrc = file->read(4);
            fComp = file->read(4);
            fUncomp = file->read(4);
            fNameL = file->read(2);
            fExtraL = file->read(2);
            fName = file->read(qbytearrayToInt16(fNameL));
            fExtra = file->read(qbytearrayToInt16(fExtraL));
            fileArray.clear();
            fileArray = file->read(qbytearrayToInt32(compSize));
            if (fileName.contains("/"))
            {
                QDir dir(QDir::currentPath());
                dir.mkpath(outPath);
            }
            QFile fileOut(outPath+outName);
            if (fileOut.open(QIODevice::WriteOnly))
            {
                fileOut.write(fileArray);
                fileOut.close();
            }
//            file->remove();
            file->close();
            delete file;
            return true;
        }
    }
//    file->remove();
    file->close();
    delete file;
    return true;
}

FileWidget::FileWidget(QWidget *parent, SettingsWidget *settings) :
    QWidget(parent),
    ui(new Ui::FileWidget)
{
    this->settings = settings;
    ui->setupUi(this);

    this->rightTableView = new MyTableView;
    this->ui->phoneLayout->addWidget(this->rightTableView);
    this->leftTableView = new MyTableView;
    this->ui->computerLayout->addWidget(this->leftTableView);
    this->phoneModel = new FileTableModel;
//    this->phoneModel->setColoring( true );
    this->phoneLeftModel = new FileTableModel;
    this->computerModel = new FileTableModel;
    this->findModel = new FileTableModel;

    this->phoneSortModel = new FileSortModel;
    this->phoneLeftSortModel = new FileSortModel;
    this->computerSortModel = new FileSortModel;

    this->phoneSortModel->setSourceModel(this->phoneModel);
    this->phoneLeftSortModel->setSourceModel(this->phoneLeftModel);
    this->computerSortModel->setSourceModel(this->computerModel);

    this->phoneSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->phoneLeftSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->computerSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    this->phoneSortModel->setDynamicSortFilter(true);
    this->phoneLeftSortModel->setDynamicSortFilter(true);
    this->computerSortModel->setDynamicSortFilter(true);

    this->phoneSortModel->setFilterKeyColumn(1);
    this->phoneLeftSortModel->setFilterKeyColumn(1);
    this->computerSortModel->setFilterKeyColumn(1);

    this->rightTableView->setModel(this->phoneSortModel);
    this->leftTableView->setModel(this->computerSortModel);


    this->rightTabBar = new QTabBar();
    this->rightTabBar->setTabsClosable(true);
    ui->phoneLayout->addWidget(this->rightTabBar);

    this->leftTabBar = new QTabBar();
    this->leftTabBar->setTabsClosable(true);
    ui->computerLayout->addWidget(this->leftTabBar);

    //ustawienie comboboxa
    QPair<QIcon, QString> para;
    QFileIconProvider *provider = new QFileIconProvider;
    para.second = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    para.first = provider->icon(QFileInfo(para.second));
    this->ui->leftComboBox->addItem(para.first, para.second);

    para.second = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    para.first = provider->icon(QFileInfo(para.second));
    this->ui->leftComboBox->addItem(para.first, para.second);

    para.second = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    para.first = provider->icon(QFileInfo(para.second));
    this->ui->leftComboBox->addItem(para.first, para.second);

    QFileInfoList lista = QDir::drives();
    while(!lista.isEmpty())
    {
        para.second = lista.takeFirst().filePath();
        para.first = provider->icon(QFileInfo(para.second));
        this->ui->leftComboBox->addItem(para.first, para.second);
    }
    delete provider;
    //ustawienia tabelek

//    ui->leftTableWidget->installEventFilter(this);
//    ui->rightTableWidget->installEventFilter(this);

    this->computer=new Computer();

    this->sdk = this->settings->sdkPath;

    this->phone=new Phone(this->sdk,true);
//    connect(this->phone, SIGNAL(signalConnectionChanged(int)), this, SIGNAL(phoneConnectionChanged(int)));
    this->phone->setPath(this->settings->phonePath);
    this->computer->setPath(this->settings->computerPath);

    this->alwaysCloseCopy = this->settings->alwaysCloseCopy;
    this->dialogKopiujShowModal = this->settings->dialogKopiujShowModal;
    this->phone->setHiddenFiles(this->settings->phoneHiddenFiles);
    this->computer->setHiddenFiles(this->settings->computerHiddenFiles);

    this->leftChangeName=false;
    this->rightChangeName=false;

    rightTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    leftTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->leftPushButtonFolderUp->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogToParent));
    ui->rightPushButtonFolderUp->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogToParent));

    this->setLayout(ui->gridLayout);

    this->leftTableView->horizontalHeader()->setVisible(this->settings->showComputerColumnsHeaders);
    this->rightTableView->horizontalHeader()->setVisible(this->settings->showPhoneColumnsHeaders);

    this->phoneLeft=new Phone(sdk,false);
    this->phoneLeft->setPath(this->settings->phonePath);
    this->phoneLeft->setHiddenFiles(this->settings->phoneHiddenFiles);

    this->computerMenu = NULL;
    this->phoneLeftMenu = NULL;
    this->phoneRightMenu = NULL;

    this->dialog = NULL;
    this->appInfoDialog = NULL;
    this->appsDialog = NULL;

    if (phone->getPath().isEmpty())
        phone->setPath("/");
    if (phoneLeft->getPath().isEmpty())
        phoneLeft->setPath("/");
    if (computer->getPath().isEmpty())
        computer->setPath(QDir::drives().first().filePath());

    ui->leftPushButtonPhoneComputerSwitch->setIcon(QIcon(":/icons/phone.png"));
    qRegisterMetaType<File>("File");
    connect(&this->threadFind, SIGNAL(foundFile(File)), this, SLOT(foundFile(File)));
    connect(&this->threadFind, SIGNAL(finished()), this, SLOT(findFinished()));

    connect(ui->leftComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(leftComboBox()));
    connect(ui->leftComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(leftComboBoxScroll()));

    connect(ui->rightComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(rightComboBox()));
    connect(ui->rightComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(rightComboBox()));

    connect(rightTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightContextMenu(const QPoint &)));
    connect(leftTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(leftContextMenu(const QPoint &)));

    connect(this->rightTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rightDoubleClick()));
    connect(this->leftTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(leftDoubleClick()));

    connect(this->ui->leftFileNameFilter, SIGNAL(textChanged(QString)), this, SLOT(setLeftFilter(QString)));
    connect(this->ui->rightFileNameFilter, SIGNAL(textChanged(QString)), this, SLOT(setRightFilter(QString)));

    connect(this->leftTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(leftSelectedCount()));
    connect(this->rightTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(rightSelectedCount()));

    connect(this->phone,SIGNAL(signalConnectionChanged(int)),this,SLOT(connectionChanged()));


    connect(this->rightTabBar, SIGNAL(currentChanged(int)),this,SLOT(rightTabBarPageChanged()));
    connect(this->rightTabBar, SIGNAL(tabCloseRequested(int)),this,SLOT(rightTabBarCloseTab(int)));

    connect(this->leftTabBar, SIGNAL(currentChanged(int)),this,SLOT(leftTabBarPageChanged()));
    connect(this->leftTabBar, SIGNAL(tabCloseRequested(int)),this,SLOT(leftTabBarCloseTab(int)));

    connect(this->computerModel, SIGNAL(copy(QStringList)), this, SLOT(copySlotToComputer(QStringList)), Qt::QueuedConnection);
    connect(this->phoneModel, SIGNAL(copy(QStringList)), this, SLOT(copySlotToPhone(QStringList)), Qt::QueuedConnection);
    connect(this->phoneLeftModel, SIGNAL(copy(QStringList)), this, SLOT(copySlotToPhoneLeft(QStringList)), Qt::QueuedConnection);

    this->leftMode="computer";
    this->leftDisplay();
    this->settings->changeFont();
}

FileWidget::~FileWidget()
{
    this->settings->phonePath = this->phone->getPath();
    this->settings->computerPath = this->computer->getPath();

    delete this->phone;
    delete this->phoneLeft;
    delete this->computer;
    if (this->computerMenu != NULL)
        delete this->computerMenu;
    if (this->dialog != NULL)
        delete this->dialog;
    if (this->phoneLeftMenu != NULL)
        delete this->phoneLeftMenu;
    if (this->phoneRightMenu != NULL)
        delete this->phoneRightMenu;
    if (this->appInfoDialog != NULL)
        delete this->appInfoDialog;
    if (this->appsDialog != NULL)
        delete this->appsDialog;

    delete ui;
}

void FileWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        {
            ui->retranslateUi(this);

            if (this->computerMenu != NULL)
            {
                QList<QAction *> actions = this->computerMenu->actions();
                foreach(QAction *action, actions)
                {
                    action->setText(tr(action->data().toString().toAscii()));
                }
            }
            if (this->phoneLeftMenu != NULL)
            {
                QList<QAction *> actions = this->phoneLeftMenu->actions();
                foreach(QAction *action, actions)
                {
                    action->setText(tr(action->data().toString().toAscii()));
                }
            }
            if (this->phoneRightMenu != NULL)
            {
                QList<QAction *> actions = this->phoneRightMenu->actions();
                foreach(QAction *action, actions)
                {
                    action->setText(tr(action->data().toString().toAscii()));
                }
            }
        }
        break;
    default:
        break;
    }
}

void FileWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Return:
        if (leftTableView->hasFocus())
            leftDoubleClick();
        else if (rightTableView->hasFocus())
            rightDoubleClick();
        break;
    case Qt::Key_Escape:
        if (leftTableView->hasFocus())
            on_leftPushButtonFolderUp_pressed();
        else if (rightTableView->hasFocus())
            on_rightPushButtonFolderUp_pressed();
        break;

    case Qt::Key_F2:
        on_pushButtonRename_pressed();
        break;
    case Qt::Key_F3:
        on_pushButtonSelectAll_pressed();
        break;
    case Qt::Key_F4:
        on_pushButton_pressed();
        break;
    case Qt::Key_F5:
        on_pushButtonCopy_pressed();
        break;
    case Qt::Key_F6:
        on_pushButtonNewDir_pressed();
        break;
    case Qt::Key_F7:
        on_pushButtonRefresh_pressed();
        break;
    case Qt::Key_F8:
        on_pushButtonDelete_pressed();
        break;
    case Qt::Key_Delete:
        on_pushButtonDelete_pressed();
        break;
    case Qt::Key_F9:
        on_pushButtonHiddenFiles_pressed();
        break;
    }
}

void FileWidget::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    int widthTab = this->rightTableView->width();
    int colIcon = 25;
    int colName, colDate, colSize;

    this->rightTableView->resizeColumnsToContents();
    colSize = this->rightTableView->columnWidth(2);
    colDate = this->rightTableView->columnWidth(3);
    if (this->rightTableView->verticalScrollBar() == NULL || !this->rightTableView->verticalScrollBar()->isVisible())
        colName = widthTab - colIcon - colSize - colDate;
    else
        colName = widthTab - colIcon - colSize - colDate - this->rightTableView->verticalScrollBar()->width();


    this->rightTableView->setColumnWidth(0, colIcon);
    this->rightTableView->setColumnWidth(1, colName - 2);
    this->rightTableView->setColumnWidth(2, colSize);
    this->rightTableView->setColumnWidth(3, colDate);

    widthTab = this->leftTableView->width();
    colIcon = 25;

    this->leftTableView->resizeColumnsToContents();
    colSize = this->leftTableView->columnWidth(2);
    colDate = this->leftTableView->columnWidth(3);
    if (this->leftTableView->verticalScrollBar() == NULL || !this->leftTableView->verticalScrollBar()->isVisible())
        colName = widthTab - colIcon - colSize - colDate;
    else
        colName = widthTab - colIcon - colSize - colDate - this->leftTableView->verticalScrollBar()->width();


    this->leftTableView->setColumnWidth(0, colIcon);
    this->leftTableView->setColumnWidth(1, colName - 2);
    this->leftTableView->setColumnWidth(2, colSize);
    this->leftTableView->setColumnWidth(3, colDate);
}

void FileWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        emit customContextMenuRequested(event->pos());
}

void FileWidget::connectionChanged()
{
    if (this->phone->getConnectionState() == DISCONNECTED)
    {
        emit this->phoneConnectionChanged(DISCONNECTED);
        this->phoneLeft->setConnectionState(this->phone->getConnectionState());
        this->leftMode="computer";
        ui->leftPushButtonPhoneComputerSwitch->setIcon(QIcon(":/icons/phone.png"));
        this->leftDisplay();
    }
    else if (this->phone->getConnectionState() == DEVICE)
    {
        emit this->phoneConnectionChanged(DEVICE);
        this->phoneLeft->setConnectionState(this->phone->getConnectionState());
        if (this->leftMode=="phone")
            this->leftDisplay();
        this->rightDisplay();
    }
    else if (this->phone->getConnectionState() == RECOVERY)
    {
        emit this->phoneConnectionChanged(RECOVERY);
        this->phoneLeft->setConnectionState(this->phone->getConnectionState());
        this->leftMode="computer";
        ui->leftPushButtonPhoneComputerSwitch->setIcon(QIcon(":/icons/phone.png"));
        this->leftDisplay();
    }
    else if (this->phone->getConnectionState() == FASTBOOT)
    {
        emit this->phoneConnectionChanged(FASTBOOT);
        this->phoneLeft->setConnectionState(this->phone->getConnectionState());
    }
    return;
}

void FileWidget::computerContextMenu(const QPoint &pos,QTableView *tableView)
{
    if (this->computerMenu == NULL)
    {
        QSettings settings;
        this->computerMenu = new QMenu;
        int layoutDirection = settings.value("layoutDirection",0).toInt();

        if (layoutDirection == 0)
            this->computerMenu->setLayoutDirection(Qt::LeftToRight);
        if (layoutDirection == 1)
            this->computerMenu->setLayoutDirection(Qt::RightToLeft);
        QAction *usun,*selectAll,*selectNone,*odswiez,*nowyFolder,*zmienNazwe,*ukryte,*install, *copy, *openInNewTab;
        zmienNazwe = this->computerMenu->addAction(QIcon(":icons/rename.png"),tr("rename", "computer right click menu"),this,SLOT(computerRename()));
        zmienNazwe->setData(QString("rename"));
        selectAll = this->computerMenu->addAction(QIcon(":icons/selectAll.png"),tr("select all", "computer right click menu"),this,SLOT(leftSelectAll()));
        selectAll->setData(QString("select all"));
        selectNone = this->computerMenu->addAction(QIcon(":icons/selectNone.png"),tr("select none", "computer right click menu"),this->leftTableView,SLOT(clearSelection()));
        selectNone->setData(QString("select none"));
        copy = this->computerMenu->addAction(QIcon(":icons/copy.png"),tr("copy", "computer right click menu"), this, SLOT(computerCopy()));
        copy->setData(QString("copy"));
        nowyFolder = this->computerMenu->addAction(QIcon(":icons/newDir.png"),tr("new dir", "computer right click menu"),this,SLOT(computerNewDir()));
        nowyFolder->setData(QString("new dir"));
        odswiez = this->computerMenu->addAction(QIcon(":icons/refresh.png"),tr("refresh", "computer right click menu"),this,SLOT(leftRefresh()));
        odswiez->setData(QString("refresh"));
        usun = this->computerMenu->addAction(QIcon(":icons/remove.png"),tr("delete", "computer right click menu"),this,SLOT(computerDelete()));
        usun->setData(QString("delete"));
        ukryte = this->computerMenu->addAction(QIcon(":icons/hidden.png"),tr("hidden files", "computer right click menu"),this,SLOT(computerHiddenFiles()));
        ukryte->setData(QString("hidden files"));
        install = this->computerMenu->addAction(QIcon(":icons/install.png"),tr("install", "computer right click menu"),this,SLOT(installAppFromComputer()));
        install->setData(QString("install"));
        this->actionAppInfo = this->computerMenu->addAction(QIcon(":icons/info.png"), tr("Show app info", "computer right click menu"), this, SLOT(showAppInfo()));
        this->actionAppInfo->setData(QString("show app info"));
        openInNewTab = this->computerMenu->addAction(QApplication::style()->standardIcon(QStyle::SP_DirIcon),tr("open in new tab", "computer right click menu"),this,SLOT(computerOpenInNewTab()));
        openInNewTab->setData(QString("open in new tab"));

    }
    QPoint pos2;
    pos2.setX(pos.x());
    pos2.setY(pos.y()+20);


    if (!this->leftTableView->selectionModel()->selection().isEmpty())
    {
        QModelIndex index = this->leftTableView->selectionModel()->selection().indexes().takeFirst();
        index = this->computerSortModel->mapToSource(index);
        QString fileName = this->computerModel->getFile(index.row()).filePath;

        if (fileName.contains(QRegExp(".apk$")))
        {
            this->actionAppInfo->setVisible(true);
        }
        else
        {
            this->actionAppInfo->setVisible(false);
        }
    }
    else
        this->actionAppInfo->setDisabled(true);

    this->computerMenu->exec(tableView->mapToGlobal(pos2));
}

void FileWidget::computerCopy()
{
    if (this->leftTableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this,"",tr("select some files first"),QMessageBox::Ok);
        return;
    }

    QSettings settings;
    if (settings.value("showCopyConfirmation", true).toBool())
    {
	if (QMessageBox::question(this,tr("copying"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
	    return;
    }

    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<File> *filesToCopy = new QList<File>;
    while (!indexList.isEmpty())
    {
        index = this->computerSortModel->mapToSource(indexList.takeFirst());
        filesToCopy->append(this->computerModel->getFile(index.row()));
    }
    filesToCopy = this->computerFilesToCopy(filesToCopy);
    if (filesToCopy->isEmpty())
    {
        QMessageBox::information(this,"",tr("theres nothing to copy. Maybe selected dirs are empty"),QMessageBox::Ok);
        return;
    }

    if (this->dialog != NULL)
        delete this->dialog;
    this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::ComputerToPhone, this->computer->getPath(), this->phone->getPath());

    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();

    connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(leftRefresh()));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(rightRefresh()));
}

void FileWidget::computerDelete()
{
    if (this->leftTableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this,"",tr("select some files first"),QMessageBox::Ok);
        return;
    }

    if (QMessageBox::question(this, tr("deleting"), tr("are you sure???"),
                              QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows();
    QModelIndex index;
    while (!indexList.isEmpty())
    {
        index = this->computerSortModel->mapToSource(indexList.takeFirst());
        this->computer->remove(this->computerModel->getFile(index.row()));
    }
    this->leftDisplay();
}

void FileWidget::computerDisplay(QTableWidget *tableWidget)
{
    QSettings settings;
    this->computerModel->clear();
    this->ui->leftLabelSelectedCount->setText(tr("getting files..."));
    this->leftTableView->setDisabled(true);
    this->ui->leftComboBox->setEditText(this->computer->getPath());
    QList<File> *fileList = this->computer->getFileList();
    File tmpFile;
    App *app = NULL;

    while (!fileList->isEmpty())
    {
//        qApp->processEvents();
        tmpFile = fileList->takeFirst();
        if (tmpFile.fileName.contains(QRegExp(".apk$")))
        {
            if (this->settings->showAppName || this->settings->showAppIcon)
            {
                app = this->getAppInfo(tmpFile.filePath);
                if (this->settings->showAppIcon)
                    tmpFile.fileIcon = app->appIcon;
                QString appName = this->settings->showAppNameConfig;
                appName.replace("<appName>", app->appName);
                QString tmp=app->appVersion;
                if (tmp.length()>15)
                    tmp=tmp.left(15)+"...";

                appName.replace("<appVersion>", tmp);
                appName.replace("<packageName>", app->packageName);
                if (this->settings->showAppName)
                    tmpFile.fileName = appName;
                delete app;
                app = NULL;
            }
        }
        this->computerModel->insertFile(0, tmpFile);
    }
    delete fileList;
    this->leftTableView->resizeColumnsToContents();
    this->leftTableView->resizeRowsToContents();
    int i=0;
    int rows = this->computerModel->rowCount();
    for (i=0; i<rows; i++)
        this->leftTableView->setRowHeight(i, 20);
    this->leftTableView->sortByColumn(1, Qt::AscendingOrder);
    this->leftTableView->setDisabled(false);
    QStringList computerColumnList = settings.value("computerColumnList").toStringList();
    int col = this->computerModel->columnCount(QModelIndex());
    for (i = 0; i < col; i++)
    {
        if (!computerColumnList.contains(QString::number(i)) && !computerColumnList.isEmpty())
            this->leftTableView->hideColumn(i);
        else
            this->leftTableView->showColumn(i);
    }
    this->leftSelectedCount();
}

QList<File> * FileWidget::computerFilesToCopy(QList<File> *fileList)
{
    int x = fileList->size();
    QList<File> *tmpFiles = NULL;
    for (int i=0; i<x; i++)
    {
        if (fileList->at(i).fileType == "dir")
        {
            this->computer->cd(fileList->takeAt(i).fileName);
            tmpFiles = this->computerFilesToCopy(this->computer->getFileList());
            while (!tmpFiles->isEmpty())
            {
                fileList->append(tmpFiles->takeFirst());
            }
            delete tmpFiles;
            tmpFiles = NULL;
            this->computer->cdUp();
            i--;
            x--;
        }
    }
    return fileList;
}

void FileWidget::computerHiddenFiles()
{
    bool hidden;
    hidden = this->computer->getHiddenFilesState();
    this->computer->setHiddenFiles(!hidden);
    this->settings->computerHiddenFiles = !hidden;
    QSettings settings;
    settings.setValue("computerHiddenFiles",!hidden);
//    hidden = this->computer->getHiddenFilesState();
//    computer->setHiddenFiles(!computer->getHiddenFilesState());
    this->leftDisplay();
}

void FileWidget::computerNewDir()
{
    if (!computer->makeDir("new dir"))
    {
        QMessageBox::information(this,tr("error"),
                                 tr("dir was not created.\nMake sure that you are allowed to do this"),
                                 QMessageBox::Ok);
        return;
    }
    this->leftDisplay();

    int row = this->computerModel->getRow("new dir");
    QModelIndex index = this->computerModel->index(row, 1, QModelIndex());
    this->leftTableView->selectRow(this->computerSortModel->mapFromSource(index).row());
    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = indexList.first();
        this->leftTableView->edit(index);
        connect (this->computerModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(computerRenameSlot(QModelIndex, QModelIndex)));
    }
}

void FileWidget::computerOpenInNewTab()
{
    QModelIndexList indexList = leftTableView->selectionModel()->selectedRows();
    QModelIndex index;
    QString lastPath,tmp;
    while (!indexList.isEmpty())
    {
        index = this->computerSortModel->mapToSource(indexList.takeFirst());
        lastPath = computer->getPath();
        tmp = computer->getPath();
        if (computer->cd(this->computerModel->getFile(index.row()).fileName))
        {
            tmp = computer->getPath();
            if (this->leftTabBar->count() == 0)
            {
                this->leftTabBar->addTab(lastPath);
                this->leftTabBar->setTabToolTip(0,lastPath);
            }
            this->leftTabBar->addTab(tmp);
            this->leftTabBar->setTabToolTip(this->leftTabBar->count()-1,tmp);
            this->leftTabBar->setCurrentIndex(this->leftTabBar->count()-1);
            this->leftDisplay();
        }
    }

//    QString tmp = phone->getPath()+fileModel->getFile(index.row()).fileName;
}

void FileWidget::computerRename()
{
    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = indexList.first();
        this->leftTableView->edit(index);
        connect (this->computerModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(computerRenameSlot(QModelIndex, QModelIndex)));
    }
}

void FileWidget::computerRenameSlot(QModelIndex indexS, QModelIndex indexE)
{
    disconnect (this->computerModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this, SLOT(computerRenameSlot(QModelIndex, QModelIndex)));

    if (indexS == indexE)
    {
        File tmpFile = this->computerModel->getFile(indexS.row());
        QString oldFile = tmpFile.filePath;
        oldFile.remove(this->computer->getPath());
        this->computer->rename(oldFile, indexS.data(Qt::DisplayRole).toString());
        this->leftDisplay();
    }
}

void FileWidget::leftComboBox()
{
    if (this->leftMode=="computer")
    {
        if (this->computer->cd(ui->leftComboBox->lineEdit()->text()))
            this->leftDisplay();
    }
    else
    {
        QString path=this->phoneLeft->getPath();
        this->phoneLeft->cd(ui->leftComboBox->lineEdit()->text());
        if (this->phoneLeft->cd("."))
            this->leftDisplay();
        else
            this->phoneLeft->setPath(path);
    }
}

void FileWidget::leftComboBoxScroll()
{
    this->leftMode=="computer";
    computer->setPath(ui->leftComboBox->lineEdit()->text());
    this->leftDisplay();
}

void FileWidget::leftContextMenu(const QPoint &pos)
{
    if (this->leftMode=="computer")
        this->computerContextMenu(pos,leftTableView);
    else
        this->phoneContextMenu(pos,leftTableView);
}

void FileWidget::leftDisplay()
{
    if (this->leftMode=="computer")
    {
        ui->leftFileNameFilter->clear();
        this->computer->setHiddenFiles(this->settings->computerHiddenFiles);
        this->leftTableView->horizontalHeader()->setVisible(this->settings->showComputerColumnsHeaders);
        this->computerDisplay(NULL);

        if (this->leftTabBar->count()>0)
        {
            this->leftTabBar->setTabText(this->leftTabBar->currentIndex(),this->computer->getPath());
            this->leftTabBar->setTabToolTip(this->leftTabBar->currentIndex(),this->computer->getPath());
            this->leftTabBar->setMaximumWidth(leftTableView->width());
        }
    }
    else
    {
        this->phoneLeftModel->setColoring(this->settings->color);
        this->phoneLeft->setHiddenFiles(this->settings->phoneHiddenFiles);
        this->leftTableView->horizontalHeader()->setVisible(this->settings->showPhoneColumnsHeaders);
        this->phoneDisplay(this->leftTableView);
    }
    this->leftTableView->setFocus();
    int widthTab = this->leftTableView->width();
    int colIcon = 25;
    int colName, colDate, colSize;

    this->leftTableView->resizeColumnsToContents();
    colSize = this->leftTableView->columnWidth(2);
    colDate = this->leftTableView->columnWidth(3);
    if (this->leftTableView->verticalScrollBar() == NULL)
        colName = widthTab - colIcon - colSize - colDate;
    else
        colName = widthTab - colIcon - colSize - colDate - this->leftTableView->verticalScrollBar()->width();


    this->leftTableView->setColumnWidth(0, colIcon);
    this->leftTableView->setColumnWidth(1, colName - 2);
    this->leftTableView->setColumnWidth(2, colSize);
    this->leftTableView->setColumnWidth(3, colDate);
}

void FileWidget::leftDoubleClick()
{
    if (!this->leftTableView->selectionModel()->selection().isEmpty())
    {
        if (this->leftMode=="computer")
        {
            QModelIndex index = this->leftTableView->selectionModel()->selection().indexes().takeFirst();
            index = this->computerSortModel->mapToSource(index);
            File file = this->computerModel->getFile(index.row());

            if (this->computer->cd(file.fileName))
            {
                this->leftDisplay();
            }
            else
            {
                if (file.filePath.endsWith(".apk"))
                {
                    App *app;
                    app = this->getAppInfo(file.filePath);
                    if (this->appInfoDialog != NULL)
                        delete this->appInfoDialog;
                    this->appInfoDialog = new appInfo(this, app);
                    this->appInfoDialog->show();
                }
                else
                {
                    QDesktopServices::openUrl(QUrl("file:///" + file.filePath));
                }
            }
        }
        else
        {
            QModelIndex index = this->leftTableView->selectionModel()->selection().indexes().takeFirst();
            index = this->phoneLeftSortModel->mapToSource(index);
            QString fileName = this->phoneLeftModel->getFile(index.row()).fileName;

            if (this->phoneLeft->cd(fileName))
            {
                this->leftDisplay();
            }
        }
    }
}

void FileWidget::leftRefresh()
{
    this->leftDisplay();
}

void FileWidget::leftSelectAll()
{
    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == leftTableView->model()->rowCount())
        this->leftTableView->clearSelection();
    else
        this->leftTableView->selectAll();
}


void FileWidget::leftSelectedCount()
{
    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows(1);
    QString word(QString::number(indexList.size()) + "/" +
                 QString::number(this->computerModel->rowCount())+ " " + tr("selected", "selected label below table"));
    this->ui->leftLabelSelectedCount->setText(word);
}

void FileWidget::leftTableWidgetActivated()
{
//    QPalette paleta=ui->leftTableWidget->palette();
//    paleta.setColor(QPalette::Highlight,QColor(204,204,204));
//    ui->leftTableWidget->setPalette(paleta);

//    paleta=rightTableView->palette();
//    paleta.setColor(QPalette::Highlight,QColor(153,197,255));
//    rightTableView->setPalette(paleta);
}

void FileWidget::leftTabBarPageChanged()
{
    if (this->computer->cd(this->leftTabBar->tabText(this->leftTabBar->currentIndex())))
        this->leftDisplay();
}

void FileWidget::leftTabBarCloseTab(int index)
{
    if (this->leftTabBar->currentIndex() == index)
    {
        if (this->leftTabBar->count()>0)
        {
            this->leftTabBar->removeTab(index);
            this->leftTabBar->setCurrentIndex(0);
        }
    }
    else
        this->leftTabBar->removeTab(index);

    if (this->leftTabBar->count() == 1)
        this->leftTabBar->removeTab(0);
}

void FileWidget::on_leftPushButtonFolderUp_pressed()
{
    if (this->leftMode == "computer")
    {
        if (this->computer->cdUp())
        {
            this->leftDisplay();
        }
    }
    else
    {
        if (this->phoneLeft->cdUp())
        {
            this->leftDisplay();
        }
    }
}

void FileWidget::on_leftPushButtonPhoneComputerSwitch_pressed()
{
    if ((this->phoneLeft->getConnectionState() == RECOVERY) || (this->phoneLeft->getConnectionState() == DEVICE))
    {
        if (this->leftMode=="computer")
        {
            this->leftMode="phone";
            ui->leftPushButtonPhoneComputerSwitch->setIcon(QIcon(":/icons/computer.png"));
            this->leftTableView->setModel(this->phoneLeftSortModel);
            this->leftDisplay();
        }
        else
        {
            this->leftMode="computer";
            ui->leftPushButtonPhoneComputerSwitch->setIcon(QIcon(":/icons/phone.png"));
            this->leftTableView->setModel(this->computerSortModel);
            this->leftDisplay();
        }
    }
}

void FileWidget::on_pushButtonCopy_pressed()
{
    ui->pushButtonCopy->setDown(false);
    if (leftTableView->hasFocus())
    {
        if (this->leftMode=="computer")
            this->computerCopy();
        else
            this->phoneCopy();
    }
    else
        this->phoneCopy();
}

void FileWidget::on_pushButtonDelete_pressed()
{
    ui->pushButtonDelete->setDown(false);
    if (leftTableView->hasFocus())
    {
        if (this->leftMode=="computer")
            this->computerDelete();
        else
            this->phoneDelete();
    }
    else
        this->phoneDelete();
}

void FileWidget::on_pushButtonHiddenFiles_pressed()
{
    if (leftTableView->hasFocus())
    {
        if (this->leftMode=="computer")
            this->computerHiddenFiles();
        else
            this->phoneHiddenFiles();
    }
    else
        this->phoneHiddenFiles();
}

void FileWidget::on_pushButtonNewDir_pressed()
{
    if (leftTableView->hasFocus())
    {
        if (this->leftMode=="computer")
            this->computerNewDir();
        else
            this->phoneNewDir();
    }
    else
        this->phoneNewDir();
}

void FileWidget::on_pushButtonRefresh_pressed()
{
    if (leftTableView->hasFocus())
        this->leftRefresh();
    else
        this->rightRefresh();
}

void FileWidget::on_pushButtonRename_pressed()
{
    if (leftTableView->hasFocus())
    {
        if (this->leftMode=="computer")
            this->computerRename();
        else
            this->phoneRename();
    }
    else
        this->phoneRename();
}

void FileWidget::on_pushButtonSelectAll_pressed()
{
    if (this->leftTableView->hasFocus())
        this->leftSelectAll();
    else
        this->rightSelectAll();
}

void FileWidget::on_rightPushButtonFolderUp_pressed()
{
    if (this->phone->cdUp())
    {
        this->rightDisplay();
    }
}

void FileWidget::phoneContextMenu(const QPoint &pos,QTableView *tableView)
{
    if (this->phoneLeftMenu == NULL)
    {
        this->phoneLeftMenu = new QMenu;
        QSettings settings;
        int layoutDirection = settings.value("layoutDirection",0).toInt();

        if (layoutDirection == 0)
            this->phoneLeftMenu->setLayoutDirection(Qt::LeftToRight);
        if (layoutDirection == 1)
            this->phoneLeftMenu->setLayoutDirection(Qt::RightToLeft);
        QAction *usun,*selectAll,*selectNone,*odswiez,*nowyFolder,*zmienNazwe,*ukryte, *copy/*, *install*/;
        zmienNazwe = this->phoneLeftMenu->addAction(QIcon(":icons/rename.png"),tr("rename", "phone right click menu"),this,SLOT(phoneRename()));
        zmienNazwe->setData(QString("rename"));
        selectAll = this->phoneLeftMenu->addAction(QIcon(":icons/selectAll.png"),tr("select all", "phone right click menu"),this,SLOT(leftSelectAll()));
        selectAll->setData(QString("select all"));
        selectNone = this->phoneLeftMenu->addAction(QIcon(":icons/selectNone.png"),tr("select none", "phone right click menu"),this->leftTableView,SLOT(clearSelection()));
        selectNone->setData(QString("select none"));
        copy = this->phoneLeftMenu->addAction(QIcon(":icons/copy.png"),tr("copy", "phone right click menu"), this, SLOT(phoneCopy()));
        copy->setData(QString("copy"));
        nowyFolder = this->phoneLeftMenu->addAction(QIcon(":icons/newDir.png"),tr("new dir", "phone right click menu"),this,SLOT(phoneNewDir()));
        nowyFolder->setData(QString("new dir"));
        odswiez = this->phoneLeftMenu->addAction(QIcon(":icons/refresh.png"),tr("refresh", "phone right click menu"),this,SLOT(leftRefresh()));
        odswiez->setData(QString("refresh"));
        usun = this->phoneLeftMenu->addAction(QIcon(":icons/remove.png"),tr("delete", "phone right click menu"),this,SLOT(phoneDelete()));
        usun->setData(QString("delete"));
        ukryte = this->phoneLeftMenu->addAction(QIcon(":icons/hidden.png"),tr("hidden files", "phone right click menu"),this,SLOT(phoneHiddenFiles()));
        ukryte->setData(QString("hidden files"));

        //    install = menu->addAction(QIcon(":icons/install.png"),tr("install"),this,SLOT(installAppFromPhone()));
    }

    if (this->phoneRightMenu == NULL)
    {
        this->phoneRightMenu = new QMenu;
        QSettings settings;
        int layoutDirection = settings.value("layoutDirection",0).toInt();

        if (layoutDirection == 0)
            this->phoneRightMenu->setLayoutDirection(Qt::LeftToRight);
        if (layoutDirection == 1)
            this->phoneRightMenu->setLayoutDirection(Qt::RightToLeft);
        QAction *usun,*selectAll,*selectNone,*odswiez,*nowyFolder,*zmienNazwe,*ukryte, *copy, *openInNewTab/*, *install*/;

        zmienNazwe = this->phoneRightMenu->addAction(QIcon(":icons/rename.png"),tr("rename", "phone right click menu"),this,SLOT(phoneRename()));
        zmienNazwe->setData(QString("rename"));
        selectAll = this->phoneRightMenu->addAction(QIcon(":icons/selectAll.png"),tr("select all", "phone right click menu"),this,SLOT(rightSelectAll()));
        selectAll->setData(QString("select all"));
        selectNone = this->phoneRightMenu->addAction(QIcon(":icons/selectNone.png"),tr("select none", "phone right click menu"),this->rightTableView,SLOT(clearSelection()));
        selectNone->setData(QString("select none"));
        copy = this->phoneRightMenu->addAction(QIcon(":icons/copy.png"),tr("copy", "phone right click menu"), this, SLOT(phoneCopy()));//
        copy->setData(QString("copy"));
        nowyFolder = this->phoneRightMenu->addAction(QIcon(":icons/newDir.png"),tr("new dir", "phone right click menu"),this,SLOT(phoneNewDir()));
        nowyFolder->setData(QString("new dir"));
        odswiez = this->phoneRightMenu->addAction(QIcon(":icons/refresh.png"),tr("refresh", "phone right click menu"),this,SLOT(rightRefresh()));//
        odswiez->setData(QString("refresh"));
        usun = this->phoneRightMenu->addAction(QIcon(":icons/remove.png"),tr("delete", "phone right click menu"),this,SLOT(phoneDelete()));//
        usun->setData(QString("delete"));
        ukryte = this->phoneRightMenu->addAction(QIcon(":icons/hidden.png"),tr("hidden files", "phone right click menu"),this,SLOT(phoneHiddenFiles()));
        ukryte->setData(QString("hidden files"));
        openInNewTab = this->phoneRightMenu->addAction(QApplication::style()->standardIcon(QStyle::SP_DirIcon),tr("open in new tab", "phone right click menu"),this,SLOT(phoneOpenInNewTab()));
        openInNewTab->setData(QString("open in new tab"));
    }
    if (rightTableView->model() == this->findModel)
    {
        this->phoneRightMenu->actions().at(0)->setVisible(false);
        this->phoneRightMenu->actions().at(1)->setVisible(false);
        this->phoneRightMenu->actions().at(2)->setVisible(false);
        this->phoneRightMenu->actions().at(4)->setVisible(false);
        this->phoneRightMenu->actions().at(7)->setVisible(false);
        this->phoneRightMenu->actions().at(8)->setVisible(false);

        this->phoneRightMenu->actions().at(3)->setVisible(false);//copy
        this->phoneRightMenu->actions().at(6)->setVisible(false);//delete

    }
    else if (rightTableView->model() == this->phoneSortModel)
    {
        this->phoneRightMenu->actions().at(0)->setVisible(true);
        this->phoneRightMenu->actions().at(1)->setVisible(true);
        this->phoneRightMenu->actions().at(2)->setVisible(true);
        this->phoneRightMenu->actions().at(4)->setVisible(true);
        this->phoneRightMenu->actions().at(7)->setVisible(true);
        this->phoneRightMenu->actions().at(8)->setVisible(true);

        this->phoneRightMenu->actions().at(3)->setVisible(true);//copy
        this->phoneRightMenu->actions().at(6)->setVisible(true);//delete
    }

    QPoint pos2;
    pos2.setX(pos.x());
    pos2.setY(pos.y()+20);
    if (tableView == leftTableView)
        this->phoneLeftMenu->exec(tableView->mapToGlobal(pos2));
    else
        this->phoneRightMenu->exec(tableView->mapToGlobal(pos2));
}

void FileWidget::phoneCopy()
{
    QTableView * tableView;
    Phone *phoneTmp, *phoneTmp2;
    FileSortModel * sortModel;
    FileTableModel * fileModel;

    if (this->leftTableView->hasFocus())
    {
        phoneTmp = this->phoneLeft;
        phoneTmp2 = this->phone;
        tableView = this->leftTableView;
        sortModel = this->phoneLeftSortModel;
        fileModel = this->phoneLeftModel;
    }
    else
    {
        phoneTmp = this->phone;
        phoneTmp2 = this->phoneLeft;
        tableView = this->rightTableView;
        sortModel = this->phoneSortModel;
        fileModel = this->phoneModel;
    }

    if (tableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this,"",tr("select some files first"),QMessageBox::Ok);
        return;
    }
    QSettings settings;
    if (settings.value("showCopyConfirmation", true).toBool())
    {
	if (QMessageBox::question(this,tr("copying"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
	    return;
    }

    QModelIndexList indexList = tableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<File> *filesToCopy = new QList<File>;
    while (!indexList.isEmpty())
    {
        index = sortModel->mapToSource(indexList.takeFirst());
        filesToCopy->append(fileModel->getFile(index.row()));
    }
    filesToCopy = this->phoneFilesToCopy(filesToCopy, phoneTmp);
    if (filesToCopy->isEmpty())
    {
        QMessageBox::information(this,"",tr("theres nothing to copy. Maybe selected dirs are empty"),QMessageBox::Ok);
        return;
    }

    if (this->dialog != NULL)
        delete this->dialog;
    if (this->leftMode == "computer")
        this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::PhoneToComputer,
                                        phoneTmp->getPath(), this->computer->getPath());
    else
        this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::PhoneToPhone,
                                        phoneTmp->getPath(), phoneTmp2->getPath());

    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();

    connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));

    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(leftRefresh()));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(rightRefresh()));
}

void FileWidget::phoneDelete()
{
    QTableView * tableView;
    Phone * phoneTmp;
    FileSortModel * sortModel;
    FileTableModel * fileModel;
    if (this->leftTableView->hasFocus())
    {
        phoneTmp = this->phoneLeft;
        tableView = this->leftTableView;
        sortModel = this->phoneLeftSortModel;
        fileModel = this->phoneLeftModel;
    }
    else
    {
        phoneTmp = this->phone;
        tableView = this->rightTableView;
        sortModel = this->phoneSortModel;
        fileModel = this->phoneModel;
    }

    if (tableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this,"",tr("select some files first"),QMessageBox::Ok);
        return;
    }

    if (QMessageBox::question(this, tr("deleting"), tr("are you sure???"),
                              QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    QModelIndexList indexList = tableView->selectionModel()->selectedRows();
    QModelIndex index;
    while (!indexList.isEmpty())
    {
        index = sortModel->mapToSource(indexList.takeFirst());
        phoneTmp->remove(fileModel->getFile(index.row()).fileName);
    }
    if (this->leftTableView->hasFocus())
    {
        this->leftDisplay();
    }
    else
    {
        this->rightDisplay();
    }
}

void FileWidget::phoneDisplay(QTableView *tableView)
{
    QSettings settings;
    FileTableModel * phoneTmpModel;
    Phone * phoneTmp;
    QComboBox *tmpCombo;
    if (tableView == this->leftTableView)
    {
        phoneTmpModel = this->phoneLeftModel;
        tableView->setModel(this->phoneLeftSortModel);
        phoneTmp = this->phoneLeft;
        tmpCombo = this->ui->leftComboBox;
        this->ui->leftLabelSelectedCount->setText(tr("getting files..."));

    }
    else
    {
        phoneTmpModel = this->phoneModel;
        tableView->setModel(this->phoneSortModel);
        phoneTmp = this->phone;
        tmpCombo = this->ui->rightComboBox;
        this->ui->rightLabelSelectedCount->setText(tr("getting files..."));
    }
    tableView->setDisabled(true);
    phoneTmpModel->clear();

    tmpCombo->setEditText(phoneTmp->getPath());
    QList<File> *fileList = NULL;
    fileList = phoneTmp->getFileList();
    if (fileList == NULL)
        return;
    while (!fileList->isEmpty())
    {
//        qApp->processEvents();
        phoneTmpModel->insertFile(0, fileList->takeFirst());
    }

    delete fileList;
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
    int i=0;
    int rows = phoneTmpModel->rowCount();
    for (i=0; i<rows; i++)
        tableView->setRowHeight(i, 20);
    tableView->sortByColumn(1, Qt::AscendingOrder);
    tableView->setDisabled(false);
    if (tableView == this->leftTableView)
    {
        QStringList phoneColumnList = settings.value("phoneColumnList").toStringList();
        int col = this->phoneLeftModel->columnCount(QModelIndex());
        for (i = 0; i < col; i++)
        {
            if (!phoneColumnList.contains(QString::number(i)) && !phoneColumnList.isEmpty())
                this->leftTableView->hideColumn(i);
            else
                this->leftTableView->showColumn(i);
        }
        connect(tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(leftSelectedCount()));
        this->leftSelectedCount();
    }
    else
    {
        QStringList phoneColumnList = settings.value("phoneColumnList").toStringList();
        int col = this->phoneModel->columnCount(QModelIndex());
        for (i = 0; i < col; i++)
        {
            if (!phoneColumnList.contains(QString::number(i)) && !phoneColumnList.isEmpty())
                this->rightTableView->hideColumn(i);
            else
                this->rightTableView->showColumn(i);
        }
        connect(tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(rightSelectedCount()));
        this->rightSelectedCount();
    }
}

QList<File> *FileWidget::phoneFilesToCopy(QList<File> *fileList, Phone *phone)
{
    int x = fileList->size();
    QList<File> *tmpFiles = NULL;
    for (int i=0; i<x; i++)
    {
        if (fileList->at(i).fileType == "dir")
        {
            phone->cd(fileList->takeAt(i).fileName);
            tmpFiles = this->phoneFilesToCopy(phone->getFileList(), phone);
            while (!tmpFiles->isEmpty())
            {
                fileList->append(tmpFiles->takeFirst());
            }
            delete tmpFiles;
            tmpFiles = NULL;
            phone->cdUp();
            i--;
            x--;
        }
    }
    return fileList;
}

void FileWidget::phoneGoToDir()
{

}

void FileWidget::phoneHiddenFiles()
{
    if (leftTableView->hasFocus())
    {
        this->phoneLeft->setHiddenFiles(!this->phoneLeft->getHiddenFilesState());
        this->leftDisplay();
    }
    else
    {
        this->phone->setHiddenFiles(!this->phone->getHiddenFilesState());
        this->rightDisplay();
    }
}

void FileWidget::phoneNewDir()
{
    QTableView * tableView;
    Phone * phoneTmp;
    FileSortModel * sortModel;
    FileTableModel * fileModel;
    if (this->leftTableView->hasFocus())
    {
        phoneTmp = this->phoneLeft;
        tableView = this->leftTableView;
        sortModel = this->phoneLeftSortModel;
        fileModel = this->phoneLeftModel;
        this->leftChangeName = true;
    }
    else
    {
        phoneTmp = this->phone;
        tableView = this->rightTableView;
        sortModel = this->phoneSortModel;
        fileModel = this->phoneModel;
    }

    if (!phoneTmp->makeDir("new dir"))
    {
        QMessageBox::information(this,tr("error"),tr("dir was not created.\nMake sure that you are allowed to do this"),QMessageBox::Ok);
        return;
    }
    if (rightTableView->hasFocus())
        this->rightDisplay();
    else
        this->leftDisplay();

        int row = fileModel->getRow("new dir");
        QModelIndex index = fileModel->index(row, 1, QModelIndex());
        tableView->selectRow(sortModel->mapFromSource(index).row());
        QModelIndexList indexList = tableView->selectionModel()->selectedRows(1);
        if (indexList.size() == 1)
        {
            QModelIndex index = indexList.first();
            tableView->edit(index);
            connect (fileModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                     this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));
        }
}

void FileWidget::phoneOpenInNewTab()
{
    QModelIndexList indexList = rightTableView->selectionModel()->selectedRows();
    QModelIndex index;
    QString lastPath,tmp;
    while (!indexList.isEmpty())
    {
        index = this->phoneSortModel->mapToSource(indexList.takeFirst());
        lastPath = phone->getPath();
        tmp = phone->getPath();
        if (phone->cd(this->phoneModel->getFile(index.row()).fileName))
        {
            tmp = phone->getPath();
            if (this->rightTabBar->count() == 0)
            {
                this->rightTabBar->addTab(lastPath);
                this->rightTabBar->setTabToolTip(0,lastPath);
            }
            this->rightTabBar->addTab(tmp);
            this->rightTabBar->setTabToolTip(this->rightTabBar->count()-1,tmp);
            this->rightTabBar->setCurrentIndex(this->rightTabBar->count()-1);
            this->rightDisplay();
        }
    }

//    QString tmp = phone->getPath()+fileModel->getFile(index.row()).fileName;
}

void FileWidget::phoneRename()
{
    QTableView * tableView;
    Phone * phoneTmp;
    FileSortModel * sortModel;
    FileTableModel * fileModel;
    if (this->leftTableView->hasFocus())
    {
        phoneTmp = this->phoneLeft;
        tableView = this->leftTableView;
        sortModel = this->phoneLeftSortModel;
        fileModel = this->phoneLeftModel;
        this->leftChangeName = true;
    }
    else
    {
        phoneTmp = this->phone;
        tableView = this->rightTableView;
        sortModel = this->phoneSortModel;
        fileModel = this->phoneModel;
    }

    QModelIndexList indexList = tableView->selectionModel()->selectedRows(1);
    if (indexList.size() == 1)
    {
        QModelIndex index = indexList.first();
        tableView->edit(index);
        connect (fileModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                 this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));
    }
}

void FileWidget::phoneRenameSlot(QModelIndex indexS, QModelIndex indexE)
{
    QTableView * tableView;
    Phone * phoneTmp;
    FileSortModel * sortModel;
    FileTableModel * fileModel;
    if (this->leftChangeName)
    {
        this->leftChangeName = false;
        phoneTmp = this->phoneLeft;
        tableView = this->leftTableView;
        sortModel = this->phoneLeftSortModel;
        fileModel = this->phoneLeftModel;
    }
    else
    {
        phoneTmp = this->phone;
        tableView = this->rightTableView;
        sortModel = this->phoneSortModel;
        fileModel = this->phoneModel;
    }
    disconnect (fileModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this, SLOT(phoneRenameSlot(QModelIndex, QModelIndex)));

    if (indexS == indexE)
    {
        File tmpFile = fileModel->getFile(indexS.row());
        QString oldFile = tmpFile.filePath;
        oldFile.remove(phoneTmp->getPath());
        phoneTmp->rename(oldFile, indexS.data(Qt::DisplayRole).toString());
        if (this->leftTableView->hasFocus())
        {
            this->leftDisplay();
        }
        else
        {
            this->rightDisplay();
        }
    }
}

void FileWidget::on_toolButtonFind_pressed()
{
    if (ui->rightFileNameFilter->text().isEmpty())
        return;
    if (threadFind.isRunning())
        threadFind.terminate();

    this->rightTableView->setDisabled(true);
    this->ui->rightLabelSelectedCount->setText(tr("searching..."));
    if (findModel->rowCount() > 0)
        this->findModel->clear();
    this->rightTableView->setModel(this->findModel);
    threadFind.sdk=this->phone->getSdk();
    threadFind.fileName=ui->rightFileNameFilter->text();
    threadFind.path=this->phone->getPath();
    threadFind.start();
}

void FileWidget::rightContextMenu(const QPoint &pos)
{
    this->phoneContextMenu(pos, rightTableView);
}

void FileWidget::rightComboBox()
{
    if (phone->cd(ui->rightComboBox->lineEdit()->text()))
        this->rightDisplay();
//    QString path=this->phone->getPath();
//    this->phone->setPath(ui->rightComboBox->lineEdit()->text().toUtf8());
//    if (this->phone->cd("."))
//    {
//        this->rightDisplay();
//    }
//    else
//        this->phone->setPath(path);
}

void FileWidget::rightDisplay()
{
    ui->rightFileNameFilter->clear();
    this->phoneModel->setColoring(this->settings->color);
    this->phone->setHiddenFiles(this->settings->phoneHiddenFiles);
    this->rightTableView->horizontalHeader()->setVisible(this->settings->showPhoneColumnsHeaders);
    this->phoneDisplay(this->rightTableView);
    this->rightTableView->setFocus();
    int widthTab = this->rightTableView->width();
    int colIcon = 25;
    int colName, colDate, colSize;

    this->rightTableView->resizeColumnsToContents();
    colSize = this->rightTableView->columnWidth(2);
    colDate = this->rightTableView->columnWidth(3);
    if (this->rightTableView->verticalScrollBar() == NULL)
        colName = widthTab - colIcon - colSize - colDate;
    else
        colName = widthTab - colIcon - colSize - colDate - this->rightTableView->verticalScrollBar()->width();


    this->rightTableView->setColumnWidth(0, colIcon);
    this->rightTableView->setColumnWidth(1, colName - 2);
    this->rightTableView->setColumnWidth(2, colSize);
    this->rightTableView->setColumnWidth(3, colDate);

    if (this->rightTabBar->count()>0)
    {
        this->rightTabBar->setTabText(this->rightTabBar->currentIndex(),this->phone->getPath());
        this->rightTabBar->setTabToolTip(this->rightTabBar->currentIndex(),this->phone->getPath());
        this->rightTabBar->setMaximumWidth(rightTableView->width());
    }
}

void FileWidget::rightDoubleClick()
{
    if (!this->rightTableView->selectionModel()->selection().isEmpty())
    {
        QModelIndex index;
        QString fileName;
        if (rightTableView->model()==this->findModel)
        {
            File file;
            index = this->rightTableView->selectionModel()->selection().indexes().takeFirst();
            file = this->findModel->getFile(index.row());
            fileName=file.fileName;
            if (file.fileType == "file")
                fileName = fileName.left(fileName.lastIndexOf("/"));
        }
        else
        {
            index = this->rightTableView->selectionModel()->selection().indexes().takeFirst();
            index = this->phoneSortModel->mapToSource(index);
            fileName = this->phoneModel->getFile(index.row()).fileName;
        }

        if (fileName.contains(" -> "))
        {
            fileName = fileName.split(" -> ").at(1);
        }
        if (this->phone->cd(fileName))
        {
            this->rightDisplay();
        }
    }
}

void FileWidget::rightRefresh()
{
    this->rightDisplay();
}

void FileWidget::rightSelectAll()
{
    QModelIndexList indexList = this->rightTableView->selectionModel()->selectedRows(1);
    if (indexList.size() == this->phoneModel->rowCount())
        this->rightTableView->clearSelection();
    else
        this->rightTableView->selectAll();
}

void FileWidget::rightSelectedCount()
{
    QModelIndexList indexList = this->rightTableView->selectionModel()->selectedRows(1);
    QString word(QString::number(indexList.size()) + "/" +
                 QString::number(this->rightTableView->model()->rowCount(QModelIndex()))+ " " + tr("selected"));
    this->ui->rightLabelSelectedCount->setText(word);
}

void FileWidget::rightTabBarPageChanged()
{
    if (this->phone->cd(this->rightTabBar->tabText(this->rightTabBar->currentIndex())))
        this->rightDisplay();
}

void FileWidget::rightTabBarCloseTab(int index)
{
    if (this->rightTabBar->currentIndex() == index)
    {
        if (this->rightTabBar->count()>0)
        {
            this->rightTabBar->removeTab(index);
            this->rightTabBar->setCurrentIndex(0);
        }
    }
    else
        this->rightTabBar->removeTab(index);

    if (this->rightTabBar->count() == 1)
        this->rightTabBar->removeTab(0);
}

void FileWidget::rightTableWidgetActivated()
{
//    QPalette paleta=ui->rightTableWidget->palette();
//    paleta.setColor(QPalette::Highlight,QColor(204,204,204));
//    ui->rightTableWidget->setPalette(paleta);

//    paleta=ui->leftTableWidget->palette();
//    paleta.setColor(QPalette::Highlight,QColor(153,197,255));
//    ui->leftTableWidget->setPalette(paleta);
}

void FileWidget::showAppInfo()
{
    App *app;
    if (!this->leftTableView->selectionModel()->selection().isEmpty())
    {
        QModelIndex index = this->leftTableView->selectionModel()->selection().indexes().takeFirst();
        index = this->computerSortModel->mapToSource(index);
        QString fileName = this->computerModel->getFile(index.row()).filePath;

        if (fileName.contains(QRegExp(".apk$")))
        {
            app = this->getAppInfo(fileName);
            if (this->appInfoDialog != NULL)
                delete this->appInfoDialog;
            this->appInfoDialog = new appInfo(this, app);
            this->appInfoDialog->show();
        }
        else
        {
            QMessageBox::information(this, tr("information"), tr("It's not an application"), QMessageBox::Ok);
        }
    }
}



void ThreadFind::run()
{
    QProcess *proces=new QProcess;
    QString output;
    QString path, file;
    QStringList strList;
    proces->start("\""+this->sdk+"\"adb shell busybox find "+this->path+" -iname \'*"+this->fileName+"*\'");

    Phone phone(this->sdk,false);
    phone.setConnectionState(DEVICE);

    do
    {
        proces->waitForReadyRead();
        output=proces->readAll();
//        if (output.isEmpty())
//            break;
        strList = output.split("\n");
        while (!strList.isEmpty())
        {
            output = strList.takeFirst();
            if (output.isEmpty())
                continue;
            QList<File> *fileList;
            path = output.left(output.lastIndexOf("/"));
            file = output.right(output.size() - output.lastIndexOf("/") - 1);
            file.remove(QRegExp("\\s+$"));
            phone.setPath(path);
            fileList = phone.getFileList(file);
            File plik;
            if (fileList != NULL)
                plik = fileList->takeFirst();
            else
                continue;
            delete fileList;
            plik.fileName.prepend(phone.getPath());
            emit this->foundFile(plik);
        }
    } while (proces->bytesAvailable() > 0);
    emit this->finished();
    delete proces;
}

void FileWidget::foundFile(File file)
{
    QFile plik;
    QFileIconProvider *provider = new QFileIconProvider;
    if (file.fileType == "file" || file.fileType == "device")
    {
        QString name;
        name = file.fileName.right(file.fileName.size() - file.fileName.lastIndexOf("/") - 1);

        plik.setFileName(QDir::currentPath()+"/tmp/"+name);
        plik.open(QFile::WriteOnly);
        file.fileIcon = provider->icon(QFileInfo(plik));
        plik.remove();
    }
    else if (file.fileType == "link")
    {
        file.fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
    }
    else
        file.fileIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);

    this->findModel->insertFile(0, file);
    this->rightTableView->resizeColumnsToContents();
    this->rightTableView->resizeRowsToContents();
    int i=0;
    int rows = this->findModel->rowCount();
    for (i=0; i<rows; i++)
        this->rightTableView->setRowHeight(i, 20);
    this->rightTableView->sortByColumn(1, Qt::AscendingOrder);
}

void FileWidget::findFinished()
{
    this->rightTableView->setDisabled(false);
    connect(this->rightTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(rightSelectedCount()));
    this->rightSelectedCount();
}

void FileWidget::installAppFromComputer()
{
//    AppList selected;

//    for (int i=0;i<ui->leftTableWidget->selectedItems().count();i++)
//    {
//        if (ui->leftTableWidget->selectedItems().at(i)->column()==1&&ui->leftTableWidget->selectedItems().at(i)->text().contains(QRegExp(".apk$")))
//            selected.package.append(this->computer->getPath()+ "/" +ui->leftTableWidget->selectedItems().at(i)->text());
//    }
//    if (selected.package.length()==0)
//    {
//        QMessageBox::information(this, "install", "there is no files to install", QMessageBox::Ok);
//        return;
//    }

//    if (QMessageBox::question(this,tr("install"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
//        return;

//    if (this->appsDialog != NULL)
//        delete this->appsDialog;
//    this->appsDialog=new appDialog(this,selected, appDialog::Install, appDialog::None,this->phone);
//    this->appsDialog->show();
//    //    connect(this->appsDialog,SIGNAL(finished(int)),this,SLOT(showPageApps()));

    if (this->leftTableView->selectionModel()->selection().isEmpty())
    {
        QMessageBox::information(this, "install", "there is no files to install", QMessageBox::Ok);
        return;
    }

    QList<App> appList;
    App app;
    File file;

    QModelIndexList indexList = this->leftTableView->selectionModel()->selectedRows();
    QModelIndex index;
    while (!indexList.isEmpty())
    {
        index = this->computerSortModel->mapToSource(indexList.takeFirst());
        file=this->computerModel->getFile(index.row());
        app.appFile=file.filePath;
        app.appIcon=file.fileIcon;
        app.appName=file.fileName;
        appList.append(app);
    }

//    selected.package.append(this->app->appFile);

    if (QMessageBox::question(this,tr("install"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

//    int i=0;

    if (this->appsDialog != NULL)
        delete this->appsDialog;
    this->appsDialog=new appDialog(this,appList, appDialog::Install, appDialog::None);
    this->appsDialog->show();
}

App * FileWidget::getAppInfo(QString filePath)
{
    QProcess *proces = new QProcess;
    QString temp;
    QStringList aaptLines, aaptLineParts;
    QSettings settings;
    QByteArray ba;
    QPixmap pix;
    QString sdk;
    sdk = settings.value("sdkPath").toString();

    QFileInfo *plik = new QFileInfo(filePath);
    settings.beginGroup("apps");
    proces->start("\""+sdk+"\"aapt d badging \"" + filePath + "\"");
    proces->waitForReadyRead(-1);
    temp=proces->readAll();
    if (temp.contains("ERROR"))
        return NULL;

    App *app = new App;
    app->appFile = filePath;
    app->appFileName = filePath;
    app->appFileName.replace("//", ";");
    app->appSize = QString::number(plik->size());
    delete plik;

    qDebug()<<"Apps aapt - "<<temp;
    proces->close();
    proces->terminate();
    delete proces;
    aaptLines=temp.split("\n");
    while (aaptLines.length()>0)
    {
        aaptLineParts=aaptLines.first().split(QRegExp("\'\\s"));
        while (aaptLineParts.length()>0)
        {
            if (aaptLineParts.first().contains(QRegExp("name=")) && app->packageName.isEmpty())
            {
                app->packageName=aaptLineParts.first();
                app->packageName.remove(0,app->packageName.indexOf("name=")+5);
                app->packageName.remove("'");
            }
            else if (aaptLineParts.first().contains(QRegExp("^versionName=")) && app->appVersion.isEmpty())
            {
                app->appVersion=aaptLineParts.first();
                app->appVersion.remove("versionName='");
                app->appVersion.remove("'");
            }
            else if (aaptLineParts.first().contains(QRegExp("label="))&&app->appName.isEmpty())
            {
                app->appName=QString::fromUtf8(aaptLineParts.first().toAscii());
                app->appName.remove(0,app->appName.indexOf("label=")+6);
                app->appName.remove("'");
            }
            else if (aaptLineParts.first().contains(QRegExp("^icon="))&&app->icoName.isEmpty())
            {
                app->icoName=aaptLineParts.first();
                app->icoName.remove("icon='");
                app->icoName.remove("'");
            }
            aaptLineParts.removeFirst();
        }
        aaptLines.removeFirst();
    }

    if (!settings.contains(app->packageName))
    {
        settings.setValue(app->packageName+"/icoName", app->icoName);
        settings.setValue(app->packageName+"/appName", QString::fromUtf8(app->appName.toAscii()));
        settings.setValue(app->packageName+"/version", app->appVersion);
    }

    temp=app->packageName;
    temp.append(".png");
    if (!settings.contains(app->packageName+"/icon"))
    {
        unpack(app->appFile, QDir::currentPath()+"/icons/", app->icoName, temp);

        QFile icon(QDir::currentPath()+"/icons/"+app->packageName.toLatin1()+".png");
        icon.open(QIODevice::ReadWrite);
        ba = icon.readAll();
        settings.setValue(app->packageName+"/icon", ba); //- zapisanie pixmap w QSettings
        icon.remove();
    }
    ba = settings.value(app->packageName+"/icon").toByteArray();
    pix.loadFromData(ba);
    QIcon icon(pix);
    app->appIcon = icon;

    qDebug()<<"Apps got app name - "<<settings.value(app->packageName+"/appName").toString();
    qDebug()<<"Apps got app icon - "<<settings.value(app->packageName+"/icoName").toString();
    qDebug()<<"Apps got app version - "<<settings.value(app->packageName+"/version").toString();
    settings.endGroup();
    return app;
}

void FileWidget::setLeftFilter(QString pattern)
{
    QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp);

    if (this->leftMode == "computer")
    {
        this->computerSortModel->setFilterRegExp(regExp);
    }
    else
    {
        this->phoneLeftSortModel->setFilterRegExp(regExp);
    }
    this->leftTableView->resizeColumnsToContents();
    this->leftTableView->resizeRowsToContents();

}

void FileWidget::setRightFilter(QString pattern)
{
    QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp);

    this->phoneSortModel->setFilterRegExp(regExp);
    this->rightTableView->resizeColumnsToContents();
    this->rightTableView->resizeRowsToContents();

}

void FileWidget::on_pushButton_pressed()
{
    installAppFromComputer();
}

void FileWidget::copySlotToComputer(QStringList list)
{
    FileTableModel * fileModel = this->phoneModel;

    QSettings settings;
    if (settings.value("showCopyConfirmation", true).toBool())
    {
	if (QMessageBox::question(this,tr("copying"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
	    return;
    }

    int row;

    QList<File> *filesToCopy = new QList<File>;
    while (!list.isEmpty())
    {
        row = list.takeFirst().toInt();
        filesToCopy->append(fileModel->getFile(row));
    }
    filesToCopy = this->phoneFilesToCopy(filesToCopy, this->phone);
    if (filesToCopy->isEmpty())
    {
        QMessageBox::information(this,"",tr("theres nothing to copy. Maybe selected dirs are empty"),QMessageBox::Ok);
        return;
    }

    if (this->dialog != NULL)
        delete this->dialog;
        this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::PhoneToComputer,
                                        this->phone->getPath(), this->computer->getPath());

    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();

    connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));

    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(leftRefresh()));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(rightRefresh()));
}

void FileWidget::copySlotToPhone(QStringList list)
{
    FileTableModel * fileModel;
    if (this->leftMode == "computer")
        fileModel = this->computerModel;
    else
        fileModel = this->phoneLeftModel;

    QSettings settings;
    if (settings.value("showCopyConfirmation", true).toBool())
    {
	if (QMessageBox::question(this,tr("copying"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
	    return;
    }

    int row;

    QList<File> *filesToCopy = new QList<File>;
    while (!list.isEmpty())
    {
        row = list.takeFirst().toInt();
        filesToCopy->append(fileModel->getFile(row));
    }
    if (this->leftMode == "computer")
        filesToCopy = this->computerFilesToCopy(filesToCopy);
    else
        filesToCopy = this->phoneFilesToCopy(filesToCopy, this->phoneLeft);

    if (filesToCopy->isEmpty())
    {
        QMessageBox::information(this,"",tr("theres nothing to copy. Maybe selected dirs are empty"),QMessageBox::Ok);
        return;
    }

    if (this->dialog != NULL)
        delete this->dialog;
    if (this->leftMode == "computer")
        this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::ComputerToPhone,
                                        this->computer->getPath(), this->phone->getPath());
    else
        this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::PhoneToPhone,
                                        this->phoneLeft->getPath(), this->phone->getPath());

    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();

    connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));

    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(leftRefresh()));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(rightRefresh()));
}

void FileWidget::copySlotToPhoneLeft(QStringList list)
{
    FileTableModel * fileModel;

    fileModel = this->phoneModel;

    QSettings settings;
    if (settings.value("showCopyConfirmation", true).toBool())
    {
	if (QMessageBox::question(this,tr("copying"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
	    return;
    }

    int row;

    QList<File> *filesToCopy = new QList<File>;
    while (!list.isEmpty())
    {
        row = list.takeFirst().toInt();
        filesToCopy->append(fileModel->getFile(row));
    }

    filesToCopy = this->phoneFilesToCopy(filesToCopy, this->phone);

    if (filesToCopy->isEmpty())
    {
        QMessageBox::information(this,"",tr("theres nothing to copy. Maybe selected dirs are empty"),QMessageBox::Ok);
        return;
    }

    if (this->dialog != NULL)
        delete this->dialog;

    this->dialog = new dialogKopiuj(this, filesToCopy, this->sdk, dialogKopiuj::PhoneToPhone,
                                    this->phone->getPath(), this->phoneLeft->getPath());

    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();

    connect(this->dialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->dialog, SIGNAL(finished(int)), this, SIGNAL(copyFinished(int)));

    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(leftRefresh()));
    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(rightRefresh()));
}
