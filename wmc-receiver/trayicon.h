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

#include <shellapi.h>
#include <string>
#include <boost/utility.hpp>


const UINT RWM_TRAYICON = (
  RegisterWindowMessageW(L"A5712F24-FA18-47BC-8FA8-3C6B6C66C688"));

const UINT RWM_TASKBAR_CREATED = (
  RegisterWindowMessageW(L"TaskbarCreated"));


class SafeIcon : boost::noncopyable
{
public:
  explicit SafeIcon(UINT id);
  ~SafeIcon();

  UINT GetID() const { return id_; }
  inline operator HICON() { return hIcon_; }

private:
  UINT id_;
  HICON hIcon_;
};


class TrayIcon : boost::noncopyable
{
public:
  TrayIcon(UINT id, const wchar_t* text);
  ~TrayIcon();

  void Attach(HWND hWnd);
  void Remove();

private:
  SafeIcon icon_;
  std::wstring text_;
  BOOL attached_;
  NOTIFYICONDATAW nid_;
};
