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


#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QtGui>
#include "../classes/models/filetablemodel.h"

class MyTableView : public QTableView
{
    Q_OBJECT
    QPoint dragStartPosition;
    FileTableModel fileModel;
public:
    explicit MyTableView(QWidget *parent = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
//    void dragLeaveEvent(QDragLeaveEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

signals:

public slots:

};

#endif // MYTABLEVIEW_H
