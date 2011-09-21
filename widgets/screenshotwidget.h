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


#ifndef SCREENSHOTWIDGET_H
#define SCREENSHOTWIDGET_H

#include <QtGui>
#include "../threads/screenshotthread.h"

namespace Ui {
    class ScreenshotWidget;
}

class ScreenshotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenshotWidget(QWidget *parent = 0);
    ~ScreenshotWidget();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent*);

private:
    Ui::ScreenshotWidget *ui;
    int widthScreen, heightScreen;
    ThreadScreenshot threadScreenshot;
    QPixmap screenshot;
    int rotation;
private slots:
    void takeScreenshot();
    void showScreenshot(QImage, int, int);
    void saveScreenshot();
    void refreshScreenshot();
    void rotateLeft();
    void rotateRight();
    void updateScreenshotLabel();
};

#endif // SCREENSHOTWIDGET_H
