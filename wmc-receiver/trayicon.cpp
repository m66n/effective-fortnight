/*
MIT License

Copyright(c) 2018 Michael Chapman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "stdafx.h"
#include "trayicon.h"


SafeIcon::SafeIcon(UINT id) : id_(id),
hIcon_(reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL),
  MAKEINTRESOURCE(id), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
  GetSystemMetrics(SM_CYSMICON), LR_LOADTRANSPARENT)))
{
}


SafeIcon::~SafeIcon()
{
  DestroyIcon(hIcon_);
}


TrayIcon::TrayIcon(UINT id, const wchar_t* text) :
  icon_(id), text_(text), attached_(FALSE)
{
  ZeroMemory(&nid_, sizeof(nid_));
  nid_.cbSize = sizeof(nid_);
}


TrayIcon::~TrayIcon()
{
  Remove();
}


void TrayIcon::Attach(HWND hWnd)
{
  if (attached_) {
    Remove();
  }

  nid_.hWnd = hWnd;
  nid_.uID = icon_.GetID();
  nid_.hIcon = icon_;
  nid_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  nid_.uCallbackMessage = RWM_TRAYICON;

  wcscpy_s(nid_.szTip, _countof(nid_.szTip), text_.c_str());

  attached_ = Shell_NotifyIconW(NIM_ADD, &nid_);
}


void TrayIcon::Remove()
{
  attached_ = Shell_NotifyIconW(NIM_DELETE, &nid_) ? FALSE : TRUE;
}
