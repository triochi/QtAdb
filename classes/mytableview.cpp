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


#include "mytableview.h"

MyTableView::MyTableView(QWidget *parent) :
    QTableView(parent)
{
    this->setAcceptDrops(true);
    this->setDragEnabled(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setAlternatingRowColors(true);
    this->setTabKeyNavigation(false);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::DragDrop);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setWordWrap(false);
    this->setShowGrid(false);
    this->setSortingEnabled(true);
    this->verticalHeader()->setVisible(false);
}

void MyTableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mouseButtons() == Qt::LeftButton)
        QTableView::dragEnterEvent(event);
    else
        event->ignore();
}

void MyTableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QTableView::mousePressEvent(event);
    }
    else if (event->button() == Qt::MiddleButton)
    {
        QModelIndex index = this->indexAt(event->pos());
        QModelIndexList list = this->selectionModel()->selectedRows();
        if (list.isEmpty())
            this->selectRow(index.row());
        else
        {
            for (int i=0; i< list.size(); i++)
                this->selectionModel()->select(list.at(i), QItemSelectionModel::Select);
            for (int i=0; i<this->fileModel.columnCount(QModelIndex()); i++)
                this->selectionModel()->select(this->model()->index(index.row(),i, QModelIndex()), QItemSelectionModel::Toggle);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        QModelIndex index = this->indexAt(event->pos());
        QModelIndexList list = this->selectionModel()->selectedRows();
        if (list.isEmpty())
            this->selectRow(index.row());
        else
        {
            for (int i=0; i< list.size(); i++)
                this->selectionModel()->select(list.at(i), QItemSelectionModel::Select);
            for (int i=0; i<this->fileModel.columnCount(QModelIndex()); i++)
                this->selectionModel()->select(this->model()->index(index.row(),i, QModelIndex()), QItemSelectionModel::Select);
        }
        emit customContextMenuRequested(event->pos());
    }
}

void MyTableView::dropEvent(QDropEvent *event)
{
    if (event->source() == this)
        event->ignore();
    else
        QTableView::dropEvent(event);
}

//void MyTableView::mouseMoveEvent(QMouseEvent *event)
//{
//    if (event->button() != Qt::LeftButton)
//         return;
////     if ((event->pos() - dragStartPosition).manhattanLength()
////          < QApplication::startDragDistance())
////         return;

//     event->accept();

//}
