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


#ifndef APPTABLEMODEL_H
#define APPTABLEMODEL_H

#include <QtGui>

class App : public QObject{
//    Q_OBJECT
public:
    App(QObject *parent=0);
    App(const App&);
    QString appName;
    QString appSize;
    QString appVersion;
    QString appFile;
    QString appFileName;
    QString icoName;
    QString packageName;
    QString location;
    QIcon appIcon;
    QPixmap qrCode;
    QString cyrketVer;
    QString date;

    App& operator =(const App&);
};// App;

Q_DECLARE_METATYPE(App)

class AppTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AppTableModel(QObject *parent=0);
    AppTableModel(QList< App > appList, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool insertApp(int position, App app);
    bool insertApps(int position, QList< App > appList);
    App getApp(int row);
    QList< App > getList();
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool clear();
    static QString humanReadableSize(QString size);
    bool isEmpty();
    void setQr(int row, QPixmap pix);
    void setCyrketVer(int row, QString str);
private:
    QList< App > appList;
};


class AppSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    AppSortModel(QObject *parent = 0);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
     bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};
#endif // APPTABLEMODEL_H
