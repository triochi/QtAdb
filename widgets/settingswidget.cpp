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


#include "settingswidget.h"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
#ifdef Q_OS_WIN
    this->ui->buttonAssociate->show();
#else
    this->ui->buttonAssociate->hide();
#endif
    this->appsColumnModel = new QStandardItemModel;
    this->backupsColumnModel = new QStandardItemModel;
    this->computerColumnModel = new QStandardItemModel;
    this->phoneColumnModel = new QStandardItemModel;

    this->setLayout(this->ui->gridLayout_7);
    this->ui->pageSettingsGeneral->setLayout(this->ui->layoutGeneralSettings);
    this->ui->pageSettingsFile->setLayout(this->ui->layoutFileSettings);
    this->ui->pageSettingsApp->setLayout(this->ui->layoutSettingsApps);
    this->ui->groupComputer->setLayout(this->ui->gridComputer);
    this->ui->groupPhone->setLayout(this->ui->gridPhone);
    ui->groupBoxBoth->setLayout(ui->gridBoth);
    ui->pageSettingsShell->setLayout(ui->shellGridLayout);
    ui->pageSettingsAnimations->setLayout(ui->layoutAnimationsSettings);
    this->currentIndex=0;

    connect(this->ui->buttonSaveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(this->ui->buttonFontApp, SIGNAL(clicked()), this, SLOT(chooseFontApp()));
    connect(this->ui->buttonFontTables, SIGNAL(clicked()), this, SLOT(chooseFontTables()));
    connect(this->ui->buttonFontShell, SIGNAL(clicked()), this, SLOT(chooseFontShell()));
    connect(this->ui->buttonDefaults, SIGNAL(clicked()), this, SLOT(setSettingsToDefaults()));
    connect(this->ui->radioComputerPath, SIGNAL(toggled(bool)), this, SLOT(settingsSlotChanged()));
    connect(this->ui->radioPhonePath, SIGNAL(toggled(bool)), this, SLOT(settingsSlotChanged()));
    connect(this->ui->checkAppName, SIGNAL(toggled(bool)), this, SLOT(settingsSlotChanged()));
    connect(this, SIGNAL(settingsChanged()), this, SLOT(getSettings()));
    connect(this, SIGNAL(settingsChanged()), this, SLOT(changeFont()));
    connect(this->ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changePage(int)));
    connect(&this->animation.animation, SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(this->ui->buttonDetectSdExt, SIGNAL(clicked()), this, SLOT(detectSdExtFolder()));

    ui->listWidgetCurvePicker->setIconSize(QSize(64,64));

    createCurveIcons();

    connect(ui->listWidgetCurvePicker, SIGNAL(currentRowChanged(int)), this, SLOT(curveChanged(int)));

    this->getSettings();
    this->changeFont();
}

void SettingsWidget::animationFinished()
{
    this->ui->stackedSettings->setCurrentIndex(this->currentIndex);
}

void SettingsWidget::changePage(int page)
{
    int previous=this->currentIndex;
    this->currentIndex=page;
    animation.start(ui->stackedSettings->widget(page),ui->stackedSettings->widget(previous));
}

void SettingsWidget::createCurveIcons()
{
    QPixmap pix(QSize(64,64));
    QPainter painter(&pix);
    QLinearGradient gradient(0,0, 0, 64);
    gradient.setColorAt(0.0, QColor(240, 240, 240));
    gradient.setColorAt(1.0, QColor(224, 224, 224));
    QBrush brush(gradient);
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    // Skip QEasingCurve::Custom
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        painter.fillRect(QRect(QPoint(0, 0), QSize(64,64)), brush);
        QEasingCurve curve((QEasingCurve::Type)i);
        painter.setPen(QColor(0, 0, 255, 64));
        qreal xAxis = 64/1.5;
        qreal yAxis = 64/3;
        painter.drawLine(0, xAxis, 64,  xAxis);
        painter.drawLine(yAxis, 0, yAxis, 64);

        qreal curveScale = 64/2;

        painter.setPen(Qt::NoPen);

        // start point
        painter.setBrush(Qt::red);
        QPoint start(yAxis, xAxis - curveScale * curve.valueForProgress(0));
        painter.drawRect(start.x() - 1, start.y() - 1, 3, 3);

        // end point
        painter.setBrush(Qt::blue);
        QPoint end(yAxis + curveScale, xAxis - curveScale * curve.valueForProgress(1));
        painter.drawRect(end.x() - 1, end.y() - 1, 3, 3);

        QPainterPath curvePath;
        curvePath.moveTo(start);
        for (qreal t = 0; t <= 1.0; t+=1.0/curveScale) {
            QPoint to;
            to.setX(yAxis + curveScale * t);
            to.setY(xAxis - curveScale * curve.valueForProgress(t));
            curvePath.lineTo(to);
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.strokePath(curvePath, QColor(32, 32, 32));
        painter.setRenderHint(QPainter::Antialiasing, false);
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(pix));
        item->setText(metaEnum.key(i));
        ui->listWidgetCurvePicker->addItem(item);
    }
}

void SettingsWidget::curveChanged(int row)
{
    this->animationCurve=row;
}

SettingsWidget::~SettingsWidget()
{
    QSettings *settings = new QSettings;
    if (this->rememberPhonePath)
        settings->setValue("phonePath", this->phonePath);
    if (this->rememberComputerPath)
        settings->setValue("computerPath", this->computerPath);

    //settings->setValue("sdkPath", this->sdkPath);
    //settings->setValue("wlanIP", this->wlanIP);

    delete settings;
    delete ui;
}

void SettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        this->setSettings();
        break;
    default:
        break;
    }
}

void SettingsWidget::setSettingsToDefaults()
{
    if (QMessageBox::question(this,tr("Reset settings??"),tr("Are you sure you want restore settings to defaults???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;
    this->alwaysCloseCopy = true;
    this->phoneHiddenFiles = true;
    this->computerHiddenFiles = true;
    this->saveWindowPosition = true;
    this->rememberComputerPath = true;
    this->rememberPhonePath = true;
    this->showComputerColumnsHeaders = true;
    this->showPhoneColumnsHeaders = true;
    this->dialogKopiujShowModal = true;
    this->killDemonOnExit = false;
    this->checkForUpdatesOnStart = true;
    this->color = true;
    this->colorShellFiles = true;
    this->showAppIcon = false;
    this->showAppName = false;
    this->getQR = false;
    this->getCyrketVer = false;
    this->showCopyConfirmation = true;

    this->phonePath = "/";
    this->computerPath = "/";
    this->showAppNameConfig = "<appName>";

    this->sortByColumn = 1;
    this->getAppInfo = 0;
    this->getSystemAppInfo = 0;
    this->getBackupInfo = 0;
    this->BackupDefaultAction = appDialog::AppAndData;
    this->RestoreDefaultAction = appDialog::AppAndData;
    this->layoutDirection = 0;

    this->fontApp = qApp->font();
    this->fontTables = qApp->font();
    this->fontShell = qApp->font();

    this->toolBarHideButton = true;
    this->toolBarIconSize = QSize(32, 32);
    this->toolBarStyle = Qt::ToolButtonTextUnderIcon;
    this->toolBarArea = Qt::LeftToolBarArea;

    this->appColumnList.clear();
    this->backupColumnList.clear();
    this->computerColumnList.clear();
    this->phoneColumnList.clear();

    this->setSettings();
}

void SettingsWidget::settingsSlotChanged()
{
    if (this->ui->radioComputerPath->isChecked())
        this->ui->editComputerPath->setEnabled(true);
    else
        this->ui->editComputerPath->setDisabled(true);

    if (this->ui->radioPhonePath->isChecked())
        this->ui->editPhonePath->setEnabled(true);
    else
        this->ui->editPhonePath->setDisabled(true);

    if (this->ui->checkAppName->isChecked())
        this->ui->editAppName->setEnabled(true);
    else
        this->ui->editAppName->setDisabled(true);
}

void SettingsWidget::on_buttonRemoveSettings_pressed()
{
    QSettings settings;
    settings.clear();

    this->clearSettings=true;
}

void SettingsWidget::saveSettings()
{
    this->clearSettings=false;
    QSettings * settings = new QSettings;
    if (this->ui->radioPhonePath->isChecked())
        settings->setValue("phonePath", this->ui->editPhonePath->text());
    if (this->ui->radioComputerPath->isChecked())
        settings->setValue("computerPath", this->ui->editComputerPath->text());
    if (this->rememberPhonePath)
        settings->setValue("phonePath", this->phonePath);
    if (this->rememberComputerPath)
        settings->setValue("computerPath", this->computerPath);
    settings->setValue("alwaysCloseCopy", this->ui->checkAlwaysCloseCopy->isChecked());
    settings->setValue("phoneHiddenFiles", this->ui->checkPhoneHidden->isChecked());
    settings->setValue("computerHiddenFiles", this->ui->checkComputerHidden->isChecked());
    settings->setValue("saveWindowPosition", this->ui->checkWindowPosition->isChecked());
    settings->setValue("rememberComputerPath", this->ui->radioComputerRemPath->isChecked());
    settings->setValue("rememberPhonePath", this->ui->radioPhoneRemPath->isChecked());
//    settings->setValue("getQR", this->ui->checkGetQrcode->isChecked());
    settings->setValue("getCyrketVer", this->ui->checkCyrketVer->isChecked());
    settings->setValue("fontAppFamily", this->fontApp.family());
    settings->setValue("fontAppSize", this->fontApp.pointSize());
    settings->setValue("fontTablesFamily", this->fontTables.family());
    settings->setValue("fontTablesSize", this->fontTables.pointSize());
    settings->setValue("fontShellFamily", this->fontShell.family());
    settings->setValue("fontShellSize", this->fontShell.pointSize());
    settings->setValue("showComputerColumnsHeaders", this->ui->checkShowComputerHeaders->isChecked());
    settings->setValue("showPhoneColumnsHeaders", this->ui->checkShowPhoneHeaders->isChecked());
    settings->setValue("dialogKopiujShowModal",ui->checkBoxDialogKopiujShowModal->isChecked());
    settings->setValue("killDemonOnExit",ui->checkBoxKillDemonOnExit->isChecked());
    settings->setValue("checkForUpdatesOnStart", ui->checkBoxUpdatesStart->isChecked());
    settings->setValue("sortByColumn", ui->comboSort->currentIndex() + 1);
    settings->setValue("color",ui->checkBoxColorNever->isChecked());
    settings->setValue("colorShellFiles",ui->checkColorShellFiles->isChecked());
    settings->setValue("getAppInfo", ui->comboGetApps->currentIndex());
    settings->setValue("getSystemAppInfo", ui->comboGetSystemApps->currentIndex());
    settings->setValue("getBackupInfo", ui->comboGetBackups->currentIndex());
    settings->setValue("BackupDefaultAction", this->ui->comboActionBackup->currentIndex());
    settings->setValue("RestoreDefaultAction", this->ui->comboActionRestore->currentIndex());
    settings->setValue("showAppIcon", this->ui->checkAppIcon->isChecked());
    settings->setValue("showAppName", this->ui->checkAppName->isChecked());
    settings->setValue("showAppNameConfig", this->ui->editAppName->text());
    settings->setValue("enableAnimations", this->ui->checkBoxEnableAnimations->isChecked());
    settings->setValue("curve",this->ui->listWidgetCurvePicker->currentRow());
    settings->setValue("animationDuration", this->ui->spinBoxAnimationDuration->value());
    settings->setValue("animationDirection", this->ui->comboBoxAnimationDirection->currentIndex());
    settings->setValue("tooBarHideButton", this->ui->comboToolBarHideButton->currentIndex());
    settings->setValue("toolBarIconSize", this->ui->spinToolBarIconSize->value());
    settings->setValue("toolBarStyle", this->ui->comboToolBarStyle->currentIndex());
    settings->setValue("toolBarArea", this->ui->comboToolBarArea->currentIndex());
    settings->setValue("layoutDirection", this->ui->comboLayout->currentIndex());
    settings->setValue("shellBackgroundColor", this->ui->shellLabelCurrentBackground->palette().background().color());
    settings->setValue("shellFontColor", this->ui->shellLabelCurrentFont->palette().background().color());
    settings->setValue("sdFolder", this->ui->editSdExt->text());
    settings->setValue("showCopyConfirmation",ui->checkBoxShowCopyConfirmation->isChecked());
    int columns = this->appsColumnModel->rowCount(QModelIndex());
    QStringList columnsList;
    for (int i=0; i<columns; i++)
    {
        if (this->appsColumnModel->item(i)->checkState() == Qt::Checked)
            columnsList.append(QString::number(i));
    }
    settings->setValue("appColumnList", columnsList);

    columns = this->backupsColumnModel->rowCount(QModelIndex());
    columnsList.clear();
    for (int i=0; i<columns; i++)
    {
        if (this->backupsColumnModel->item(i)->checkState() == Qt::Checked)
            columnsList.append(QString::number(i));
    }
    settings->setValue("backupColumnList", columnsList);

    columns = this->computerColumnModel->rowCount(QModelIndex());
    columnsList.clear();
    for (int i=0; i<columns; i++)
    {
        if (this->computerColumnModel->item(i)->checkState() == Qt::Checked)
            columnsList.append(QString::number(i));
    }
    settings->setValue("computerColumnList", columnsList);

    columns = this->phoneColumnModel->rowCount(QModelIndex());
    columnsList.clear();
    for (int i=0; i<columns; i++)
    {
        if (this->phoneColumnModel->item(i)->checkState() == Qt::Checked)
            columnsList.append(QString::number(i));
    }
    settings->setValue("phoneColumnList", columnsList);


    if (settings->value("tooBarHideButton", 0).toInt() == 0)
        this->toolBarHideButton = true;
    else
        this->toolBarHideButton = false;
    int size = settings->value("toolBarIconSize", 32).toInt();
    this->toolBarIconSize = QSize(size, size);
    int value = settings->value("toolBarStyle", 3).toInt();
    switch(value)
    {
    case 0:
        this->toolBarStyle = Qt::ToolButtonIconOnly;
        break;
    case 1:
        this->toolBarStyle = Qt::ToolButtonTextOnly;
        break;
    case 2:
        this->toolBarStyle = Qt::ToolButtonTextBesideIcon;
        break;
    case 3:
        this->toolBarStyle = Qt::ToolButtonTextUnderIcon;
        break;
    }
    value = settings->value("toolBarArea", 0).toInt();
    switch(value)
    {
    case 0:
        this->toolBarArea = Qt::LeftToolBarArea;
        break;
    case 1:
        this->toolBarArea = Qt::RightToolBarArea;
        break;
    case 2:
        this->toolBarArea = Qt::TopToolBarArea;
        break;
    case 3:
        this->toolBarArea = Qt::BottomToolBarArea;
        break;
    }

    delete settings;
    emit this->settingsChanged();
}


void SettingsWidget::getSettings()
{
    QSettings * settings = new QSettings;
    this->alwaysCloseCopy = settings->value("alwaysCloseCopy", true).toBool();
    this->phoneHiddenFiles = settings->value("phoneHiddenFiles", true).toBool();
    this->computerHiddenFiles = settings->value("computerHiddenFiles", true).toBool();
    this->saveWindowPosition = settings->value("saveWindowPosition", true).toBool();
    this->rememberComputerPath = settings->value("rememberComputerPath", true).toBool();
    this->rememberPhonePath = settings->value("rememberPhonePath", true).toBool();
    this->showComputerColumnsHeaders = settings->value("showComputerColumnsHeaders", true).toBool();
    this->showPhoneColumnsHeaders = settings->value("showPhoneColumnsHeaders", true).toBool();
    this->dialogKopiujShowModal = settings->value("dialogKopiujShowModal", true).toBool();
    this->killDemonOnExit = settings->value("killDemonOnExit", false).toBool();
    this->checkForUpdatesOnStart = settings->value("checkForUpdatesOnStart", true).toBool();
    this->color = settings->value("color", true).toBool();
    this->colorShellFiles = settings->value("colorShellFiles", true).toBool();
    this->showAppIcon = settings->value("showAppIcon", false).toBool();
    this->showAppName = settings->value("showAppName", false).toBool();
    this->getQR = settings->value("getQR", false).toBool();
    this->getCyrketVer = settings->value("getCyrketVer", false).toBool();
    this->enableAnimations = settings->value("enableAnimations", true).toBool();
    this->animationCurve = settings->value("curve", int(QEasingCurve::OutBounce)).toInt();
    this->clearSettings = false;
    this->showCopyConfirmation = settings->value("showCopyConfirmation", true).toBool();

    this->phonePath = settings->value("phonePath", "/").toString();
    this->computerPath = settings->value("computerPath", QDir::drives().first().filePath()).toString();
    this->fontApp.setFamily(settings->value("fontAppFamily", qApp->font().family()).toString());
    this->fontTables.setFamily(settings->value("fontTablesFamily", qApp->font().family()).toString());
    this->fontShell.setFamily(settings->value("fontShellFamily", qApp->font().family()).toString());
    this->showAppNameConfig = settings->value("showAppNameConfig", "<appName>").toString();

    this->fontApp.setPointSize(settings->value("fontAppSize", qApp->font().pointSize()).toInt());
    this->fontTables.setPointSize(settings->value("fontTablesSize", qApp->font().pointSize()).toInt());
    this->fontShell.setPointSize(settings->value("fontShellSize", qApp->font().pointSize()).toInt());
    this->sortByColumn = settings->value("sortByColumn", 1).toInt();
    this->getAppInfo = settings->value("getAppInfo", 0).toInt();
    this->getSystemAppInfo = settings->value("getSystemAppInfo", 0).toInt();
    this->getBackupInfo = settings->value("getBackupInfo", 0).toInt();
    this->BackupDefaultAction = settings->value("BackupDefaultAction", appDialog::AppAndData).toInt();
    this->RestoreDefaultAction = settings->value("RestoreDefaultAction", appDialog::AppAndData).toInt();
    this->animationDuration = settings->value("animationDuration", 1000).toInt();
    this->animationDirection = settings->value("animationDirection", 0).toInt();

    this->shellBackgroundColor = settings->value("shellBackgroundColor", Qt::white).value<QColor>();
    this->shellFontColor = settings->value("shellFontColor", Qt::black).value<QColor>();

    this->windowGeometry = settings->value("windowGeometry").toByteArray();

//    this->phonePath = settings->value("phonePath", "/").toString();
//    this->computerPath = settings->value("computerPath", "/").toString();

    this->sdkPath = settings->value("sdkPath").toString();
    this->sdFolder = settings->value("sdFolder","empty").toString();
    if (this->sdFolder == "empty")
    {
        this->detectSdExtFolder();
        settings->setValue("sdFolder", this->sdFolder);
    }

    this->appColumnList = settings->value("appColumnList").toStringList();
    this->backupColumnList = settings->value("backupColumnList").toStringList();
    this->computerColumnList = settings->value("computerColumnList").toStringList();
    this->phoneColumnList = settings->value("phoneColumnList").toStringList();

    this->layoutDirection = settings->value("layoutDirection", 0).toInt();

    if (settings->value("tooBarHideButton", 0).toInt() == 0)
        this->toolBarHideButton = true;
    else
        this->toolBarHideButton = false;
    int size = settings->value("toolBarIconSize", 32).toInt();
    this->toolBarIconSize = QSize(size, size);
    int value = settings->value("toolBarStyle", 3).toInt();
    switch(value)
    {
    case 0:
        this->toolBarStyle = Qt::ToolButtonIconOnly;
        break;
    case 1:
        this->toolBarStyle = Qt::ToolButtonTextOnly;
        break;
    case 2:
        this->toolBarStyle = Qt::ToolButtonTextBesideIcon;
        break;
    case 3:
        this->toolBarStyle = Qt::ToolButtonTextUnderIcon;
        break;
    }
    value = settings->value("toolBarArea", 0).toInt();
    switch(value)
    {
    case 0:
        this->toolBarArea = Qt::LeftToolBarArea;
        break;
    case 1:
        this->toolBarArea = Qt::RightToolBarArea;
        break;
    case 2:
        this->toolBarArea = Qt::TopToolBarArea;
        break;
    case 3:
        this->toolBarArea = Qt::BottomToolBarArea;
        break;
    }

    delete settings;
    this->setSettings();
    this->animation.setPrameters(this->animationDuration,this->animationDirection,this->animationCurve);
    if (!this->enableAnimations)
        animation.setDuration(0);
}

void SettingsWidget::setSettings()
{
    this->ui->checkWindowPosition->setChecked(this->saveWindowPosition);
    this->ui->checkAlwaysCloseCopy->setChecked(this->alwaysCloseCopy);
    this->ui->checkPhoneHidden->setChecked(this->phoneHiddenFiles);
    this->ui->checkComputerHidden->setChecked(this->computerHiddenFiles);
    this->ui->radioComputerRemPath->setChecked(this->rememberComputerPath);
    this->ui->radioPhoneRemPath->setChecked(this->rememberPhonePath);
    this->ui->radioComputerPath->setChecked(!this->rememberComputerPath);
    this->ui->radioPhonePath->setChecked(!this->rememberPhonePath);
    this->ui->checkBoxDialogKopiujShowModal->setChecked(this->dialogKopiujShowModal);
    this->ui->checkBoxKillDemonOnExit->setChecked(this->killDemonOnExit);
    if (!this->ui->radioComputerRemPath->isChecked())
        this->ui->editComputerPath->setText(this->computerPath);
    if (!this->ui->radioPhoneRemPath->isChecked())
        this->ui->editPhonePath->setText(this->phonePath);
    this->ui->checkShowComputerHeaders->setChecked(this->showComputerColumnsHeaders);
    this->ui->checkShowPhoneHeaders->setChecked(this->showPhoneColumnsHeaders);
    this->ui->checkBoxUpdatesStart->setChecked(this->checkForUpdatesOnStart);
    this->ui->comboSort->setCurrentIndex(this->sortByColumn - 1);
    this->ui->checkBoxColorNever->setChecked(this->color);
    this->ui->comboGetApps->setCurrentIndex(this->getAppInfo);
    this->ui->comboGetSystemApps->setCurrentIndex(this->getSystemAppInfo);
    this->ui->comboGetBackups->setCurrentIndex(this->getBackupInfo);
    this->ui->comboActionBackup->setCurrentIndex(this->BackupDefaultAction);
    this->ui->comboActionRestore->setCurrentIndex(this->RestoreDefaultAction);
    this->ui->checkAppIcon->setChecked(this->showAppIcon);
    this->ui->checkAppName->setChecked(this->showAppName);
    this->ui->editAppName->setText(this->showAppNameConfig);
//    this->ui->checkGetQrcode->setChecked(this->getQR);
    this->ui->checkCyrketVer->setChecked(this->getCyrketVer);
    this->ui->checkColorShellFiles->setChecked(this->colorShellFiles);
    ui->checkBoxShowCopyConfirmation->setChecked(this->showCopyConfirmation);

    this->ui->checkBoxEnableAnimations->setChecked(this->enableAnimations);
    ui->listWidgetCurvePicker->setCurrentRow(this->animationCurve);
    ui->spinBoxAnimationDuration->setValue(this->animationDuration);
    ui->comboBoxAnimationDirection->setCurrentIndex(this->animationDirection);

    this->ui->editFontApp->setText(this->fontApp.family());
    this->ui->editFontApp->setFont(this->fontApp);
    this->ui->editFontShell->setText(this->fontShell.family());
    this->ui->editFontShell->setFont(this->fontShell);
    this->ui->editFontTables->setText(this->fontTables.family());
    this->ui->editFontTables->setFont(this->fontTables);

    this->ui->stackedSettings->setCurrentWidget(this->ui->pageSettingsGeneral);
    this->ui->listWidget->item(0)->setSelected(true);
    this->ui->listWidget->item(1)->setSelected(false);
    this->ui->listWidget->item(2)->setSelected(false);
    this->ui->listWidget->setCurrentRow(0);


    this->ui->comboLayout->setCurrentIndex(this->layoutDirection);
    this->setBackgroundColor(ui->shellLabelCurrentBackground, this->shellBackgroundColor);
    this->setBackgroundColor(ui->shellLabelCurrentFont, this->shellFontColor);
    this->ui->editSdExt->setText(this->sdFolder);

    this->ui->spinToolBarIconSize->setValue(this->toolBarIconSize.height());
    if (this->toolBarHideButton)
        this->ui->comboToolBarHideButton->setCurrentIndex(0);
    else
        this->ui->comboToolBarHideButton->setCurrentIndex(1);
    switch(this->toolBarStyle)
    {
    case Qt::ToolButtonIconOnly:
        this->ui->comboToolBarStyle->setCurrentIndex(0);
        break;
    case Qt::ToolButtonTextOnly:
        this->ui->comboToolBarStyle->setCurrentIndex(1);
        break;
    case Qt::ToolButtonTextBesideIcon:
        this->ui->comboToolBarStyle->setCurrentIndex(2);
        break;
    case Qt::ToolButtonTextUnderIcon:
        this->ui->comboToolBarStyle->setCurrentIndex(3);
        break;
    default:
        this->ui->comboToolBarStyle->setCurrentIndex(0);
        break;
    }

    switch(this->toolBarArea)
    {
    case Qt::LeftToolBarArea:
        this->ui->comboToolBarArea->setCurrentIndex(0);
        break;
    case Qt::RightToolBarArea:
        this->ui->comboToolBarArea->setCurrentIndex(1);
        break;
    case Qt::TopToolBarArea:
        this->ui->comboToolBarArea->setCurrentIndex(2);
        break;
    case Qt::BottomToolBarArea:
        this->ui->comboToolBarArea->setCurrentIndex(3);
        break;
    default:
        this->ui->comboToolBarArea->setCurrentIndex(0);
        break;
    }
    AppTableModel appModel;
    this->appsColumnModel->setColumnCount(1);
    int columns = appModel.columnCount(QModelIndex());
    this->appsColumnModel->setRowCount(columns);

    for( int r=0; r<columns; r++ )
    {
        QString tmp = appModel.headerData(r, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tmp.isEmpty())
            tmp = tr("Icon", "icon app column");
        QStandardItem *item = new QStandardItem(tmp);
        item->setCheckable(true);
        if (this->appColumnList.contains(QString::number(r)) || this->appColumnList.isEmpty())
            item->setCheckState(Qt::Checked);
        this->appsColumnModel->setItem(r, item);
    }
    this->ui->listAppsColumn->setModel( this->appsColumnModel );


    BackupTableModel backupModel;
    this->backupsColumnModel->setColumnCount(1);
    columns = backupModel.columnCount(QModelIndex());
    this->backupsColumnModel->setRowCount(columns);

    for( int r=0; r<columns; r++ )
    {
        QString tmp = backupModel.headerData(r, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tmp.isEmpty())
            tmp = tr("Icon", "icon backup column");
        QStandardItem *item = new QStandardItem(tmp);
        item->setCheckable(true);
        if (this->backupColumnList.contains(QString::number(r)) || this->backupColumnList.isEmpty())
            item->setCheckState(Qt::Checked);
        this->backupsColumnModel->setItem(r, item);
    }
    this->ui->listBackupsColumn->setModel( this->backupsColumnModel );

    FileTableModel fileModel;
    this->computerColumnModel->setColumnCount(1);
    columns = fileModel.columnCount(QModelIndex());
    this->computerColumnModel->setRowCount(columns);

    for( int r=0; r<columns; r++ )
    {
        QString tmp = fileModel.headerData(r, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tmp.isEmpty())
            tmp = tr("Icon", "icon computer column");
        QStandardItem *item = new QStandardItem(tmp);
        item->setCheckable(true);
        if (this->computerColumnList.contains(QString::number(r)) || this->computerColumnList.isEmpty())
            item->setCheckState(Qt::Checked);
        this->computerColumnModel->setItem(r, item);
    }
    this->ui->listComputerColumn->setModel( this->computerColumnModel );

    this->phoneColumnModel->setColumnCount(1);
    columns = fileModel.columnCount(QModelIndex());
    this->phoneColumnModel->setRowCount(columns);

    for( int r=0; r<columns; r++ )
    {
        QString tmp = fileModel.headerData(r, Qt::Horizontal, Qt::DisplayRole).toString();
        if (tmp.isEmpty())
            tmp = tr("Icon", "icon phone column");
        QStandardItem *item = new QStandardItem(tmp);
        item->setCheckable(true);
        if (this->phoneColumnList.contains(QString::number(r)) || this->phoneColumnList.isEmpty())
            item->setCheckState(Qt::Checked);
        this->phoneColumnModel->setItem(r, item);
    }
    this->ui->listPhoneColumn->setModel( this->phoneColumnModel );

    this->settingsSlotChanged();
}

void SettingsWidget::chooseFontApp()
{
    bool ok;
    QFont tmp;
    tmp = QFontDialog::getFont(&ok, QFont(this->fontApp), this);
    if (ok)
    {
        this->fontApp = tmp;
        this->ui->editFontApp->setText(this->fontApp.family());
        this->ui->editFontApp->setFont(this->fontApp);
    }
}

void SettingsWidget::chooseFontShell()
{
    bool ok;
    QFont tmp;
    tmp = QFontDialog::getFont(&ok, QFont(this->fontShell), this);
    if (ok)
    {
        this->fontShell = tmp;
        this->ui->editFontShell->setText(this->fontShell.family());
        this->ui->editFontShell->setFont(this->fontShell);
    }
}

void SettingsWidget::chooseFontTables()
{
    bool ok;
    QFont tmp;
    tmp = QFontDialog::getFont(&ok, QFont(this->fontTables), this);
    if (ok)
    {
        this->fontTables = tmp;
        this->ui->editFontTables->setText(this->fontTables.family());
        this->ui->editFontTables->setFont(this->fontTables);
    }
}

void SettingsWidget::changeFont()
{
    foreach (QWidget *widget, QApplication::allWidgets())
    {
        QString className = widget->metaObject()->className();
        if (className == "QTableWidget" || className == "QTableView" || className == "MyTableView")
        {
            widget->setFont(this->fontTables);
        }
        else if (className == "QTextBrowser" || className == "QTextEdit" || className == "ShellWidget")
        {
            widget->setFont(this->fontShell);
        }
        else
        {
            widget->setFont(this->fontApp);
        }
        if (this->layoutDirection == 0)
        {
            widget->setLayoutDirection(Qt::LeftToRight);
        }
        else
        {
            widget->setLayoutDirection(Qt::RightToLeft);
        }
    }

//    this->ui->leftTableWidget->setFont(this->fontTables);
//    this->ui->rightTableWidget->setFont(this->fontTables);
//    this->ui->tableWidgetApps->setFont(this->fontTables);
//    this->ui->textShell->setFont(this->fontShell);
//    this->ui->textNandroidBackup->setFont(this->fontShell);
//    this->ui->textNandroidRestore->setFont(this->fontShell);
//    this->ui->textFixUID->setFont(this->fontShell);
//    this->ui->textFlashZIP->setFont(this->fontShell);
//    this->ui->textWipeData->setFont(this->fontShell);
    this->ui->editFontTables->setText(this->fontTables.family());
    this->ui->editFontTables->setFont(this->fontTables);
    this->ui->editFontShell->setText(this->fontShell.family());
    this->ui->editFontShell->setFont(this->fontShell);
}

void SettingsWidget::on_buttonAssociate_clicked()
{
    if (QMessageBox::question(this,tr("Associate??"),tr("Are you sure you want to associate *.apk files with QtADB???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\", QSettings::NativeFormat);
    settings.setValue(".apk/Default", "QtADB.File.apk");
    QString tmp;
    tmp = qApp->arguments().at(0);
    tmp.replace("/", "\\");
    settings.setValue("QtADB.File.apk/DefaultIcon/Default", tmp + ",0");
    settings.setValue("QtADB.File.apk/shell/open/command/Default",
                      "\"" + tmp + "\" \"%1\"");
    settings.setValue("QtADB.File.apk/shell/Install/Default",
                      tr("Install apk"));
    settings.setValue("QtADB.File.apk/shell/Install/command/Default",
                      "\"" + tmp + "\" \"-install\" \"%1\"");
}

void SettingsWidget::setBackgroundColor(QLabel *label, QColor color)
{
    QPalette palette = label->palette();
    palette.setColor(label->backgroundRole(), color);
    label->setAutoFillBackground(true);
    label->setPalette(palette);
}

void SettingsWidget::on_shellPushButtonBackground_clicked()
{
    QColorDialog dialog;
    QColor color = dialog.getColor();

    setBackgroundColor(ui->shellLabelCurrentBackground, color);
}

void SettingsWidget::on_shellPushButtonFont_clicked()
{
    QColorDialog dialog;
    QColor color = dialog.getColor();

    setBackgroundColor(ui->shellLabelCurrentFont, color);
}

void SettingsWidget::detectSdExtFolder()
{

        QProcess *shell = new QProcess;
        QString output;
        this->sdFolder = "";

        shell->start("\"" + this->sdkPath + "\"adb shell 'busybox stat /data/app |grep \"File\"'");
        shell->waitForFinished();
        output = shell->readAll();
        if (output.contains("->"))
        {
            output.remove(QRegExp("^.*->"));
            output.remove(QRegExp("app\".*$"));
            this->sdFolder = output;
        }
        else
        {
            shell->start("\"" + this->sdkPath + "\"adb shell busybox mount");
            shell->waitForFinished();
            output = shell->readAll();
            if (output.contains("ext"))
            {
                    QStringList lines = output.split("\n", QString::SkipEmptyParts);
                    while (lines.size() > 0)
                    {
                        QString tmp = lines.takeFirst();
                        if (tmp.contains("ext"))
                        {
                            QStringList split = tmp.split(QRegExp("\\s+"),QString::SkipEmptyParts);
                            if (split.size() > 4)

                                if (split.at(4).startsWith("ext"))
                                    this->sdFolder = split.at(2);
                        }
                    }

            }
        }
        if (!this->sdFolder.isEmpty() && !this->sdFolder.endsWith("/",Qt::CaseInsensitive))
            this->sdFolder.append("/");
        if (this->sdFolder.isEmpty())
            this->ui->editSdExt->setText(tr("<Not Found>"));
        else
            this->ui->editSdExt->setText(this->sdFolder);
}

void SettingsWidget::on_pushButtonChangeSDKPath_pressed()
{
    QString sdk;
    QString tmp;
    bool sdkOk = false;

    sdk=QFileDialog::getExistingDirectory(NULL,QObject::tr("Choose path to dir with adb and aapt binaries"),"/");


    if (sdk.isEmpty())
	return;

    sdk.append("/");

    QDir checkSDK(sdk);
    QFileInfoList list=checkSDK.entryInfoList();
    while(list.length()>0)
    {
	tmp = list.takeFirst().fileName();
	if (tmp.contains("adb"))
	{
	    sdkOk=true;
	    break;
	}
    }

    if (!sdkOk)
    {
	QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, QObject::tr("error"), QObject::tr("there is no adb binary in here!"));
	msgBox->exec();
    }
    else
    {
	QSettings settings;
	settings.setValue("sdkPath",sdk);
	this->sdkPath = sdk;
    }
}
