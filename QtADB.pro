#
#Copyright 2010-20XX by Jakub Motyczko
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#   @author Jakub Motyczko
# -------------------------------------------------
QT += network
QT += declarative
TARGET = QtADB

TEMPLATE = app
SOURCES += main.cpp \
    ./dialogs/mainwindow.cpp \
    ./dialogs/dialogkopiuj.cpp \
    ./classes/phone.cpp \
    ./classes/computer.cpp \
    ./threads/screenshotthread.cpp \
    ./dialogs/connectWifi.cpp \
    ./dialogs/aboutdialog.cpp \
    ./dialogs/appdialog.cpp \
    ./classes/application.cpp \
    ./classes/updateapp.cpp \
    ./dialogs/appinfo.cpp \
    widgets/filewidget.cpp \
    widgets/appwidget.cpp \
    widgets/recoverywidget.cpp \
    widgets/fastbootwidget.cpp \
    widgets/screenshotwidget.cpp \
    widgets/phoneinfowidget.cpp \
    widgets/settingswidget.cpp \
    widgets/shellwidget.cpp \
    widgets/messagewidget.cpp \
    widgets/contactwidget.cpp \
    classes/models/apptablemodel.cpp \
    classes/models/filetablemodel.cpp \
    classes/models/backuptablemodel.cpp \
    classes/animation.cpp \
    classes/models/messagemodel.cpp \
    classes/models/messagethreadmodel.cpp \
    classes/models/contactmodel.cpp \
    dialogs/logcatdialog.cpp \
    classes/models/logcatmodel.cpp \
    classes/ecwin7.cpp \
    classes/mytableview.cpp \
    dialogs/registerdialog.cpp
HEADERS += ./dialogs/mainwindow.h \
    ./dialogs/dialogkopiuj.h \
    ./classes/phone.h \
    ./classes/computer.h \
    ./threads/screenshotthread.h \
    ./dialogs/connectWifi.h \
    ./dialogs/aboutdialog.h \
    ./dialogs/appdialog.h \
    ./classes/application.h \
    ./classes/updateapp.h \
    ./dialogs/appinfo.h \
    widgets/filewidget.h \
    widgets/appwidget.h \
    widgets/recoverywidget.h \
    widgets/fastbootwidget.h \
    widgets/screenshotwidget.h \
    widgets/phoneinfowidget.h \
    widgets/settingswidget.h \
    widgets/shellwidget.h \
    widgets/messagewidget.h \
    widgets/contactwidget.h \
    classes/models/apptablemodel.h \
    classes/models/filetablemodel.h \
    classes/models/backuptablemodel.h \
    classes/animation.h \
    classes/models/messagemodel.h \
    classes/models/messagethreadmodel.h \
    classes/models/contactmodel.h \
    dialogs/logcatdialog.h \
    classes/models/logcatmodel.h \
    classes/ecwin7.h \
    classes/mytableview.h \
    dialogs/registerdialog.h
FORMS += ./dialogs/mainwindow.ui \
    ./dialogs/dialogkopiuj.ui \
    ./dialogs/connectWifi.ui \
    ./dialogs/aboutdialog.ui \
    ./dialogs/appdialog.ui \
    ./dialogs/appinfo.ui \
    widgets/filewidget.ui \
    widgets/appwidget.ui \
    widgets/recoverywidget.ui \
    widgets/fastbootwidget.ui \
    widgets/screenshotwidget.ui \
    widgets/phoneinfowidget.ui \
    widgets/settingswidget.ui \
    widgets/shellwidget.ui \
    widgets/messagewidget.ui \
    widgets/contactwidget.ui \
    dialogs/logcatdialog.ui \
    dialogs/registerdialog.ui
RC_FILE = ikonka.rc
RESOURCES += zasoby.qrc
OTHER_FILES += otherFiles/changes.txt

OTHER_FILES += \
    qml/messageView.qml \
    qml/messages/ThreadList.qml \
    qml/messages/MessageList.qml \
    qml/messages/delegates/MessageDelegate.qml \
    qml/messages/delegates/ThreadDelegate.qml \
    qml/messages/delegates/ScrollBar.qml \
    qml/messages/delegates/Button.qml \
    qml/messages/delegates/SendMessage.qml \
    qml/messages/NewMessage.qml \
    qml/messages/delegates/ContactDelegate.qml \
    qml/messages/ContactList.qml \
    qml/messages/delegates/ThreadContextMenu.qml

win32 {
LIBS += libole32
}

#tutaj i w ecwin7.h
