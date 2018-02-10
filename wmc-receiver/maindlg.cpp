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

#include <sstream>


const int MIN_PORT = 1024;
const int MAX_PORT = 65535;


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
  return CWindow::IsDialogMessage(pMsg);
}


BOOL CMainDlg::OnIdle()
{
  UIUpdateChildWindows();
  return FALSE;
}


int CMainDlg::GetInitialShowWindow(int nCmdShow)
{
  int value = nCmdShow;

  if (config_.GetStartMinimized()) {
    if (config_.GetMinimizeToTray()) {
      value = SW_HIDE;
    }
    else {
      value = SW_SHOWMINIMIZED;
    }
  }

  return value;
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

  util::LoadStringResource(GetModuleHandle(NULL), IDS_APPNAME, appName_);

  SetWindowTextW(appName_.c_str());

  std::wstring configPath =
    util::GetConfigPath(appName_.c_str(), L"config.json");
  config_.Load(configPath.c_str());

  util::GetAddresses(addresses_);

  DoDataExchange(FALSE);

  InitNICCombo();
  InitPortEdit();
  InitCheckButtons();

  trayIcon_.reset(new TrayIcon(IDR_MAINFRAME, appName_.c_str()));
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
  if (config_.GetCloseToTray()) {
    ShowWindow(SW_HIDE);
  }
  else {
    CloseDialog(wID);
  }

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


LRESULT CMainDlg::OnNICChange(WORD /*wNotifyCode*/, WORD /*wID*/,
  HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  void* itemData = nicCombo_.GetItemDataPtr(nicCombo_.GetCurSel());

  if (itemData != reinterpret_cast<void*>(-1)) {
    util::Address* a = reinterpret_cast<util::Address*>(itemData);
    config_.PutNetworkInterface(a->description.c_str(), true);
  }

  // todo: stop listening

  return 0;
}


void CMainDlg::InitNICCombo()
{
  std::wstring networkInterface = config_.GetNetworkInterface();

  int selectedIndex = -1;

  for (auto& a : addresses_) {
    int index = nicCombo_.AddString(a.ToStr().c_str());
    nicCombo_.SetItemDataPtr(index, &a);
    if (a.description == networkInterface) {
      selectedIndex = index;
    }
  }

  if (selectedIndex != -1) {
    nicCombo_.SetCurSel(selectedIndex);
  }
  else {
    void* itemData = nicCombo_.GetItemDataPtr(0);
    if (itemData != reinterpret_cast<void*>(-1)) {
      nicCombo_.SetCurSel(0);
      util::Address* a = reinterpret_cast<util::Address*>(itemData);
      config_.PutNetworkInterface(a->description.c_str(), true);
    }
  }
}


void CMainDlg::InitPortEdit()
{
  int port = config_.GetPort();
  std::wstringstream ss;
  ss << port;
  portEdit_.SetWindowText(ss.str().c_str());
}


void CMainDlg::InitCheckButtons()
{
  bool startMin = config_.GetStartMinimized();
  startMinBtn_.SetCheck(startMin ? BST_CHECKED : BST_UNCHECKED);

  bool startWin = config_.GetStartWithWindows(appName_.c_str());
  startWinBtn_.SetCheck(startWin ? BST_CHECKED : BST_UNCHECKED);

  bool minTray = config_.GetMinimizeToTray();
  minTrayBtn_.SetCheck(minTray ? BST_CHECKED : BST_UNCHECKED);

  bool closeTray = config_.GetCloseToTray();
  closeTrayBtn_.SetCheck(closeTray ? BST_CHECKED : BST_UNCHECKED);
}


LRESULT CMainDlg::OnPortChange(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  // todo: stop listening
  return 0;
}


LRESULT CMainDlg::OnPortKillFocus(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  // add 1 for null termination
  int textLength = portEdit_.GetWindowTextLengthW() + 1;
  std::vector<wchar_t> buffer(textLength);
  
  portEdit_.GetWindowTextW(&buffer[0], textLength);

  std::wstringstream ss(&buffer[0]);

  int port = 0;
  
  ss >> port;

  bool resetText = false;

  if (port < MIN_PORT) {
    port = MIN_PORT;
    resetText = true;
  }
  else if (port > MAX_PORT) {
    port = MAX_PORT;
    resetText = true;
  }

  if (resetText) {
    std::wstringstream ss;
    ss << port;
    portEdit_.SetWindowTextW(ss.str().c_str());
  }

  if (port != config_.GetPort()) {
    config_.PutPort(port, true);
  }

  return 0;
}


LRESULT CMainDlg::OnStartMinClicked(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  bool value = startMinBtn_.GetCheck() == BST_CHECKED;
  config_.PutStartMinimized(value, true);
  return 0;
}


LRESULT CMainDlg::OnStartWinClicked(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  bool value = startWinBtn_.GetCheck() == BST_CHECKED;
  config_.PutStartWithWindows(appName_.c_str(),
    value ? GetAppPath().c_str() : nullptr);
  return 0;
}


LRESULT CMainDlg::OnMinTrayClicked(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  bool value = minTrayBtn_.GetCheck() == BST_CHECKED;
  config_.PutMinimizeToTray(value, true);
  return 0;
}


LRESULT CMainDlg::OnCloseTrayClicked(WORD wNotifyCode, WORD wID,
  HWND hWndCtl, BOOL& bHandled)
{
  bool value = closeTrayBtn_.GetCheck() == BST_CHECKED;
  config_.PutCloseToTray(value, true);
  return 0;
}


LRESULT CMainDlg::OnSysCommand(UINT /*uMsg*/, WPARAM wParam,
  LPARAM /*lParam*/, BOOL& bHandled)
{
  bHandled = FALSE;

  if (SC_MINIMIZE == wParam && config_.GetMinimizeToTray())
  {
    ShowWindow(SW_HIDE);
    bHandled = TRUE;
  }

  return 0;
}


LRESULT CMainDlg::OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/,
  LPARAM lParam, BOOL& /*bHandled*/)
{
  switch (LOWORD(lParam))
  {
  case WM_LBUTTONDOWN:
    LONG style = GetWindowLong(GWL_STYLE);
    if (!IsWindowVisible()) {
      ShowWindow(SW_SHOW);
    }
    else if (style & WS_MINIMIZE) {
      ShowWindow(SW_RESTORE);
    }
    break;
  }

  return 0;
}


std::wstring CMainDlg::GetAppPath()
{
  DWORD bufferSize = MAX_PATH + 1;
  std::vector<wchar_t> buffer(bufferSize);
  while (!GetModuleFileNameW(NULL, &buffer[0], bufferSize)) {
    if (bufferSize > 0x7fff) {
      return L"";
    }
    bufferSize *= 2;
    buffer.resize(bufferSize);
  }
  return &buffer[0];
}
