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


#include "appwidget.h"
#include "ui_appwidget.h"

AppWidget::AppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget)
{
    ui->setupUi(this);

//    this->ui->labelQRcode->hide();
//    this->ui->labelCyrketVersion->hide();
//    this->ui->editCyrketVersion->hide();

    this->appModel = new AppTableModel;
    this->appSortModel = new AppSortModel;
    this->systemAppModel = new AppTableModel;
    this->systemAppSortModel = new AppSortModel;
    this->backupModel = new BackupTableModel;
    this->backupSortModel = new BackupSortModel;

    this->appSortModel->setSourceModel(this->appModel);
    this->systemAppSortModel->setSourceModel(this->systemAppModel);
    this->backupSortModel->setSourceModel(this->backupModel);

    this->appSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->systemAppSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->backupSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    this->appSortModel->setDynamicSortFilter(true);

    this->ui->stackedAppsPageApps->setLayout(this->ui->horizontalLayoutStackedApps);
    this->ui->stackedAppsPageBackups->setLayout(this->ui->horizontalLayoutBackup);

    this->appMenu = NULL;
    this->backupMenu = NULL;

    QSettings settings;
    this->phone = new Phone(settings.value("sdkPath").toString(),false);
    this->sdk = this->phone->getSdk();

    qRegisterMetaType<App>("App");
    qRegisterMetaType<Backup>("Backup");

    connect(&this->threadApps, SIGNAL(gotApp(App)), this, SLOT(insertApp(App)));
    connect(&this->threadApps, SIGNAL(gotAllApps(QThread *)), this, SLOT(gotAllApps(QThread *)));
    connect(&this->threadBackups, SIGNAL(gotBackup(Backup)), this, SLOT(insertBackup(Backup)));
    connect(&this->threadBackups, SIGNAL(gotAllApps(QThread *)), this, SLOT(gotAllApps(QThread *)));
    connect(&this->threadApps, SIGNAL(maximum(int)), this->ui->progressApps, SLOT(setMaximum(int)));
    connect(&this->threadApps, SIGNAL(value(int)), this->ui->progressApps, SLOT(setValue(int)));
    connect(&this->threadBackups, SIGNAL(maximum(int)), this->ui->progressApps, SLOT(setMaximum(int)));
    connect(&this->threadBackups, SIGNAL(value(int)), this->ui->progressApps, SLOT(setValue(int)));
    connect(&this->threadApps, SIGNAL(missingAapt()), this, SLOT(missingAapt()));
    connect(&this->threadApps, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(&this->threadBackups, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));

    connect(ui->comboBoxApps, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboBoxAppsChanged()));
    connect(ui->buttonRefreshApps, SIGNAL(clicked()), this, SLOT(refreshApps()));

    connect(this->ui->buttonOpenMarket, SIGNAL(clicked()), this, SLOT(openMarket()));
    connect(this->ui->buttonAppBrain, SIGNAL(clicked()), this, SLOT(openAppBrain()));

    connect(this->ui->editFilter, SIGNAL(textChanged(QString)), this, SLOT(filter()));
    connect(this->ui->comboFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(filter()));

    int layoutDirection = settings.value("layoutDirection",0).toInt();

    QMenu *menu=new QMenu;
    if (layoutDirection == 0)
        menu->setLayoutDirection(Qt::LeftToRight);
    if (layoutDirection == 1)
        menu->setLayoutDirection(Qt::RightToLeft);
    menu->setTitle(tr("backup", "backup menu title"));
    menu->setIcon(QIcon(":/icons/backup.png"));
    menu->addAction(tr("app", "backup menu action"),this,SLOT(toolButtonBackupApp()))->setData(QString("app"));
    menu->addAction(tr("data", "backup menu action"),this,SLOT(toolButtonBackupData()))->setData(QString("data"));
    menu->addAction(tr("app + data", "backup menu action"),this,SLOT(toolButtonBackupAppAndData()))->setData(QString("app + data"));
    ui->toolButtonBackup->setMenu(menu);

    menu=new QMenu;
    if (layoutDirection == 0)
        menu->setLayoutDirection(Qt::LeftToRight);
    if (layoutDirection == 1)
        menu->setLayoutDirection(Qt::RightToLeft);
    menu->setTitle(tr("restore", "restore menu title"));
    menu->setIcon(QIcon(":/icons/restore.png"));
    menu->addAction(tr("app", "restore menu action"),this,SLOT(toolButtonRestoreApp()))->setData(QString("app"));
    menu->addAction(tr("data", "restore menu action"),this,SLOT(toolButtonRestoreData()))->setData(QString("data"));
    menu->addAction(tr("app + data", "restore menu action"),this,SLOT(toolButtonRestoreAppAndData()))->setData(QString("app + data"));
    ui->toolButtonRestore->setMenu(menu);

    this->dialog=NULL;
    this->appsDialog=NULL;

    this->ui->tableView->setModel(this->appModel);
    this->ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setLayout(ui->layoutApps);
    this->comboBoxAppsChanged();
}

AppWidget::~AppWidget()
{
    delete ui;
    delete this->phone;
}

void AppWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        if (this->appMenu != NULL)
        {
            foreach(QAction *action, this->appMenu->actions())
                action->setText(tr(action->data().toString().toAscii()));
        }
        if (this->backupMenu != NULL)
        {
            foreach(QAction *action, this->backupMenu->actions())
                action->setText(tr(action->data().toString().toAscii()));
        }
        foreach(QAction *action, this->ui->toolButtonBackup->menu()->actions())
            action->setText(tr(action->data().toString().toAscii()));
        foreach(QAction *action, this->ui->toolButtonRestore->menu()->actions())
            action->setText(tr(action->data().toString().toAscii()));

        break;
    default:
        break;
    }
}

void AppWidget::copyAppToPC()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    App app;
    QList<App> *tmpList = new QList<App>;
    while (!indexList.isEmpty())
    {
        if (this->ui->comboBoxApps->currentIndex() == 0)
        {
            app = this->appModel->getApp(this->appSortModel->mapToSource(indexList.takeFirst()).row());
        }
        else if (this->ui->comboBoxApps->currentIndex() == 2)
        {
            app = this->systemAppModel->getApp(this->systemAppSortModel->mapToSource(indexList.takeFirst()).row());
        }
        tmpList->append(app);
    }
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Choose where You want to save selected apps"),
                                                     "", QFileDialog::ShowDirsOnly);

    if (this->dialog != NULL)
        delete this->dialog;
    this->dialog = new dialogKopiuj(this, tmpList, this->sdk, dialogKopiuj::AppsToComputer, path);
    if (this->alwaysCloseCopy)
        this->dialog->closeAfterFinished();
    if (this->dialogKopiujShowModal)
        this->dialog->setModal(true);
    this->dialog->show();
}

void AppWidget::appsContextMenu(const QPoint &pos)
{
    if (this->appMenu == NULL)
    {
        this->appMenu = new QMenu;
        QAction *backup;
        backup = this->appMenu->addMenu(this->ui->toolButtonBackup->menu());
        backup->setData(QString("backup"));
        this->appMenu->addAction(QIcon(":icons/uninstall.png"),tr("uninstall", "right click apps menu"),this,SLOT(on_toolButtonUninstall_pressed()))->setData(QString("uninstall"));
        this->appMenu->addAction(QIcon(":icons/save.png"),tr("copy to PC", "right click apps menu"), this, SLOT(copyAppToPC()))->setData(QString("copy to PC"));
        this->appMenu->addAction(QIcon(":/icons/info.png"), tr("Get apps info", "right click apps menu"), this, SLOT(getAppInfo()))->setData(QString("Get apps info"));
        this->appMenu->addAction(tr("Get apps version from internet", "right click apps menu"), this, SLOT(getCyrketVersions()))->setData(QString("Get apps version from internet"));
    }
    QPoint pos2;
    pos2.setX(pos.x());
    pos2.setY(pos.y()+20);


    this->appMenu->exec(ui->tableView->mapToGlobal(pos2));
}

void AppWidget::backupsContextMenu(const QPoint &pos)
{
    if (this->backupMenu == NULL)
    {
        this->backupMenu = new QMenu;
        this->backupMenu->addMenu(this->ui->toolButtonRestore->menu())->setData(QString("restore"));
        this->backupMenu->addAction(QIcon(":icons/remove.png"),tr("remove", "right click backups menu"),this,SLOT(on_toolButtonRemoveBackup_pressed()))->setData(QString("remove"));
    }
    QPoint pos2;
    pos2.setX(pos.x());
    pos2.setY(pos.y()+20);
    this->backupMenu->exec(ui->tableView->mapToGlobal(pos2));
}

void AppWidget::appsSelectedCount()
{
    this->ui->buttonOpenMarket->setDisabled(true);
    this->ui->toolButtonBackup->setDisabled(true);
    this->ui->toolButtonRemoveBackup->setDisabled(true);
    this->ui->toolButtonRestore->setDisabled(true);
    this->ui->toolButtonUninstall->setDisabled(true);
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    if (indexList.isEmpty())
    {
        this->ui->tableView->selectRow(0);
    }
    else
    {
        QString word;
        word.setNum(indexList.size());
        word.append("/"+QString::number(this->ui->tableView->model()->rowCount(QModelIndex())) + " ");
        word.append(tr("selected", "selected label below table"));
        this->ui->appsLabelSelected->setText(word);
        if (this->ui->comboBoxApps->currentIndex() == 0)
        {
            //apps
            App app = this->appModel->getApp(this->appSortModel->mapToSource(indexList.first()).row());
            this->ui->editAppsLocation->setEnabled(true);
            this->ui->editAppsFileName->setEnabled(true);
            this->ui->editAppsAppName->setEnabled(true);
            this->ui->editAppsVersion->setEnabled(true);
            this->ui->editAppsSize->setEnabled(true);
            this->ui->editAppsPackageName->setEnabled(true);
            if (app.cyrketVer.isEmpty())
            {
                this->ui->editCyrketVersion->setEnabled(false);
            }
            else
            {
                this->ui->editCyrketVersion->setEnabled(true);
            }

            this->ui->buttonOpenMarket->setDisabled(false);
            this->ui->buttonAppBrain->setDisabled(false);
            this->ui->toolButtonBackup->setDisabled(false);
            this->ui->toolButtonRemoveBackup->setDisabled(true);
            this->ui->toolButtonRestore->setDisabled(true);
            this->ui->toolButtonUninstall->setDisabled(false);

            this->ui->labelQRcode->setPixmap(app.qrCode);
            this->ui->labelAppsIcon->setPixmap(app.appIcon.pixmap(100,100));
            this->ui->editAppsAppName->setText(app.appName);
            this->ui->editAppsVersion->setText(app.appVersion);
            this->ui->editAppsSize->setText(app.appSize);
            this->ui->editAppsFileName->setText(app.appFile);
            this->ui->editAppsPackageName->setText(app.packageName);
            this->ui->editAppsLocation->setText(app.location);
            this->ui->editCyrketVersion->setText(app.cyrketVer);
        }
        else if (this->ui->comboBoxApps->currentIndex() == 1)
        {
            //backups
            Backup backup = this->backupModel->getBackup(this->backupSortModel->mapToSource(indexList.first()).row());
            this->ui->editAppsLocation->clear();
            this->ui->editAppsFileName->clear();
            this->ui->editAppsLocation->setEnabled(false);
            this->ui->editAppsFileName->setEnabled(false);
            this->ui->editAppsAppName->setEnabled(true);
            this->ui->editAppsVersion->setEnabled(true);
            this->ui->editAppsSize->setEnabled(true);
            this->ui->editAppsPackageName->setEnabled(true);
            this->ui->buttonOpenMarket->setDisabled(false);
            this->ui->buttonAppBrain->setDisabled(false);
            this->ui->toolButtonBackup->setDisabled(true);
            this->ui->toolButtonRemoveBackup->setDisabled(false);
            this->ui->toolButtonRestore->setDisabled(false);
            this->ui->toolButtonUninstall->setDisabled(true);
            this->ui->editCyrketVersion->setDisabled(true);

            this->ui->labelAppsIcon->setPixmap(backup.appIcon.pixmap(100,100));
            this->ui->editAppsAppName->setText(backup.appName);
            this->ui->editAppsVersion->setText(backup.appVersion);
            this->ui->editAppsSize->setText(backup.appSize);
            this->ui->editAppsPackageName->setText(backup.packageName);
        }
        else if (this->ui->comboBoxApps->currentIndex() == 2)
        {
            //system apps
            App app = this->systemAppModel->getApp(this->systemAppSortModel->mapToSource(indexList.first()).row());
            this->ui->editAppsLocation->setEnabled(true);
            this->ui->editAppsFileName->setEnabled(true);
            this->ui->editAppsAppName->setEnabled(true);
            this->ui->editAppsVersion->setEnabled(true);
            this->ui->editAppsSize->setEnabled(true);
            this->ui->editAppsPackageName->setEnabled(true);
            this->ui->editCyrketVersion->setDisabled(true);
            this->ui->buttonOpenMarket->setDisabled(true);
            this->ui->buttonAppBrain->setDisabled(true);
            this->ui->toolButtonBackup->setDisabled(false);
            this->ui->toolButtonRemoveBackup->setDisabled(true);
            this->ui->toolButtonRestore->setDisabled(true);
            this->ui->toolButtonUninstall->setDisabled(false);

            this->ui->labelAppsIcon->setPixmap(app.appIcon.pixmap(100,100));
            this->ui->editAppsAppName->setText(app.appName);
            this->ui->editAppsVersion->setText(app.appVersion);
            this->ui->editAppsSize->setText(app.appSize);
            this->ui->editAppsFileName->setText(app.appFile);
            this->ui->editAppsPackageName->setText(app.packageName);
            this->ui->editAppsLocation->setText(app.location);
        }
    }
}

void AppWidget::comboBoxAppsChanged()
{
    this->ui->buttonOpenMarket->setDisabled(true);
    this->ui->buttonAppBrain->setDisabled(true);
    this->ui->toolButtonBackup->setDisabled(true);
    this->ui->toolButtonRemoveBackup->setDisabled(true);
    this->ui->toolButtonRestore->setDisabled(true);
    this->ui->toolButtonUninstall->setDisabled(true);
    this->ui->labelQRcode->clear();
    this->ui->editCyrketVersion->clear();
    this->ui->editCyrketVersion->setDisabled(true);
    this->ui->editFilter->setDisabled(true);
    this->ui->comboFilter->setDisabled(true);
    this->ui->comboFilter->clear();
    this->ui->editFilter->clear();

    this->ui->progressApps->setRange(0,0);
    this->ui->progressApps->setValue(0);
    this->ui->progressApps->show();
    QSettings settings;
    ui->labelAppsIcon->clear();
    ui->editAppsAppName->clear();
    ui->editAppsVersion->clear();
    ui->editAppsSize->clear();
    ui->editAppsFileName->clear();
    ui->editAppsPackageName->clear();
    ui->editAppsLocation->clear();
    ui->editAppsAppName->setDisabled(true);
    ui->editAppsVersion->setDisabled(true);
    ui->editAppsSize->setDisabled(true);
    ui->editAppsFileName->setDisabled(true);
    ui->editAppsPackageName->setDisabled(true);
    ui->editAppsLocation->setDisabled(true);

    disconnect(this->ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(appsSelectedCount()));
    this->ui->tableView->setSelectionMode(QTableView::NoSelection);
    this->ui->tableView->setSortingEnabled(false);
    this->ui->tableView->horizontalHeader()->setVisible(true);
    this->ui->tableView->setDisabled(true);
    ui->comboBoxApps->setDisabled(true);

    if (ui->comboBoxApps->currentIndex()==0)//apps
    {
        this->ui->comboFilter->addItem(tr("App name", "combo apps filter"), 1);
        this->ui->comboFilter->addItem(tr("Package name", "combo apps filter"), 4);
        this->ui->comboFilter->addItem(tr("File path", "combo apps filter"), 5);
        this->ui->comboFilter->addItem(tr("Location", "combo apps filter"), 6);

        disconnect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(backupsContextMenu(const QPoint &)));
        connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(appsContextMenu(const QPoint &)));


        ui->stackedWidgetApps->setCurrentIndex(0);

        if (settings.value("getAppInfo", 0) == 0 && !this->appModel->isEmpty())
        {
            if (!this->threadApps.isRunning())
            {
                this->ui->tableView->setModel(this->appSortModel);
                this->gotAllApps(NULL);
            }
        }
        else
        {
            this->appModel->clear();
            this->ui->tableView->setModel(this->appSortModel);
            this->threadApps.systemApps = false;
            this->threadApps.sdk=this->sdk;
            this->threadApps.start();
        }
        QStringList appColumnList = settings.value("appColumnList").toStringList();
        int col = this->appModel->columnCount(QModelIndex());
        for (int i = 0; i < col; i++)
        {
            if (!appColumnList.contains(QString::number(i)) && !appColumnList.isEmpty())
                this->ui->tableView->hideColumn(i);
            else
                this->ui->tableView->showColumn(i);
        }
    }
    else if (ui->comboBoxApps->currentIndex()==1)//backups
    {
        this->ui->comboFilter->addItem(tr("App name", "combo backups filter"), 1);
        this->ui->comboFilter->addItem(tr("Package name", "combo backups filter"), 4);

        disconnect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(appsContextMenu(const QPoint &)));
        connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(backupsContextMenu(const QPoint &)));

        ui->stackedWidgetApps->setCurrentIndex(1);
        if (settings.value("getBackupInfo", 0) == 0 && !this->backupModel->isEmpty())
        {
            if (!this->threadBackups.isRunning())
            {
                this->ui->tableView->setModel(this->backupSortModel);
                this->gotAllApps(NULL);
            }
        }
        else
        {
            this->backupModel->clear();
            this->ui->tableView->setModel(this->backupSortModel);
            this->threadBackups.sdk = this->sdk;
            this->threadBackups.start();
        }
        QStringList backupColumnList = settings.value("backupColumnList").toStringList();
        int col = this->backupModel->columnCount(QModelIndex());
        for (int i = 0; i < col; i++)
        {
            if (!backupColumnList.contains(QString::number(i)) && !backupColumnList.isEmpty())
                this->ui->tableView->hideColumn(i);
            else
                this->ui->tableView->showColumn(i);
        }
    }
    else if (ui->comboBoxApps->currentIndex()==2)//system apps
    {
        this->ui->comboFilter->addItem(tr("App name", "combo apps filter"), 1);
        this->ui->comboFilter->addItem(tr("Package name", "combo apps filter"), 4);
        this->ui->comboFilter->addItem(tr("File path", "combo apps filter"), 5);
        this->ui->comboFilter->addItem(tr("Location", "combo apps filter"), 6);

        disconnect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(backupsContextMenu(const QPoint &)));
        connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(appsContextMenu(const QPoint &)));

        ui->stackedWidgetApps->setCurrentIndex(0);

        if (settings.value("getSystemAppInfo", 0) == 0 && !this->systemAppModel->isEmpty())
        {
            if (!this->threadApps.isRunning())
            {
                this->ui->tableView->setModel(this->systemAppSortModel);
                this->gotAllApps(NULL);
            }
        }
        else
        {
            this->systemAppModel->clear();
            this->ui->tableView->setModel(this->systemAppSortModel);
            this->threadApps.systemApps = true;
            this->threadApps.sdk=this->sdk;
            this->threadApps.start();
        }
        QStringList appColumnList = settings.value("appColumnList").toStringList();
        int col = this->appModel->columnCount(QModelIndex());
        for (int i = 0; i < col; i++)
        {
            if (!appColumnList.contains(QString::number(i)) && !appColumnList.isEmpty())
                this->ui->tableView->hideColumn(i);
            else
                this->ui->tableView->showColumn(i);
        }
    }
}

void AppWidget::getAppInfo()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    QSettings settings;
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        //apps
        while (!indexList.isEmpty())
        {
            App app = this->appModel->getApp(this->appSortModel->mapToSource(indexList.takeFirst()).row());
            settings.remove("apps/"+app.packageName);
        }
    }
    else if (this->ui->comboBoxApps->currentIndex() == 1)
    {
        //backups
        while (!indexList.isEmpty())
        {
            Backup backup = this->backupModel->getBackup(this->backupSortModel->mapToSource(indexList.takeFirst()).row());
            settings.remove("apps/"+backup.packageName);
        }
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        //system apps
        while (!indexList.isEmpty())
        {
            App app = this->systemAppModel->getApp(this->systemAppSortModel->mapToSource(indexList.takeFirst()).row());
            settings.remove("apps/"+app.packageName);
        }
    }
    this->refreshApps();
}

void AppWidget::gotAllApps(QThread * thread)
{
    qDebug()<<"Apps::gotAllApps START";
    this->ui->progressApps->hide();
    emit this->progressFinished();
    QSettings settings;
    if ((thread == &this->threadApps) && this->firstGetApps)
    {
        if ((ui->comboBoxApps->currentIndex() == 0)
            && (settings.value("getAppInfo", 0).toInt() == 0))
        {
            this->comboBoxAppsChanged();
        }
        this->firstGetApps = false;
    }
    this->ui->tableView->resizeColumnsToContents();
    this->ui->tableView->resizeRowsToContents();
    this->ui->comboBoxApps->setEnabled(true);
    this->ui->tableView->setEnabled(true);
    this->ui->editFilter->setEnabled(true);
    this->ui->comboFilter->setEnabled(true);

    this->ui->tableView->setSelectionMode(QTableView::ExtendedSelection);
    this->ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    connect(this->ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(appsSelectedCount()));

    this->ui->tableView->selectRow(0);
    this->ui->tableView->setSortingEnabled(true);
    this->ui->tableView->sortByColumn(settings.value("sortByColumn", 1).toInt(), Qt::AscendingOrder);

    int i;
    int rows = this->ui->tableView->model()->rowCount(QModelIndex());
    for (i = 0; i < rows; i++)
        this->ui->tableView->setRowHeight(i, 40);
//    if (thread == &this->threadApps)
//    {
//        QStringList appColumnList = settings.value("appColumnList").toStringList();
//        int col = this->appModel->columnCount(QModelIndex());
//        for (i = 0; i < col; i++)
//        {
//            if (!appColumnList.contains(QString::number(i)) && !appColumnList.isEmpty())
//                this->ui->tableView->hideColumn(i);
//            else
//                this->ui->tableView->showColumn(i);
//        }
//    }
    appsSelectedCount();
    qDebug()<<"Apps::gotAllApps END";
}

void AppWidget::insertApp(App app)
{
//    QSettings settings;
//    settings.beginGroup("apps/"+packageName);

//    App app;

//    app.appName = settings.value("appName").toString();
//    app.appSize = settings.value("size").toString();
//    QString tmp=settings.value("version").toString();
//    if (tmp.length()>15)
//        tmp=tmp.left(15)+"...";
//    app.appVersion = tmp;
//    app.packageName = packageName;
//    app.appFile = settings.value("filePath").toString();
//    app.location = settings.value("location").toString();
    QSettings settings;
//    if (settings.value("getQR", false).toBool())
//    {
//        app.qrCode = appInfo::getQR(app.packageName);
//    }
    qDebug()<<"Apps insertApp - START";
    QByteArray ba;
    ba = settings.value("apps/" +app.packageName +"/icon").toByteArray();
    QPixmap pix;
    pix.loadFromData(ba);
    QIcon icon(pix);
    app.appIcon = icon;
    settings.endGroup();

    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        this->appModel->insertApp(0, app);
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        this->systemAppModel->insertApp(0, app);
    }

    this->ui->tableView->resizeColumnsToContents();
    this->ui->tableView->resizeRowsToContents();
    qDebug()<<"Apps insertApp - END";
}

void AppWidget::missingAapt()
{
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, tr("Missing aapt"), tr("QtADB did not found aapt.\nDownload it and place in one directory with adb.\nAfter You place it in correct directory click Refresh button"));
    QPushButton *download = msgBox->addButton(tr("Download", "missing aapt message button"), QMessageBox::AcceptRole);
    QPushButton *closeMsg = msgBox->addButton(tr("Cancel", "missing aapt message button"), QMessageBox::RejectRole);

    msgBox->exec();

    if (msgBox->clickedButton() == download)
    {
        QDesktopServices::openUrl(QUrl("http://qtadb.wordpress.com/download/"));
        QDesktopServices::openUrl(QUrl("file:///"+this->sdk));
    }
    delete closeMsg;
    delete download;
    delete msgBox;
}

void AppWidget::on_toolButtonBackup_pressed()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    QList<App> appList;
    AppTableModel *tmpModel;
    AppSortModel *sortModel;
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        tmpModel = this->appModel;
        sortModel = this->appSortModel;
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        tmpModel = this->systemAppModel;
        sortModel = this->systemAppSortModel;
    }
    else
    {
        this->withData=false;
        return;
    }

    while (!indexList.isEmpty())
    {
        appList.append(tmpModel->getApp(sortModel->mapToSource(indexList.takeFirst()).row()));
    }
//    QString tmp=this->phone->getPath();
//    this->phone->setPath("/");
//    this->phone->makeDir("sdcard/QtADB/backup");
//    this->phone->setPath(tmp);
//    AppList selected;
//    for (int i=0;i<ui->tableWidgetApps->selectedItems().count();i++)
//    {
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==1)
//            selected.name.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==2)
//            selected.version.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==3)
//            selected.size.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==5)
//            selected.filename.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==4)
//            selected.package.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//    }
//    if (selected.name.length()==0)
//    {
//        this->withData=false;
//        return;
//    }

    if (QMessageBox::question(this,tr("backup"),
                              tr("are you sure???"),QMessageBox::Ok | QMessageBox::No)== QMessageBox::No)
        return;


    if (!this->withApk && !this->withData)
    {
        QSettings settings;
        int mode;
        mode = settings.value("BackupDefaultAction", appDialog::AppAndData).toInt();
        if (mode == appDialog::Application)
        {
            this->withApk = true;
            this->withData = false;
        }
        else if (mode == appDialog::Data)
        {
            this->withApk = false;
            this->withData = true;
        }
        else if (mode == appDialog::AppAndData)
        {
            this->withApk = true;
            this->withData = true;
        }
    }
    if (this->appsDialog != NULL)
        delete this->appsDialog;
    if (this->withApk)
    {
        if (this->withData)
        {
            this->appsDialog=new appDialog(this,appList,appDialog::Backup,appDialog::AppAndData);
        }
        else
        {
            this->appsDialog=new appDialog(this,appList,appDialog::Backup,appDialog::Application);
        }
    }
    else if (this->withData)
    {
        this->appsDialog=new appDialog(this,appList,appDialog::Backup,appDialog::Data);
    }
    this->withData=false;
    this->withApk = false;
    this->appsDialog->show();
    connect(this->appsDialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->appsDialog, SIGNAL(closed()), this, SIGNAL(progressFinished()));
    connect(this->appsDialog,SIGNAL(closed()),this,SLOT(refreshApps()));
}

void AppWidget::on_toolButtonInstall_pressed()
{
    QStringList filesToInstall;
    filesToInstall = QFileDialog::getOpenFileNames(this, tr("select apps to install"),".", "Apk file (*.apk)");
    if (filesToInstall.length() == 0)
        return;
    QList<App> appList;
    App *app = NULL;
    while (!filesToInstall.isEmpty())
    {
        app = this->getAppInfo(filesToInstall.takeFirst());
        if (app != NULL)
            appList.append(*app);
    }
    if (this->appsDialog != NULL)
        delete this->appsDialog;
    this->appsDialog=new appDialog(this,appList,appDialog::Install,appDialog::None);
    this->appsDialog->show();
    connect(this->appsDialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->appsDialog, SIGNAL(closed()), this, SIGNAL(progressFinished()));
    connect(this->appsDialog,SIGNAL(closed()),this,SLOT(refreshApps()));
}

void AppWidget::on_toolButtonUninstall_pressed()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    QList<App> appList;
    AppTableModel *tmpModel;
    AppSortModel *sortModel;
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        tmpModel = this->appModel;
        sortModel = this->appSortModel;
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        tmpModel = this->systemAppModel;
        sortModel = this->systemAppSortModel;
    }
    else
    {
        this->withData=false;
        return;
    }

    while (!indexList.isEmpty())
    {
        appList.append(tmpModel->getApp(sortModel->mapToSource(indexList.takeFirst()).row()));
    }
//    AppList selected;
//    for (int i=0;i<ui->tableWidgetApps->selectedItems().count();i++)
//    {
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==1)
//            selected.name.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==2)
//            selected.version.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==3)
//            selected.size.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==5)
//            selected.filename.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==4)
//            selected.package.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//    }
//    if (selected.name.length()==0)
//        return;

    if (QMessageBox::question(this,tr("uninstall"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    if (this->appsDialog != NULL)
        delete this->appsDialog;
    this->appsDialog=new appDialog(this,appList,appDialog::Uninstall,appDialog::AppAndData);
//    if (this->ui->comboBoxApps->currentIndex() == 2)
//        this->appsDialog->system = true;
//    else
//        this->appsDialog->system = false;
    this->appsDialog->show();
    connect(this->appsDialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->appsDialog, SIGNAL(closed()), this, SIGNAL(progressFinished()));
    connect(this->appsDialog,SIGNAL(closed()),this,SLOT(refreshApps()));
}

void AppWidget::on_toolButtonRemoveBackup_pressed()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    QList<Backup> backupList;

    while (!indexList.isEmpty())
    {
        backupList.append(this->backupModel->getBackup(this->backupSortModel->mapToSource(indexList.takeFirst()).row()));
    }
//    QStringList selected;
//    for (int i=0;i<ui->tableWidgetApps->selectedItems().count();i++)
//    {
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==4)
//            selected.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//    }

    if (QMessageBox::question(this,tr("remove backup"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    QString oldPath=this->phone->getPath();
    this->phone->setPath("/sdcard/QtADB/backup/");
    while (backupList.length()>0)
    {
        this->phone->remove(backupList.first().packageName+".png");
        this->phone->remove(backupList.first().packageName+".txt");
        this->phone->remove(backupList.first().packageName+".apk");
        this->phone->remove(backupList.first().packageName+".DATA.tar.gz");
        backupList.removeFirst();
    }
    this->phone->setPath(oldPath);
    refreshApps();
}

void AppWidget::on_toolButtonRestore_pressed()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);
    QList<Backup> backupList;

    while (!indexList.isEmpty())
    {
        backupList.append(this->backupModel->getBackup(this->backupSortModel->mapToSource(indexList.takeFirst()).row()));
    }
    if (backupList.isEmpty())
        return;
    Backup backup;
    App app;
    QList<App> appList;
    while (!backupList.isEmpty())
    {
        backup = backupList.takeFirst();
        app.appIcon = backup.appIcon;
        app.appName = backup.appName;
        app.appSize = backup.appSize;
        app.appVersion = backup.appVersion;
        app.packageName = backup.packageName;
        appList.append(app);
    }
//    AppList selected;
//    for (int i=0;i<ui->tableWidgetApps->selectedItems().count();i++)
//    {
//        if (ui->tableWidgetApps->selectedItems().at(i)->column()==4)
//            selected.package.append(ui->tableWidgetApps->selectedItems().at(i)->text());
//    }
//    if (selected.package.length()==0)
//        return;

    if (QMessageBox::question(this,tr("restore"),tr("are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    if (!this->withApk && !this->withData)
    {
        QSettings settings;
        int mode;
        mode = settings.value("RestoreDefaultAction", appDialog::AppAndData).toInt();
        if (mode == appDialog::Application)
        {
            this->withApk = true;
            this->withData = false;
        }
        else if (mode == appDialog::Data)
        {
            this->withApk = false;
            this->withData = true;
        }
        else if (mode == appDialog::AppAndData)
        {
            this->withApk = true;
            this->withData = true;
        }
    }
    if (this->appsDialog != NULL)
        delete this->appsDialog;
    if (this->withApk)
    {
        if (this->withData)
        {
            this->appsDialog=new appDialog(this,appList,appDialog::Restore,appDialog::AppAndData);
        }
        else
        {
            this->appsDialog=new appDialog(this,appList,appDialog::Restore,appDialog::Application);
        }
    }
    else if (this->withData)
    {
        this->appsDialog=new appDialog(this,appList,appDialog::Restore,appDialog::Data);
    }

    this->withData=false;
    this->withApk = false;
    this->appsDialog->show();
    connect(this->appsDialog, SIGNAL(progressValue(int,int)), this, SIGNAL(progressValue(int,int)));
    connect(this->appsDialog, SIGNAL(closed()), this, SIGNAL(progressFinished()));
    connect(this->appsDialog,SIGNAL(closed()),this,SLOT(refreshApps()));
}

void AppWidget::refreshApps()
{
    this->ui->toolButtonBackup->setDown(false);
    this->ui->toolButtonRemoveBackup->setDown(false);
    this->ui->toolButtonRestore->setDown(false);
    this->ui->toolButtonUninstall->setDown(false);
    this->ui->buttonRefreshApps->setDown(false);
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        this->appModel->clear();
    }
    else if (this->ui->comboBoxApps->currentIndex() == 1)
    {
        this->backupModel->clear();
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        this->systemAppModel->clear();
    }
    this->comboBoxAppsChanged();
}

void AppWidget::insertBackup(Backup backup)
{
    QSettings settings;
//    Backup backup;
    QByteArray ba;
    ba = settings.value("apps/"+backup.packageName+"/icon").toByteArray();
    QPixmap pix;
    pix.loadFromData(ba);
    backup.appIcon = QIcon(pix);
//    backup.appName = appName;
//    backup.appVersion = appVersion;
//    backup.appSize = appSize;
//    backup.packageName = packageName;
//    backup.withData = withData;
//    backup.withApk = withApk;
    this->backupModel->insertBackup(0, backup);
    this->ui->tableView->resizeColumnsToContents();
    this->ui->tableView->resizeRowsToContents();
}



void ThreadBackups::run()
{
    QProcess *proces = new QProcess;
    QSettings settings;
    QString output, tmp;
    QStringList outputLines, txtLines;
    Backup backupFound;
    int i;

    proces->start("\"" + this->sdk + "\"adb shell busybox ls /sdcard/QtADB/backup/*.txt");

    proces->waitForFinished(-1);
    output = proces->readAll();
    output.remove(QString("%1[0m").arg( QChar( 0x1b )));
    output.remove(QChar( 0x1b ), Qt::CaseInsensitive);
    output.remove(QRegExp("\\[\\d;\\d+m"));
    outputLines = output.split("\n", QString::SkipEmptyParts);
    emit this->maximum(outputLines.size());
    int max = outputLines.size();
    i = 0;
    while (outputLines.size() > 0)
    {
        i++;
        emit this->value(i);
        emit this->progressValue(i, max);
        backupFound.packageName = outputLines.takeFirst();
        backupFound.packageName.remove(QRegExp("^.+/"));
        backupFound.packageName.remove(QRegExp("\\.txt\\s+$"));
        proces->start("\"" + this->sdk + "\"adb shell cat /sdcard/QtADB/backup/"+backupFound.packageName+".txt");
        proces->waitForFinished(-1);
        output = proces->readAll();
        if (!settings.contains("apps/"+backupFound.packageName+"/icon"))
        {
            proces->start("\"" + this->sdk + "\"adb pull /sdcard/QtADB/backup/"+backupFound.packageName+".png "+QDir::currentPath()+"/icons/"+backupFound.packageName+".png");
            proces->waitForFinished(-1);

            QFile icon(QDir::currentPath()+"/icons/"+backupFound.packageName+".png");
            icon.open(QIODevice::ReadOnly);
            QByteArray ba;
            ba = icon.readAll();
            settings.setValue("apps/"+backupFound.packageName+"/icon", ba); //- zapisanie pixmap w QSettings
            QFile::remove(QDir::currentPath()+"/icons/"+backupFound.packageName+".png");
        }
//        QByteArray ba;
//        ba = settings.value("apps/"+backupFound.packageName+"/icon").toByteArray();
//        QPixmap pix;
//        pix.loadFromData(ba);
//        backupFound.appIcon = QIcon(pix);

        if (output.contains("No such file or directory"))
            break;
        txtLines = output.split("\n", QString::SkipEmptyParts);
        while (txtLines.size() > 0)
        {
            tmp = txtLines.takeFirst();
            if (tmp.contains("app.name"))
            {
                tmp.remove("app.name=");
                tmp.remove(QRegExp("\\s+$"));
                backupFound.appName = QString::fromUtf8(tmp.toAscii());
            }
            else if (tmp.contains("app.size"))
            {
                tmp.remove("app.size=");
                tmp.remove(QRegExp("\\s+$"));
                backupFound.appSize = tmp;
            }
            else if (tmp.contains("app.version"))
            {
                tmp.remove("app.version=");
                tmp.remove(QRegExp("\\s+$"));
                backupFound.appVersion = tmp;
            }
        }
        proces->start("\"" + this->sdk + "\"adb shell ls  /sdcard/QtADB/backup/"+backupFound.packageName+".apk");
        proces->waitForFinished(-1);
        output = proces->readAll();
        output.remove(QString("%1[0m").arg( QChar( 0x1b )));
        output.remove(QChar( 0x1b ), Qt::CaseInsensitive);
        output.remove(QRegExp("\\[\\d;\\d+m"));
        if (output.contains("No such file or directory"))
            backupFound.withApk = false;
        else
            backupFound.withApk = true;
        proces->start("\"" + this->sdk + "\"adb shell ls /sdcard/QtADB/backup/"+backupFound.packageName+".DATA.tar.gz");
        proces->waitForFinished(-1);
        output = proces->readAll();
        output.remove(QString("%1[0m").arg( QChar( 0x1b )));
        output.remove(QChar( 0x1b ), Qt::CaseInsensitive);
        output.remove(QRegExp("\\[\\d;\\d+m"));
        if (output.contains("No such file or directory"))
            backupFound.withData = false;
        else
            backupFound.withData = true;
        backupFound.appSize.remove(QRegExp("\\s+$"));
        emit this->gotBackup(backupFound);
    }
    delete proces;
    emit this->gotAllApps(this);
}

void ThreadApps::run()
{
    QList<App> appList;
    App app;

    QProcess proces;
    QProcess *aapt = new QProcess;
    QSettings settings;
    QString output, tmp, sdFolder;
    QStringList lines, split;
    aapt->setProcessChannelMode(QProcess::MergedChannels);
    aapt->start("\"" + this->sdk + "\"aapt");
    if (aapt->error() == QProcess::FailedToStart)
    {
        emit this->missingAapt();
        return;
    }
    delete aapt;
    if (this->systemApps)
    {
        proces.start("\"" + this->sdk + "\"adb shell busybox ls -l /system/app/*.apk");
        proces.waitForFinished(-1);
        output = proces.readAll();
        qDebug()<<"Get apps system - "<<output;
        lines = output.split("\n", QString::SkipEmptyParts);
        while (lines.size() > 0)
        {
            tmp = lines.takeFirst();
            split = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (split.size() > 7)
            {
                app.appSize = split.at(4);
                tmp = split.at(8);
                tmp.remove(QString("%1[0m").arg( QChar( 0x1b )));
                tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
                tmp.remove(QRegExp("\\[\\d;\\d+m"));
                tmp.remove("/system/app/");
                app.appFileName = tmp;
                app.appFile = "/system/app/" + tmp;
                app.location = "system";
                app.packageName = settings.value("apps/" + app.appFileName, "").toString();

                tmp = split.at(5) + split.at(6) + split.at(7);
                app.date = tmp;

                appList.append(app);
            }
        }
    }
    else
    {
        proces.start("\"" + this->sdk + "\"adb shell busybox ls -l /data/app/*.apk");
        proces.waitForFinished(-1);
        output = proces.readAll();
        qDebug()<<"Get apps data - "<<output;
        lines = output.split("\n", QString::SkipEmptyParts);
        while (lines.size() > 0)
        {
            tmp = lines.takeFirst();
            split = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (split.size() > 7)
            {
                app.appSize = split.at(4);
                tmp = split.at(8);
                tmp.remove(QString("%1[0m").arg( QChar( 0x1b )));
                tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
                tmp.remove(QRegExp("\\[\\d;\\d+m"));
                tmp.remove("/data/app/");
                app.appFileName = tmp;
                app.appFile = "/data/app/" + tmp;
                app.location = "internal";
                app.packageName = settings.value("apps/" + app.appFileName, "").toString();

                tmp = split.at(5) + split.at(6) + split.at(7);
                app.date = tmp;

                appList.append(app);
            }
        }
        proces.start("\"" + this->sdk + "\"adb shell busybox ls -l /data/app-private/*.apk");
        proces.waitForFinished(-1);
        output = proces.readAll();
        qDebug()<<"Get apps data - "<<output;
        lines = output.split("\n", QString::SkipEmptyParts);
        while (lines.size() > 0)
        {
            tmp = lines.takeFirst();
            split = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (split.size() > 7)
            {
                app.appSize = split.at(4);
                tmp = split.at(8);
                tmp.remove(QString("%1[0m").arg( QChar( 0x1b )));
                tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
                tmp.remove(QRegExp("\\[\\d;\\d+m"));
                tmp.remove("/data/app-private/");
                app.appFileName = tmp;
                app.appFile = "/data/app-private/" + tmp;
                app.location = "internal";
                app.packageName = settings.value("apps/" + app.appFileName, "").toString();

                tmp = split.at(5) + split.at(6) + split.at(7);
                app.date = tmp;

                appList.append(app);
            }
        }
        proces.start("\"" + this->sdk + "\"adb shell busybox ls -l /mnt/asec/*/*.apk");
        proces.waitForFinished(-1);
        output = proces.readAll();
        qDebug()<<"Get apps sd - "<<output;
        lines = output.split("\n", QString::SkipEmptyParts);
        while (lines.size() > 0)
        {
            tmp = lines.takeFirst();
            split = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if (split.size() > 7)
            {
                app.appSize = split.at(4);
                tmp = split.at(8);
                tmp.remove(QString("%1[0m").arg( QChar( 0x1b )));
                tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
                tmp.remove(QRegExp("\\[\\d;\\d+m"));
                tmp.remove("/mnt/asec/");
                app.appFile = "/mnt/asec/" + tmp;
                tmp.replace("/","-");
                app.appFileName = tmp;
                app.location = "sdcard";
                app.packageName = settings.value("apps/" + app.appFileName, "").toString();

                tmp = split.at(5) + split.at(6) + split.at(7);
                app.date = tmp;

                appList.append(app);
            }
        }
//        proces.start("\"" + this->sdk + "\"adb shell busybox mount");
//        proces.waitForFinished(-1);
//        tmp = proces.readAll();
//        qDebug()<<"Get apps mount - "<<tmp;
//        if (tmp.contains("ext"))
//        {
//            lines = tmp.split("\n", QString::SkipEmptyParts);
//            while (lines.size() > 0)
//            {
//                tmp = lines.takeFirst();
//                if (tmp.contains("ext") && tmp.contains("sd"))
//                {
//                    split = tmp.split(QRegExp("\\s+"),QString::SkipEmptyParts);
//                    if (split.size() > 2)
//                    {
//                        sdFolder = split.at(2);
        sdFolder = settings.value("sdFolder").toString();
        if (!sdFolder.isEmpty() && !sdFolder.contains(QRegExp("<.*>")))
        {
            if (!sdFolder.endsWith("/",Qt::CaseInsensitive))
                sdFolder.append("/");
            proces.start("\"" + this->sdk + "\"adb shell busybox ls -l "+ sdFolder + "/*/*.apk");
            proces.waitForFinished(-1);
            output.append(proces.readAll());
            qDebug()<<"Get apps sd - "<<output;
            lines = output.split("\n", QString::SkipEmptyParts);
            while (lines.size() > 0)
            {
                tmp = lines.takeFirst();
                split = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                if (split.size() > 7)
                {
                    app.appSize = split.at(4);
                    tmp = split.at(8);
                    tmp.remove(QString("%1[0m").arg( QChar( 0x1b )));
                    tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
                    tmp.remove(QRegExp("\\[\\d;\\d+m"));
                    tmp.remove(sdFolder);
                    app.appFileName = tmp;
                    app.appFile = sdFolder + tmp;
                    app.location = "sd-ext";
                    app.packageName = settings.value("apps/" + app.appFileName, "").toString();

                    tmp = split.at(5) + split.at(6) + split.at(7);
                    app.date = tmp;

                    appList.append(app);
                }
            }
//                    }
//                }
//            }
        }
    }
    emit this->maximum(appList.size());

    QProcess zip;
    QString temp;
    zip.setProcessChannelMode(QProcess::MergedChannels);
    zip.start("\""+sdk+"\""+"adb shell mkdir /sdcard/QtADB");
    zip.waitForFinished(-1);
    zip.start("\""+sdk+"\""+"adb shell mkdir /sdcard/QtADB/tmp");
    zip.waitForFinished(-1);
    settings.beginGroup("apps");
    QStringList settingsList=settings.childKeys();
    settings.endGroup();
    QStringList aaptLines,aaptLineParts;
    QDir dir(QDir::currentPath()+"/icons/");
    QFileInfoList fileInfoList=dir.entryInfoList();
    QStringList fileNameList,fileTmpList;
    while (fileInfoList.length()>0)
        fileNameList.append(fileInfoList.takeFirst().fileName());

    dir.setPath(QDir::currentPath()+"/tmp/");
    qDebug()<<"Apps START 'for'";
    for (int i=0; i<appList.size(); i++)
    {
        emit this->value(i+1);
        emit this->progressValue(i + 1, appList.size());
        app = appList.at(i);
        if ((!settingsList.contains(app.appFileName)) ||
            (settings.value("apps/" + app.packageName + "/date", "").toString() != app.date))
        {
            qDebug()<<"Apps needs to pull apk";
            zip.start("\""+sdk+"\""+"adb pull "+app.appFile.toLatin1()+" \""+QDir::currentPath()+"/tmp/\""+app.appFileName);
            zip.waitForFinished(-1);
            temp = zip.readAll();
            qDebug()<<"Apps copy - "<<temp;
            if (temp.contains("does not exist") || temp.contains("Android Debug Bridge"))
                continue;
            zip.start("\""+sdk+"\"aapt d badging \""+QDir::currentPath()+"/tmp/\""+app.appFileName);
            zip.waitForReadyRead(-1);
            temp=zip.readAll();
            qDebug()<<"Apps aapt - "<<temp;
            zip.close();
            zip.terminate();
            aaptLines=temp.split("\n");
            qDebug()<<"Apps aapt decoding";
            while (aaptLines.length()>0)
            {
                aaptLineParts=aaptLines.first().split(QRegExp("\'\\s"));
                while (aaptLineParts.length()>0)
                {
                    if (aaptLineParts.first().contains(QRegExp("name=")) && app.packageName.isEmpty())
                    {
                        app.packageName=QString::fromUtf8(aaptLineParts.first().toAscii());
                        app.packageName.remove(0,app.packageName.indexOf("name=")+5);
                        app.packageName.remove("'");
                    }
                    else if (aaptLineParts.first().contains(QRegExp("^versionName=")) && app.appVersion.isEmpty())
                    {
                        app.appVersion=aaptLineParts.first();
                        app.appVersion.remove("versionName='");
                        app.appVersion.remove("'");
                    }
                    else if (aaptLineParts.first().contains(QRegExp("label="))&&app.appName.isEmpty())
                    {
                        app.appName=QString::fromUtf8(aaptLineParts.first().toAscii());
                        app.appName.remove(0,app.appName.indexOf("label=")+6);
                        app.appName.remove("'");
                    }
                    else if (aaptLineParts.first().contains(QRegExp("^icon="))&&app.icoName.isEmpty())
                    {
                        app.icoName=aaptLineParts.first();
                        app.icoName.remove("icon='");
                        app.icoName.remove("'");
                    }
                    aaptLineParts.removeFirst();
                }
                aaptLines.removeFirst();
            }
            qDebug()<<"Apps aapt decoded";
            settings.setValue("apps/"+app.packageName+"/icoName", app.icoName);
            settings.setValue("apps/"+app.packageName+"/appName", QString::fromUtf8(app.appName.toAscii()));
            settings.setValue("apps/"+app.packageName+"/version", app.appVersion);
            settings.setValue("apps/"+app.packageName+"/size", app.appSize);
            settings.setValue("apps/"+app.packageName+"/date", app.date);
        }
        else
        {
            app.appName = settings.value("apps/" + app.packageName + "/appName").toString();
            app.appVersion = settings.value("apps/" + app.packageName + "/version").toString();
            app.appSize = settings.value("apps/" + app.packageName + "/size").toString();
            app.icoName = settings.value("apps/" + app.packageName + "/icoName").toString();
        }
        if (settings.value("getCyrketVer", false).toBool() && app.location != "system")
        {
            qDebug()<<"Apps getting info from Cyrket";
            app.cyrketVer = appInfo::getCyrketVer(app.packageName);
            qDebug()<<"Apps got info from cyrket";
        }

        settings.setValue("apps/"+app.appFileName, app.packageName);
        settings.setValue("apps/"+app.packageName+"/filePath", app.appFile);
        settings.setValue("apps/"+app.packageName+"/location", app.location);
        temp=app.packageName;
        temp.append(".png");
        fileTmpList.clear();
        fileInfoList=dir.entryInfoList();
        while (fileInfoList.length()>0)
            fileTmpList.append(fileInfoList.takeFirst().fileName());
        if (!settings.contains("apps/"+app.packageName+"/icon"))
        {
            qDebug()<<"Apps there is missing icon i settings";
            if (!fileTmpList.contains(app.appFileName))
            {
                zip.start("\""+sdk+"\""+"adb pull "+app.appFile.toLatin1()+" \""+QDir::currentPath()+"/tmp/\""+app.appFileName);
                zip.waitForFinished(-1);
                QString out;
                out = zip.readAll();
            }
            FileWidget::unpack(QDir::currentPath()+"/tmp/"+app.appFileName,QDir::currentPath()+"/icons/",app.icoName,temp);

            QByteArray ba;
            QFile icon(QDir::currentPath()+"/icons/"+temp);
            icon.open(QIODevice::ReadWrite);
            ba = icon.readAll();
            settings.setValue("apps/"+app.packageName+"/icon", ba); //- zapisanie pixmap w QSettings
            icon.remove();
            qDebug()<<"Apps got icon now";
        }
//        QByteArray ba;
//        ba = settings.value("apps/"+app.packageName+"/icon").toByteArray();
//        QPixmap pix;
//        pix.loadFromData(ba);
//        QIcon icon(pix);
//        app.appIcon = icon;

        QFile::remove(QDir::currentPath()+"/tmp/"+app.appFileName);

        qDebug()<<"Apps got app name - "<<settings.value("apps/"+app.packageName+"/appName").toString();
        qDebug()<<"Apps got app file - "<<settings.value("apps/"+app.packageName+"/filePath").toString();
        qDebug()<<"Apps got app icon - "<<settings.value("apps/"+app.packageName+"/icoName").toString();
        qDebug()<<"Apps got app location - "<<settings.value("apps/"+app.packageName+"/location").toString();
        qDebug()<<"Apps got app size - "<<settings.value("apps/"+app.packageName+"/size").toString();
        qDebug()<<"Apps got app version - "<<settings.value("apps/"+app.packageName+"/version").toString();
        emit gotApp(app);
        if (i == (appList.length()-1))
            emit gotAllApps(this);
    }
    emit gotAllApps(this);
}

void AppWidget::toolButtonBackupApp()
{
    this->withData=false;
    this->withApk = true;
    on_toolButtonBackup_pressed();
}

void AppWidget::toolButtonBackupData()
{
    this->withData = true;
    this->withApk = false;
    on_toolButtonBackup_pressed();
}

void AppWidget::toolButtonBackupAppAndData()
{
    this->withData = true;
    this->withApk = true;
    on_toolButtonBackup_pressed();
}

void AppWidget::toolButtonInstallOnSd()
{

}

void AppWidget::toolButtonRestoreApp()
{
    this->withData=false;
    this->withApk = true;
    on_toolButtonRestore_pressed();
}

void AppWidget::toolButtonRestoreData()
{
    this->withData = true;
    this->withApk = false;
    on_toolButtonRestore_pressed();
}

void AppWidget::toolButtonRestoreAppAndData()
{
    this->withData = true;
    this->withApk = true;
    on_toolButtonRestore_pressed();
}

App * AppWidget::getAppInfo(QString filePath)
{
    QProcess *proces = new QProcess;
    QString temp;
    QStringList aaptLines, aaptLineParts;
    QSettings settings;
    QByteArray ba;
    QPixmap pix;

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

    return app;
}

bool AppWidget::unpack(QString inFile,QString outPath,QString fileToUnpack,QString outName)
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


quint32 AppWidget::qbytearrayToInt32(QByteArray array)
{
    QDataStream stream(array);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint32 x;
    stream>>x;
    return x;
}

quint16 AppWidget::qbytearrayToInt16(QByteArray array)
{
    QDataStream stream(array);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint16 x;
    stream>>x;
    return x;
}

void AppWidget::openMarket()
{
    QString sdk;
    QSettings settings;
    sdk = settings.value("sdkPath").toString();
    QProcess proc;
    proc.start("\"" + sdk + "\"adb shell am start -a android.intent.action.VIEW -d market://details?id="
               + this->ui->editAppsPackageName->text() + " -n com.android.vending/.AssetInfoActivity");
    proc.waitForFinished(-1);
}

void AppWidget::openAppBrain()
{
    QDesktopServices::openUrl(QUrl("http://market.android.com/details?id=" + this->ui->editAppsPackageName->text()));
}

void AppWidget::getCyrketVersions()
{
    QModelIndexList indexList = this->ui->tableView->selectionModel()->selectedRows(1);

    int i;
    this->ui->progressApps->setMaximum(indexList.size());
    this->ui->progressApps->setValue(0);
    this->ui->progressApps->show();
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        i = 1;
        while (!indexList.isEmpty())
        {
            this->ui->progressApps->setValue(i);
            i++;
            int row = this->appSortModel->mapToSource(indexList.takeFirst()).row();
            App app = this->appModel->getApp(row);
            app.cyrketVer = appInfo::getCyrketVer(app.packageName);
            this->appModel->setCyrketVer(row, app.cyrketVer);
        }
        this->ui->progressApps->setMaximum(0);
        this->ui->progressApps->setValue(0);
        this->ui->progressApps->hide();
        QList<App> appList = this->appModel->getList();
        this->appModel->clear();
        this->appModel->insertApps(0, appList);

        this->ui->tableView->selectRow(0);
        this->ui->tableView->setSortingEnabled(true);
        QSettings settings;
        this->ui->tableView->sortByColumn(settings.value("sortByColumn", 1).toInt(), Qt::AscendingOrder);

        int i;
        int rows = this->ui->tableView->model()->rowCount(QModelIndex());
        for (i = 0; i < rows; i++)
            this->ui->tableView->setRowHeight(i, 40);
        appsSelectedCount();
    }
}

void AppWidget::filter()
{
    QRegExp regExp(this->ui->editFilter->text(), Qt::CaseInsensitive, QRegExp::RegExp);

    int key = this->ui->comboFilter->itemData(this->ui->comboFilter->currentIndex()).toInt();
    if (this->ui->comboBoxApps->currentIndex() == 0)
    {
        this->appSortModel->setFilterKeyColumn(key);
        this->appSortModel->setFilterRegExp(regExp);
    }
    else if (this->ui->comboBoxApps->currentIndex() == 1)
    {
        this->backupSortModel->setFilterKeyColumn(key);
        this->backupSortModel->setFilterRegExp(regExp);
    }
    else if (this->ui->comboBoxApps->currentIndex() == 2)
    {
        this->systemAppSortModel->setFilterKeyColumn(key);
        this->systemAppSortModel->setFilterRegExp(regExp);
    }
    this->ui->tableView->resizeColumnsToContents();
    this->ui->tableView->resizeRowsToContents();

}
