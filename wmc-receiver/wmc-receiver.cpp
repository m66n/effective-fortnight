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


CAppModule _Module;


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  CMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CMainDlg dlgMain;

  if (dlgMain.Create(NULL) == NULL)
  {
    ATLTRACE(_T("Main dialog creation failed!\n"));
    return 0;
  }

  dlgMain.ShowWindow(dlgMain.GetInitialShowWindow(nCmdShow));

  int nRet = theLoop.Run();

  _Module.RemoveMessageLoop();
  return nRet;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
  LPTSTR lpstrCmdLine, int nCmdShow)
{
  HRESULT hRes = ::CoInitialize(NULL);
  ATLASSERT(SUCCEEDED(hRes));

  AtlInitCommonControls(ICC_BAR_CLASSES);

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  int nRet = Run(lpstrCmdLine, nCmdShow);

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}
