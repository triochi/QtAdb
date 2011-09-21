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


#include "screenshotwidget.h"
#include "ui_screenshotwidget.h"

ScreenshotWidget::ScreenshotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenshotWidget)
{
    ui->setupUi(this);

    this->rotation = 0;
    this->widthScreen = 320;
    this->heightScreen = 480;
    this->screenshot = QPixmap::fromImage(noScreenshotImage(this->widthScreen, this->heightScreen), Qt::AutoColor);
    this->ui->labelRgb565->setPixmap(this->screenshot);

    connect(this->ui->buttonSaveScreenshot, SIGNAL(clicked()), this, SLOT(saveScreenshot()));
    connect(this->ui->buttonRefreshScreenshot, SIGNAL(clicked()), this, SLOT(refreshScreenshot()));
    connect(this->ui->buttonRotateLeft, SIGNAL(clicked()), this, SLOT(rotateLeft()));
    connect(this->ui->buttonRotateRight, SIGNAL(clicked()), this, SLOT(rotateRight()));

    this->setLayout(ui->layoutScreenshot);
    refreshScreenshot();
}

ScreenshotWidget::~ScreenshotWidget()
{
    delete ui;
}

void ScreenshotWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void ScreenshotWidget::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);

    QSize scaledSize = this->screenshot.size();
    scaledSize.scale(this->ui->labelRgb565->size(), Qt::KeepAspectRatio);
    if (!this->ui->labelRgb565->pixmap() || scaledSize != this->ui->labelRgb565->pixmap()->size())
        updateScreenshotLabel();
}

void ScreenshotWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->rotation = 0;
        QSize scaledSize = QSize(this->widthScreen, this->heightScreen);
        scaledSize.scale(this->ui->labelRgb565->size(), Qt::KeepAspectRatio);
        QPixmap pix = QPixmap::fromImage(noScreenshotImage(scaledSize.width(), scaledSize.height()), Qt::AutoColor);
        this->ui->labelRgb565->setPixmap(pix);
        this->takeScreenshot();
    }
}

void ScreenshotWidget::refreshScreenshot()
{
    this->rotation = 0;

    QSize scaledSize = QSize(this->widthScreen, this->heightScreen);
    scaledSize.scale(this->ui->labelRgb565->size(), Qt::KeepAspectRatio);
    QPixmap pix = QPixmap::fromImage(noScreenshotImage(scaledSize.width(), scaledSize.height()), Qt::AutoColor);
    this->ui->labelRgb565->setPixmap(pix);
    takeScreenshot();
}

void ScreenshotWidget::rotateLeft()
{
    QMatrix matrix;
    QImage image;
    image = this->screenshot.toImage();
    this->rotation -= 90;
    matrix.rotate(this->rotation);

    image = image.transformed(matrix);
    this->ui->labelRgb565->setPixmap(
                QPixmap::fromImage(image, Qt::AutoColor).scaled(this->ui->labelRgb565->size(),
                                                                Qt::KeepAspectRatio,
                                                                Qt::SmoothTransformation));
}

void ScreenshotWidget::rotateRight()
{
    QMatrix matrix;
    QImage image;
    image = this->screenshot.toImage();
    this->rotation += 90;
    matrix.rotate(this->rotation);

    image = image.transformed(matrix);
    this->ui->labelRgb565->setPixmap(
                QPixmap::fromImage(image, Qt::AutoColor).scaled(this->ui->labelRgb565->size(),
                                                                Qt::KeepAspectRatio,
                                                                Qt::SmoothTransformation));
}

void ScreenshotWidget::saveScreenshot()
{
    QFile plik;
    plik.setFileName(QFileDialog::getSaveFileName(this, tr("Save File..."), "./screenshot.png", tr("Png file")+" (*.png)"));
    if (plik.fileName().isEmpty())
        return;
    if (plik.open(QFile::WriteOnly))
    {
        QMatrix matrix;
        matrix.rotate(this->rotation);
        QImage image;
        image = this->screenshot.toImage();
        image = image.transformed(matrix);
        image.save(&plik, "PNG");
        plik.close();
    }
}

void ScreenshotWidget::showScreenshot(QImage image, int width, int height)
{
    this->rotation = 0;
    QSize scaledSize = QSize(width, height);
    scaledSize.scale(this->size(), Qt::KeepAspectRatio);
    this->screenshot = QPixmap::fromImage(image, Qt::AutoColor);
    this->ui->labelRgb565->setPixmap(this->screenshot.scaled(this->ui->labelRgb565->size(),
                                                             Qt::KeepAspectRatio,
                                                             Qt::SmoothTransformation));
    disconnect(this, SLOT(showScreenshot(QImage,int,int)));
}

void ScreenshotWidget::takeScreenshot()
{
    threadScreenshot.widthScreen = this->ui->labelRgb565->width();
    threadScreenshot.heightScreen = this->ui->labelRgb565->height();
    threadScreenshot.start();
    connect(&threadScreenshot, SIGNAL(gotScreenshot(QImage, int, int)), this, SLOT(showScreenshot(QImage, int, int)));
}

void ScreenshotWidget::updateScreenshotLabel()
 {
    QMatrix matrix;
    matrix.rotate(this->rotation);
    QImage image;
    image = this->screenshot.toImage();
    image = image.transformed(matrix);

    QSize scaledSize = image.size();
    scaledSize.scale(this->ui->labelRgb565->size(), Qt::KeepAspectRatio);

    this->ui->labelRgb565->setPixmap(QPixmap::fromImage(image.scaled(this->ui->labelRgb565->size(),
                                                             Qt::KeepAspectRatio,
                                                             Qt::SmoothTransformation), Qt::AutoColor));
 }
