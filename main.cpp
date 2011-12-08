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
#include <QProcess>
#include <QTextStream>
#include <QFileDialog>

QProcess *adbProces;
QString sdk;
QString adb;
QString aapt;
QString busybox;
QString fastboot;

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
//    qInstallMsgHandler(myMessageHandler);
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
    sdk = settings.value("sdkPath").toString();
    adb = settings.value("adbExecutable").toString();
    aapt = settings.value("aaptExecutable").toString();
    fastboot = settings.value("fastbootExecutable").toString();


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

    if(!QFile::exists(adb)){

        adb = QFileDialog::getOpenFileName(0,"adb executable", "", "adb.*");
    }
    if(!QFile::exists(aapt)){

        aapt = QFileDialog::getOpenFileName(0,"aapt executable",QFileInfo(adb).canonicalPath(), "aapt.*");
    }
    if(!QFile::exists(fastboot)){

        fastboot = QFileDialog::getOpenFileName(0,"fastboot executable", QFileInfo(adb).canonicalPath(), "fastboot.*");
    }

    if ((QFile::exists(adb))&&(QFile::exists(aapt))&&(QFile::exists(fastboot))){
        settings.setValue("sdkPath", sdk);
        settings.setValue("adbExecutable", adb);
        settings.setValue("aaptExecutable", aapt);
        settings.setValue("fastbootExecutable", fastboot);
        adbProces = new QProcess(&a);
        adbProces->setObjectName("adb process");
        adbProces->setReadChannel(QProcess::StandardOutput);
        adbProces->setProcessChannelMode(QProcess::MergedChannels);
        adbProces->setWorkingDirectory(sdk);
//        adbd cannot run as root in production builds
        adbProces->start("\"" + adb + "\" version");
        if (!adbProces->waitForStarted(5000)){
            QMessageBox::critical(0,"Error", "Error strarting adb. This is fatal!\n" );
            settings.setValue("adbExecutable", "");
            exit(0);
        }
        adbProces->waitForFinished(-1);
        QString tmp = adbProces->readAll();
        qDebug()<<" version - "<<tmp.toStdString().c_str();
        if (adbProces->exitCode() != 0)
        {
            qDebug()<<" error - "<<adbProces->errorString().toStdString().c_str();
            QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, QObject::tr("error"), QObject::tr("It seems that adb is not working properly"), QMessageBox::Ok);
            msgBox->exec();
            settings.setValue("adbExecutable", "");
            delete msgBox;
            return 1;
        }
//        adbd cannot run as root in production builds
        adbProces->start("\"" + adb + "\" root");
        adbProces->waitForFinished(-1);
        tmp = adbProces->readAll();
        qDebug()<<" root - "<<tmp.toStdString().c_str();

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
        QMessageBox::critical(0,"Error", "One or more of the executables needed by the program do not exist.\nPlease restart the application");
        settings.setValue("sdkPath", "");
        settings.setValue("adbExecutable", "");
        settings.setValue("aaptExecutable", "");
        settings.setValue("fastbootExecutable", "");
        return 0;
    }
}
