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


#include <QtGui/QApplication>
#include "./classes/application.h"
#include "./dialogs/mainwindow.h"
////////////////////////////////////////
#include <QtDebug>
#include <QFile>
#include <QTextStream>

void myMessageHandler(QtMsgType type, const char *msg)
{
    QStringList args = qApp->arguments();

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    if (args.contains("--debug"))
    {
        QFile outFile("debug.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);

        txt.remove(QChar( 0xa ),Qt::CaseInsensitive);
        txt.remove(QChar( 0xd ),Qt::CaseInsensitive);

        ts << txt << endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Bracia");
    QCoreApplication::setApplicationName("QtADB");
    QCoreApplication::setApplicationVersion("0.8.1");
    QCoreApplication::setOrganizationDomain("http://qtadb.com");
    Application a(argc, argv);
    qInstallMsgHandler(myMessageHandler);
    a.loadTranslations(":/lang");
    a.loadTranslations(qApp->applicationDirPath());
    a.setQuitOnLastWindowClosed(true);
    qDebug()<<"app version: "<<QCoreApplication::applicationVersion();
#ifdef Q_WS_WIN
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_XP:qDebug()<<"system: Windows XP "<<QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE");
        break;
    case QSysInfo::WV_VISTA:qDebug()<<"system: Windows Vista "<<QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE");
        break;
    case QSysInfo::WV_WINDOWS7:qDebug()<<"system: Windows 7 "<<QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE");
        break;
    default:
        qDebug()<<"system: "<<QSysInfo::windowsVersion()<<" "<<QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE");
    }
#endif
//    qDebug()<<"system: "<<QSysInfo::windowsVersion();
//    qDebug()<<"bits: "<<QSysInfo::WordSize;
    QSettings settings;
    QString sdk;
    sdk = settings.value("sdkPath").toString();


    QString locale = QLocale::system().name().left(2);

    QString lang = settings.value("Language", locale).toString();
    bool langSet = false;

    if (!Application::availableLanguagesRes().contains(lang))
        lang = "en";
    foreach (QString avail, Application::availableLanguagesRes())
    {
        if (avail == lang)
        {
            langSet = true;
            Application::setLanguage(lang, "res");
        }
    }

    foreach (QString avail, Application::availableLanguagesDir())
    {
        if ((avail == lang) && (langSet == false))
        {
            Application::setLanguage(lang, "dir");
        }
    }

    bool sdkOk = false;
    do{
        QString tmp;
        if (sdk.isEmpty())
        {
            QDir checkSDK(QDir::currentPath());
            QFileInfoList list=checkSDK.entryInfoList();
            while(list.length()>0)
            {
                tmp = list.takeFirst().fileName();
                if (tmp.contains("adb"))
                {
                    sdk = QDir::currentPath();
                    sdk.append("/");
                    sdkOk=true;
                    break;
                }
            }
        }
        if (sdk.isEmpty())
        {
            sdk=QFileDialog::getExistingDirectory(NULL,QObject::tr("Choose path to dir with adb and aapt binaries"),"/");
            if (!sdk.isEmpty())
                sdk.append("/");
        }
        if (!sdk.isEmpty())
        {
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
        }
        if (!sdkOk)
        {
            sdk.clear();
            QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, QObject::tr("error"), QObject::tr("there is no adb binary in here!"));
            QPushButton *choosePathMsg = msgBox->addButton(QObject::tr("Choose path"), QMessageBox::AcceptRole);
            QPushButton *closeMsg = msgBox->addButton(QObject::tr("Close"), QMessageBox::RejectRole);

            msgBox->exec();

            if (msgBox->clickedButton() == choosePathMsg)
            {
                continue;
            }
            else
            {
                break;
            }
            delete closeMsg;
            delete choosePathMsg;
            delete msgBox;
        }
        else break;
    }while(true);
    if (sdkOk){
        settings.setValue("sdkPath", sdk);
        QProcess proces;
//        adbd cannot run as root in production builds
        proces.setProcessChannelMode(QProcess::MergedChannels);
        proces.start("\"" + sdk + "\"adb version");
        proces.waitForFinished(-1);
        QString tmp = proces.readAll();
        qDebug()<<"adb version - "<<tmp;
        if (proces.exitCode() != 0)
        {
            qDebug()<<"adb error - "<<proces.errorString();
            QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, QObject::tr("error"), QObject::tr("It seems that adb is not working properly"), QMessageBox::Ok);
            msgBox->exec();
            delete msgBox;
            return 1;
        }
//        adbd cannot run as root in production builds
        proces.setProcessChannelMode(QProcess::MergedChannels);
        proces.start("\"" + sdk + "\"adb root");
        proces.waitForFinished(-1);
        tmp = proces.readAll();
        qDebug()<<"adb root - "<<tmp;

        if (tmp.contains("adbd cannot run as root in production builds") && !settings.value("disableProductionBuildsMessage",false).toBool())
        {
            QMessageBox *msgBox2 = new QMessageBox(QMessageBox::Critical, QObject::tr("error"),
                                                   QObject::tr("adbd cannot run as root in production builds so You can't do anything with /system partition. Run anyway?\n(press save to run QtADB and disable this message)"),
                                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Save);
            int button = msgBox2->exec();
            if ( button == QMessageBox::No)
            {
                delete msgBox2;
                return 0;
            }
            if ( button == QMessageBox::Save)
            {
                settings.setValue("disableProductionBuildsMessage",true);
            }
        }

        QStringList args = qApp->arguments();
        if (args.count() > 1)
        {
            if (args.at(1).endsWith(".apk"))
            {
                App *app = NULL;
                app = FileWidget::getAppInfo(args.at(1));
                appInfo *appInfoDialog = new appInfo(app);
                return appInfoDialog->exec();
            }
            if (args.at(1) == "-install")
            {
                QList<App> appList;
                App *app = NULL;
                for (int i = 2; i < args.count(); i++)
                {
                    if (args.at(i).endsWith(".apk"))
                    {
                        QString fileName = args.at(i);
                        app = FileWidget::getAppInfo(fileName);
                        if (app != NULL)
                            appList.append(*app);
                    }
                }
                appDialog *appDialogInstall = new appDialog(appList, appDialog::Install, appDialog::None);
                return appDialogInstall->exec();
            }
            else if (args.at(1) == "-logcat")
            {
                LogcatDialog *logcat = new LogcatDialog;
                return logcat->exec();
            }
        }
        MainWindow w;
        w.show();
        return a.exec();
    }
    else{
        return 0;
    }
}
