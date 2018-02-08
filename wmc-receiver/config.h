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

#include <string>
#include <boost/utility.hpp>
#include <boost/property_tree/ptree.hpp>


namespace pt = boost::property_tree;


class Config : boost::noncopyable
{
public:
  Config();
  ~Config();

  bool Load(const wchar_t* path);
  bool Save();

  std::wstring GetNetworkInterface();
  void PutNetworkInterface(const wchar_t* value, bool save = false);

  int GetPort();
  void PutPort(int value, bool save = false);

  bool GetStartMinimized();
  void PutStartMinimized(bool value, bool save = false);

  bool GetStartWithWindows();
  void PutStartWithWindows(bool value);

  bool GetMinimizeToTray();
  void PutMinimizeToTray(bool value, bool save = false);

  bool GetCloseToTray();
  void PutCloseToTray(bool value, bool save = false);

private:
  bool dirty_;
  std::wstring path_;
  pt::wptree root_;
};

