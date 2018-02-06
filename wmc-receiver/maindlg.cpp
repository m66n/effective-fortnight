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
#include "resource.h"
#include "maindlg.h"
#include "utility.h"


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
  return CWindow::IsDialogMessage(pMsg);
}


BOOL CMainDlg::OnIdle()
{
  UIUpdateChildWindows();
  return FALSE;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/,
  LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CenterWindow();

  HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
    ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
  SetIcon(hIcon, TRUE);

  HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
    ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
  SetIcon(hIconSmall, FALSE);

  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->AddMessageFilter(this);
  pLoop->AddIdleHandler(this);

  UIAddChildWindowContainer(m_hWnd);

  std::wstring appName;
  util::LoadStringResource(GetModuleHandle(NULL), IDS_APPNAME, appName);

  SetWindowTextW(appName.c_str());

  std::wstring configPath = util::GetConfigPath(appName.c_str(), L"config.json");

  DoDataExchange(FALSE);

  util::Addresses addresses;
  util::GetAddresses(addresses);

  for (auto const& a : addresses) {
    nicCombo_.AddString(a.ToStr().c_str());
  }

  trayIcon_.reset(new TrayIcon(IDR_MAINFRAME, appName.c_str()));
  trayIcon_->Attach(m_hWnd);

  return TRUE;
}


LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/,
  LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->RemoveMessageFilter(this);
  pLoop->RemoveIdleHandler(this);

  return 0;
}


LRESULT CMainDlg::OnClose(WORD /*wNotifyCode*/, WORD wID,
  HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CloseDialog(wID);
  return 0;
}


LRESULT CMainDlg::OnExit(WORD /*wNotifyCode*/, WORD wID,
  HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CloseDialog(wID);
  return 0;
}


void CMainDlg::CloseDialog(int nVal)
{
  DestroyWindow();
  ::PostQuitMessage(nVal);
}
