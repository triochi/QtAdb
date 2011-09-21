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

#ifndef ECWIN7_H
#define ECWIN7_H

#include <QtGlobal>
#include <QWidget>
#include <QIcon>
#include <QProcessEnvironment>

// Windows only data definitions
#ifdef Q_WS_WIN
#define WIN7PROGRESS
#endif

#ifdef WIN7PROGRESS
#include <windows.h>
#include <initguid.h>
#define CMIC_MASK_ASYNCOK SEE_MASK_ASYNCOK

// Structs types and enums definitions for Windows 7 taskbar

typedef enum THUMBBUTTONMASK
{
    THB_BITMAP = 0x1,
    THB_ICON = 0x2,
    THB_TOOLTIP	= 0x4,
    THB_FLAGS = 0x8
} THUMBBUTTONMASK;

typedef enum THUMBBUTTONFLAGS
{
    THBF_ENABLED = 0,
    THBF_DISABLED = 0x1,
    THBF_DISMISSONCLICK	= 0x2,
    THBF_NOBACKGROUND = 0x4,
    THBF_HIDDEN	= 0x8,
    THBF_NONINTERACTIVE	= 0x10
} THUMBBUTTONFLAGS;

typedef struct THUMBBUTTON
{
    THUMBBUTTONMASK dwMask;
    UINT iId;
    UINT iBitmap;
    HICON hIcon;
    WCHAR szTip[260];
    THUMBBUTTONFLAGS dwFlags;
} THUMBBUTTON;
typedef struct THUMBBUTTON *LPTHUMBBUTTON;

typedef enum TBPFLAG
{
    TBPF_NOPROGRESS = 0,
    TBPF_INDETERMINATE = 0x1,
    TBPF_NORMAL = 0x2,
    TBPF_ERROR = 0x4,
    TBPF_PAUSED = 0x8
} TBPFLAG;

typedef IUnknown *HIMAGELIST;

// Taskbar interface
DECLARE_INTERFACE_(ITaskbarList3,IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface) (THIS_ REFIID riid,void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS) PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    // ITaskbarList
    STDMETHOD(HrInit) (THIS) PURE;
    STDMETHOD(AddTab) (THIS_ HWND hwnd) PURE;
    STDMETHOD(DeleteTab) (THIS_ HWND hwnd) PURE;
    STDMETHOD(ActivateTab) (THIS_ HWND hwnd) PURE;
    STDMETHOD(SetActiveAlt) (THIS_ HWND hwnd) PURE;
    STDMETHOD (MarkFullscreenWindow) (THIS_ HWND hwnd, int fFullscreen) PURE;
    // ITaskbarList3
    STDMETHOD (SetProgressValue) (THIS_ HWND hwnd, ULONGLONG ullCompleted, ULONGLONG ullTotal) PURE;
    STDMETHOD (SetProgressState) (THIS_ HWND hwnd, TBPFLAG tbpFlags) PURE;
    STDMETHOD (RegisterTab) (THIS_ HWND hwndTab,HWND hwndMDI) PURE;
    STDMETHOD (UnregisterTab) (THIS_ HWND hwndTab) PURE;
    STDMETHOD (SetTabOrder) (THIS_ HWND hwndTab, HWND hwndInsertBefore) PURE;
    STDMETHOD (SetTabActive) (THIS_ HWND hwndTab, HWND hwndMDI, DWORD dwReserved) PURE;
    STDMETHOD (ThumbBarAddButtons) (THIS_ HWND hwnd, UINT cButtons, LPTHUMBBUTTON pButton) PURE;
    STDMETHOD (ThumbBarUpdateButtons) (THIS_ HWND hwnd, UINT cButtons, LPTHUMBBUTTON pButton) PURE;
    STDMETHOD (ThumbBarSetImageList) (THIS_ HWND hwnd, HIMAGELIST himl) PURE;
    STDMETHOD (SetOverlayIcon) (THIS_ HWND hwnd, HICON hIcon, LPCWSTR pszDescription) PURE;
    STDMETHOD (SetThumbnailTooltip) (THIS_ HWND hwnd, LPCWSTR pszTip) PURE;
    STDMETHOD (SetThumbnailClip) (THIS_ HWND hwnd, RECT *prcClip) PURE;
};
typedef ITaskbarList3 *LPITaskbarList3;

// Windows only data d#endifefinitions - END
#endif

// ********************************************************************
// EcWin7 class - Windows 7 taskbar handling for Qt and MinGW

class EcWin7
{
public:

        // Initialization methods
    EcWin7();
    void init(WId wid);
#ifdef WIN7PROGRESS
    bool winEvent(MSG * message, long * result);
#endif
	// Overlay icon handling
	void setOverlayIcon(QIcon icon, QString description);

	// Progress indicator handling
	enum ToolBarProgressState {
	NoProgress = 0,
	Indeterminate = 1,
	Normal = 2,
	Error = 4,
	Paused = 8
    };
    void setProgressValue(int value, int max);
    void setProgressState(ToolBarProgressState state);

private:
    WId mWindowId;
#ifdef WIN7PROGRESS
    SYSTEM_INFO siSysInfo;

       // Copy the hardware information to the SYSTEM_INFO structure.

    UINT mTaskbarMessageId;
    ITaskbarList3 *mTaskbar;
    HICON mOverlayIcon;
#endif
};

#endif // ECWIN7_H
