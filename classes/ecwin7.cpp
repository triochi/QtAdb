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

#include "ecwin7.h"

// Windows only GUID definitions
#if defined(WIN7PROGRESS)
DEFINE_GUID(CLSID_TaskbarList,0x56fdf344,0xfd6d,0x11d0,0x95,0x8a,0x0,0x60,0x97,0xc9,0xa0,0x90);
DEFINE_GUID(IID_ITaskbarList3,0xea1afb91,0x9e28,0x4b86,0x90,0xE9,0x9e,0x9f,0x8a,0x5e,0xef,0xaf);
#endif

// Constructor: variabiles initialization
EcWin7::EcWin7()
{
#ifdef WIN7PROGRESS
    GetSystemInfo(&siSysInfo);
    mOverlayIcon = NULL;
#endif
}

// Init taskbar communication
void EcWin7::init(WId wid)
{
    mWindowId = wid;
#ifdef WIN7PROGRESS
    mTaskbarMessageId = RegisterWindowMessage(L"TaskbarButtonCreated");
#endif
}

// Windows event handler callback function
// (handles taskbar communication initial message)
#ifdef WIN7PROGRESS
bool EcWin7::winEvent(MSG * message, long * result)
{
    if (message->message == mTaskbarMessageId)
    {
        HRESULT hr = CoCreateInstance(CLSID_TaskbarList,
                                      0,
                                      CLSCTX_INPROC_SERVER,
                                      IID_ITaskbarList3,
                                      reinterpret_cast<void**> (&(mTaskbar)));
        *result = hr;
        return true;
    }
    return false;
}
#endif

// Set progress bar current value
void EcWin7::setProgressValue(int value, int max)
{
#ifdef WIN7PROGRESS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
        if (QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE", "") == "x86")
            mTaskbar->SetProgressValue(mWindowId, value, max);
#endif
}

// Set progress bar current state (active, error, pause, ecc...)
void EcWin7::setProgressState(ToolBarProgressState state)
{
#ifdef WIN7PROGRESS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
        if (QProcessEnvironment::systemEnvironment().value("PROCESSOR_ARCHITECTURE", "") == "x86")
            mTaskbar->SetProgressState(mWindowId, (TBPFLAG)state);
#endif
}

// Set new overlay icon and corresponding description (for accessibility)
// (call with iconName == "" and description == "" to remove any previous overlay icon)
void EcWin7::setOverlayIcon(QIcon icon, QString description)
{
#ifdef WIN7PROGRESS
    HICON oldIcon = NULL;
    if (mOverlayIcon != NULL) oldIcon = mOverlayIcon;
    if (icon.isNull())
    {
        mTaskbar->SetOverlayIcon(mWindowId, NULL, NULL);
        mOverlayIcon = NULL;
    }
    else
    {
        mOverlayIcon = (HICON) icon.pixmap(40).toWinHICON();
        mTaskbar->SetOverlayIcon(mWindowId, mOverlayIcon, description.toStdWString().c_str());
    }
    if ((oldIcon != NULL) && (oldIcon != mOverlayIcon))
    {
        DestroyIcon(oldIcon);
    }
#endif
}
