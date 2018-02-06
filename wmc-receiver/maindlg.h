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

#pragma once

#include <memory>

#include "trayicon.h"
#include "config.h"


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
  public CMessageFilter, public CIdleHandler, public CWinDataExchange<CMainDlg>
{
public:
  enum { IDD = IDD_MAINDLG };

  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL OnIdle();

  BEGIN_UPDATE_UI_MAP(CMainDlg)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP(CMainDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    COMMAND_ID_HANDLER(IDCANCEL, OnClose)
    COMMAND_ID_HANDLER(ID_EXIT, OnExit)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CMainDlg)
    DDX_CONTROL_HANDLE(IDC_NIC, nicCombo_)
  END_DDX_MAP()

private:
  std::unique_ptr<TrayIcon> trayIcon_;

  CComboBox nicCombo_;

  LRESULT OnInitDialog(UINT uMsg, WPARAM wParam,
    LPARAM lParam, BOOL& bHandled);
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam,
    LPARAM lParam, BOOL& bHandled);
  LRESULT OnClose(WORD wNotifyCode, WORD wID,
    HWND hWndCtl, BOOL& bHandled);
  LRESULT OnExit(WORD wNotifyCode, WORD wID,
    HWND hWndCtl, BOOL& bHandled);

  /* Handler prototypes:
  LRESULT MessageHandler(UINT uMsg, WPARAM wParam,
    LPARAM lParam, BOOL& bHandled)
  LRESULT CommandHandler(WORD wNotifyCode, WORD wID,
    HWND hWndCtl, BOOL& bHandled)
  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh,
    BOOL& bHandled)
  */

  void CloseDialog(int nVal);
};
