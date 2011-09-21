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


#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include "application.h"

QTranslator* Application::current = 0;
Translators Application::translatorsRes, Application::translatorsDir;

Application::Application(int& argc, char* argv[])
        : QApplication(argc, argv)
{
}

Application::~Application()
{
}

//void Application::loadTranslations(const QString& dir)
//{
//        loadTranslations(QDir(dir));
//}

void Application::loadTranslations(const QString& dir)
{
        QString filter = "qtadb_*.qm";
        QDir directory;
        directory.setPath(dir);
        QDir::Filters filters = QDir::Files | QDir::Readable;
        QDir::SortFlags sort = QDir::Name;
        QFileInfoList entriesA = directory.entryInfoList();
        QFileInfoList entries = directory.entryInfoList(QStringList() << filter, filters, sort);
        foreach (QFileInfo file, entries)
        {
                // pick country and language out of the file name
                QString language = file.baseName().split("_").at(1);

                // construct and load translator
                QTranslator* translator = new QTranslator(instance());
                if (translator->load(file.absoluteFilePath()))
                {
                        QString locale = language;
                        if (directory.path() == qApp->applicationDirPath())
                            translatorsDir.insert(locale, translator);
                        else
                            translatorsRes.insert(locale, translator);
                }
        }
}

const QStringList Application::availableLanguagesRes()
{
        // the content won't get copied thanks to implicit sharing and constness
        return QStringList(translatorsRes.keys());
}

const QStringList Application::availableLanguagesDir()
{
        // the content won't get copied thanks to implicit sharing and constness
        return QStringList(translatorsDir.keys());
}

void Application::setLanguage(const QString& locale, QString source)
{
        // remove previous
        if (current)
        {
                removeTranslator(current);
        }

        // install new
        if (source == "dir")
        {
            current = translatorsDir.value(locale, 0);
            if (current)
            {
                installTranslator(current);
                return;
            }
        }
        if (source == "res")
        {
            current = translatorsRes.value(locale, 0);
            if (current)
            {
                installTranslator(current);
                return;
            }
        }
}
