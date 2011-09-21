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


#include "shellwidget.h"
#include "ui_shellwidget.h"

/*
  dodac 2 listy stringow:
  - commandList - w konstruktorze wczytywac komendy busyboxa i shella, a pozniej szift+tab bedzie podpowiadal komendy
  - fileList - przy przechodzeniu miedzy folderami (komenda 'cd') bedzie wywolywana metoda Phone::getFileList a nastepnie po nacisnieciu tab bedzie podpwiadac foldery
    * obie listy maja byc posortowane alfabetycznie.
    * aby uzyskac podpowiedz nie trzeba wpisywac duzych liter
*/

ShellWidget::ShellWidget(QWidget *parent) :
    QTextEdit(parent)
{
    this->insertedChars = 0;
    this->cursorPosition = 0;
    this->commandHistoryPosition = 0;
    this->cursor = this->textCursor();
    this->setCursorWidth(3);
    this->setTextCursor(cursor);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    QSettings settings;
    this->sdk=settings.value("sdkPath").toString();

    this->fontColor = settings.value("shellFontColor", Qt::black).value<QColor>();

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, settings.value("shellBackgroundColor", Qt::white).value<QColor>());

    this->setPalette(palette);

    this->setTextColor(this->fontColor);

    //qDebug()<<"MainWindow::showPageShell() - process shell is not running, starting...";
    this->process.setProcessChannelMode(QProcess::MergedChannels);
    this->process.start("\""+sdk+"\""+"adb shell");

    connect(&this->process, SIGNAL(readyRead()), this, SLOT(readFromProcess()));
}

ShellWidget::~ShellWidget()
{
    this->process.close();
}

void ShellWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier)
    {
        if (e->key() == Qt::Key_C)
        {
            this->process.write(QString(QChar(0x3)).toAscii());
        }
        else if (e->key() == Qt::Key_Left)
        {
            if (this->cursorPosition < this->insertedChars)
            {
                int pos = this->cursor.position();
                this->cursor.movePosition(QTextCursor::PreviousWord);
                this->setTextCursor(this->cursor);
                this->cursorPosition+=pos-this->cursor.position();
            }
        }
        else if (e->key() == Qt::Key_Right)
        {
            if (this->cursorPosition > 0)
            {
                int pos = this->cursor.position();
                this->cursor.movePosition(QTextCursor::NextWord);
                this->setTextCursor(this->cursor);
                this->cursorPosition-=this->cursor.position()-pos;
            }
        }
        else if (e->key() == Qt::Key_Backspace)
        {
            //usun poprzedzajace slowo
        }
        else if (e->key() == Qt::Key_Delete)
        {
            //usun nastepne slowo
        }
        return;
    }
    else if ((e->modifiers() & Qt::SHIFT) && (e->modifiers() & Qt::CTRL))
    {
        if (e->key() == Qt::Key_V)
        {
            QClipboard *clipboard = QApplication::clipboard();
            QString tmp = clipboard->text(QClipboard::Clipboard);
            if (tmp.length()>0)
            {
                this->insertedChars+=tmp.length();
                this->command.insert(this->command.length()-this->cursorPosition,tmp);
                this->insertPlainText(tmp);
            }
        }
        else if (e->key() == Qt::Key_C)
        {
            QClipboard *clipboard = QApplication::clipboard();
            QString tmp = this->textCursor().selection().toPlainText();

            clipboard->setText(tmp,QClipboard::Clipboard);
        }
        return;
    }

    this->cursor.movePosition(QTextCursor::End);
    this->cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,this->cursorPosition);
    this->setTextCursor(this->cursor);

    if (e->key() == Qt::Key_Return)
    {
        this->cursor.movePosition(QTextCursor::End);
        this->setTextCursor(this->cursor);
        this->cursorPosition = 0;
        this->insertedChars = 0;
        executeCommand(this->command);
        this->command.clear();
        this->commandHistoryPosition = -1;
    }
    else if (e->key() == Qt::Key_Up)
    {
        if (this->commandHistory.length() -1 > this->commandHistoryPosition)
        {
            if (command.length()>0)
            {
                this->cursor.movePosition(QTextCursor::End);
                for (int i = 0 ; i < this->insertedChars ; i++)
                    this->cursor.deletePreviousChar();
                this->cursorPosition = 0;
                this->insertedChars = 0;
                command.clear();
            }
            this->commandHistoryPosition++;
            this->command = this->commandHistory.at(this->commandHistoryPosition);
            this->insertedChars = this->command.length();
            this->insertPlainText(this->command);
        }
    }
    else if (e->key() == Qt::Key_Down)
    {
        if (this->commandHistoryPosition > 0)
        {
            if (command.length()>0)
            {
                this->cursor.movePosition(QTextCursor::End);
                for (int i = 0 ; i < this->insertedChars ; i++)
                    this->cursor.deletePreviousChar();
                this->cursorPosition = 0;
                this->insertedChars = 0;
                command.clear();
            }
            this->commandHistoryPosition--;
            this->command = this->commandHistory.at(this->commandHistoryPosition);
            this->insertedChars = this->command.length();
            this->insertPlainText(this->command);
        }
    }
    else if (e->key() == Qt::Key_Left)
    {
        if (this->cursorPosition < this->insertedChars)
        {
            this->cursor.movePosition(QTextCursor::Left);
            this->setTextCursor(this->cursor);
            this->cursorPosition++;
        }
    }
    else if (e->key() == Qt::Key_Right)
    {
        if (this->cursorPosition > 0)
        {
            this->cursor.movePosition(QTextCursor::Right);
            this->setTextCursor(this->cursor);
            this->cursorPosition--;
        }
    }
    else if (e->key() == Qt::Key_Delete)
    {
        if (this->cursorPosition > 0)
        {
            this->cursor.movePosition(QTextCursor::Right);
            this->setTextCursor(this->cursor);
            this->cursor.deletePreviousChar();
            this->command.remove(this->command.length()-this->cursorPosition-1,1);
            this->insertedChars--;
            this->cursorPosition--;
        }
    }
    else if (e->key() == Qt::Key_Backspace)
    {
        if (this->insertedChars > this->cursorPosition)
        {
            this->cursor.deletePreviousChar();
            this->command.remove(this->command.length()-this->cursorPosition-1,1);
            this->insertedChars--;
        }
    }
    else if(e->key() == Qt::Key_Escape)
    {
        this->process.write(QString(QChar(0x3)).toAscii());
    }
    else if (e->text().length()>0)
    {
        this->insertPlainText(e->text());
        this->insertedChars++;
        this->command.insert(this->command.length()-this->cursorPosition,e->text());
    }

}

void ShellWidget::executeCommand(QString command)
{
    if (command == "qtadb -help")
    {
        this->append(tr("\nQtADB shell help\n"));
        this->append(tr("CTRL+C                - interrupt executing command"));
        this->append(tr("ESC                   - interrupt executing command"));
        this->append(tr("Shift+CTRL+C          - copy selected text to clipboard"));
        this->append(tr("Shift+CTRL+V          - paste text from clipboard"));
        this->append(tr("Enter/Return          - execute command"));
        this->append(tr("Up (arrow)            - display previous executed command"));
        this->append(tr("Down (arrow)          - display next executed command"));
        this->append(tr("Left(arrow)           - move cursor to the left"));
        this->append(tr("Right(arrow)          - move cursor to the right"));
        this->append(tr("CTRL+Left(arrow)      - move cursor to the left skipping over the word"));
        this->append(tr("CTRL+Right(arrow)     - move cursor to the right skipping over the word"));
        this->append(tr("Delete                - delete next char"));
        this->append(tr("Backspace             - delete previous char"));
        this->process.write("\n");
    }
    else
    {
        this->process.write(command.toLatin1()+"\n");
    }
    this->commandHistory.prepend(command);
}

void ShellWidget::readFromProcess()
{
    QString tmp = QString::fromUtf8(this->process.readAll());
    QStringList tmp2;
    QString print;
    int i;

    for (i = 0; i < tmp.length(); i++)
    {
        if (tmp.at(i).unicode() == 13)
            tmp[i] = ' ';
        if (tmp.at(i).unicode() == 10)
            tmp[i] = '\n';
    }
    tmp.remove(0,tmp.indexOf("\n"));
    if (tmp.contains(QChar( 0x1b ), Qt::CaseInsensitive))
    {
        QSettings settings;
        tmp.remove("[0m");
        if (settings.value("colorShellFiles").toBool())
        {
            tmp2 = tmp.split(QChar( 0x1b ), QString::SkipEmptyParts, Qt::CaseInsensitive);

            while (tmp2.size() > 0)
            {
                print = tmp2.takeFirst();
                if (print.contains("0;30"))//black
                {
                    this->setTextColor(this->fontColor);
                }
                else if (print.contains("0;34"))//blue
                {
                    this->setTextColor(Qt::blue);
                }
                else if (print.contains("0;32"))//green
                {
                    this->setTextColor(Qt::green);
                }
                else if (print.contains("0;36"))//cyan
                {
                    this->setTextColor(Qt::cyan);
                }
                else if (print.contains("0;31"))//red
                {
                    this->setTextColor(Qt::red);
                }
                else if (print.contains("0;35"))//purple
                {
                    this->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("0;33"))//brown
                {
                    this->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("0;37"))//light gray
                {
                    this->setTextColor(Qt::lightGray);
                }
                else if (print.contains("1;30"))//dark gray
                {
                    this->setTextColor(Qt::darkGray);
                }
                else if (print.contains("[1;34"))//dark gray
                {
                    this->setTextColor(Qt::blue);
                }
                else if (print.contains("1;32"))//light green
                {
                    this->setTextColor(Qt::green);
                }
                else if (print.contains("1;36"))//light cyan
                {
                    this->setTextColor(Qt::cyan);
                }
                else if (print.contains("1;31"))//light red
                {
                    this->setTextColor(Qt::red);
                }
                else if (print.contains("1;35"))//light purple
                {
                    this->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("1;33"))//yellow
                {
                    this->setTextColor(Qt::yellow);
                }
                else if (print.contains("1;37"))//white
                {
                    this->setTextColor(Qt::white);
                }
                print.remove(QRegExp("\\[\\d;\\d+m"));
                this->insertPlainText(print);
                this->setTextColor(this->fontColor);
            }
        }
        else
        {
            tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            tmp.remove(QRegExp("\\[\\d;\\d+m"));
            this->insertPlainText(tmp);
        }
    }
    else
    {
        this->append(tmp);
    }
    this->ensureCursorVisible();

    //qDebug()<<"readShell() - "<<tmp;
}

/*
ShellWidget::ShellWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShellWidget)
{
    ui->setupUi(this);
    connect(ui->editShell, SIGNAL(returnPressed()), this, SLOT(writeShell()));
    this->commandModel = new QStringListModel;
    this->completer = new QCompleter;
    this->completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    this->completer->setMaxVisibleItems(10);
    this->completer->setModel(this->commandModel);
    this->completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    this->ui->editShell->setCompleter(this->completer);
    this->processShellIsRunning = false;

    connect(this->procesShell, SIGNAL(finished(int)), this, SLOT(shellFinished()));
}
void ShellWidget::changeEvent(QEvent *e)
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

void ShellWidget::keyPressEvent(QKeyEvent *e)
{
    if(ui->editShell->hasFocus() && e->key() == Qt::Key_Escape)
    {
        this->procesShell->write(QString(QChar(0x3)).toAscii());
    }
    if(ui->editShell->hasFocus() && e->key() == Qt::Key_Down)
    {
        this->completer->complete();
    }

}


void ShellWidget::readShell()
{
    QString tmp = QString::fromUtf8(this->procesShell->readAll());
    QStringList tmp2;
    QString print;
    int i;


    for (i = 0; i < tmp.length(); i++)
    {
        if (tmp.at(i).unicode() == 13)
            tmp[i] = ' ';
        if (tmp.at(i).unicode() == 10)
            tmp[i] = '\n';
    }
    if (tmp.contains(QChar( 0x1b ), Qt::CaseInsensitive))
    {
        QSettings settings;
        tmp.remove("[0m");
        if (settings.value("colorShellFiles").toBool())
        {
            tmp2 = tmp.split(QChar( 0x1b ), QString::SkipEmptyParts, Qt::CaseInsensitive);
            while (tmp2.size() > 0)
            {
                print = tmp2.takeFirst();
                if (print.contains("0;30"))//black
                {
//                    this->ui->textShell->setTextColor(Qt::black);
                    this->ui->textShell->setTextColor(this->fontColor);
                }
                else if (print.contains("0;34"))//blue
                {
                    this->ui->textShell->setTextColor(Qt::blue);
                }
                else if (print.contains("0;32"))//green
                {
                    this->ui->textShell->setTextColor(Qt::green);
                }
                else if (print.contains("0;36"))//cyan
                {
                    this->ui->textShell->setTextColor(Qt::cyan);
                }
                else if (print.contains("0;31"))//red
                {
                    this->ui->textShell->setTextColor(Qt::red);
                }
                else if (print.contains("0;35"))//purple
                {
                    this->ui->textShell->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("0;33"))//brown
                {
                    this->ui->textShell->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("0;37"))//light gray
                {
                    this->ui->textShell->setTextColor(Qt::lightGray);
                }
                else if (print.contains("1;30"))//dark gray
                {
                    this->ui->textShell->setTextColor(Qt::darkGray);
                }
                else if (print.contains("[1;34"))//dark gray
                {
                    this->ui->textShell->setTextColor(Qt::blue);
                }
                else if (print.contains("1;32"))//light green
                {
                    this->ui->textShell->setTextColor(Qt::green);
                }
                else if (print.contains("1;36"))//light cyan
                {
                    this->ui->textShell->setTextColor(Qt::cyan);
                }
                else if (print.contains("1;31"))//light red
                {
                    this->ui->textShell->setTextColor(Qt::red);
                }
                else if (print.contains("1;35"))//light purple
                {
                    this->ui->textShell->setTextColor(QColor::fromRgb(0, 0, 0));
                }
                else if (print.contains("1;33"))//yellow
                {
                    this->ui->textShell->setTextColor(Qt::yellow);
                }
                else if (print.contains("1;37"))//white
                {
                    this->ui->textShell->setTextColor(Qt::white);
                }
                print.remove(QRegExp("\\[\\d;\\d+m"));
                this->ui->textShell->insertPlainText(print);
                this->ui->textShell->setTextColor(this->fontColor);
            }
        }
        else
        {
            tmp.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            tmp.remove(QRegExp("\\[\\d;\\d+m"));
            this->ui->textShell->insertPlainText(tmp);
        }
    }
    else
    {
        this->ui->textShell->append(tmp);
    }

    qDebug()<<"readShell() - "<<tmp;
}

void ShellWidget::shellFinished()
{
    qDebug()<<"ShellWidget::shellFinished()";
    disconnect(this->procesShell,SIGNAL(readyRead()),this,SLOT(readShell()));
    disconnect(this->procesShell, SIGNAL(finished(int)), this, SLOT(shellFinished()));
}

void ShellWidget::writeShell()
{
    QString tmp;

    tmp = this->ui->editShell->text();
    if (!this->commandList.contains(tmp))
        this->commandList.append(tmp);
    this->commandModel->setStringList(this->commandList);
    tmp.append("\n");
    this->procesShell->write(tmp.toUtf8());

    this->ui->editShell->clear();
}
*/
