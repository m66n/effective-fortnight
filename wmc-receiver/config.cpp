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
#include "config.h"

#include <boost/property_tree/json_parser.hpp>
#include <fstream>
#include <boost/filesystem.hpp>


namespace fs = boost::filesystem;


const wchar_t* KEY_NETWORK_INTERFACE = L"network interface";
const wchar_t* KEY_PORT = L"port";
const wchar_t* KEY_START_MINIMIZED = L"start minimized";
const wchar_t* KEY_MINIMIZE_TO_TRAY = L"minimize to tray";
const wchar_t* KEY_CLOSE_TO_TRAY = L"close to tray";

const wchar_t* REGKEY_RUN = (
  L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");


Config::Config() : dirty_(false) {}


Config::~Config()
{
  if (dirty_) {
    Save();
  }
}


bool Config::Load(const wchar_t* path)
{
  path_ = path;

  std::wifstream f;
  f.open(path_);

  if (!f) {
    dirty_ = true;
    return false;
  }

  try {
    pt::read_json(f, root_);
    f.close();
  }
  catch (const pt::json_parser_error& /* e */) {
    f.close();
    fs::remove(path_);
    dirty_ = true;
    return false;
  }

  return true;
}


bool Config::Save()
{
  std::wofstream fs;
  fs.open(path_);
  if (fs) {
    pt::write_json(fs, root_);
    fs.close();
    dirty_ = false;
    return true;
  }
  return false;
}


std::wstring Config::GetNetworkInterface()
{
  std::wstring value;

  try {
    value = root_.get<std::wstring>(KEY_NETWORK_INTERFACE);
  }
  catch (const pt::ptree_error& /* e */) {
    PutNetworkInterface(value.c_str());
  }

  return value;
}


void Config::PutNetworkInterface(const wchar_t* value, bool save)
{
  root_.put(KEY_NETWORK_INTERFACE, value);
  dirty_ = true;
  if (save) { Save(); }
}


int Config::GetPort()
{
  int value = 29092;

  try {
    value = root_.get<int>(KEY_PORT);
  }
  catch (const pt::ptree_error& /* e */) {
    PutPort(value);
  }

  return value;
}


void Config::PutPort(int value, bool save)
{
  root_.put(KEY_PORT, value);
  dirty_ = true;
  if (save) { Save(); }
}


bool Config::GetStartMinimized()
{
  int value = true;

  try {
    value = root_.get<bool>(KEY_START_MINIMIZED);
  }
  catch (const pt::ptree_error& /* e */) {
    PutStartMinimized(value);
  }

  return value;
}


void Config::PutStartMinimized(bool value, bool save)
{
  root_.put(KEY_START_MINIMIZED, value);
  dirty_ = true;
  if (save) { Save(); }
}


bool Config::GetStartWithWindows(const wchar_t* appName)
{
  CRegKey key;
  key.Open(HKEY_CURRENT_USER, REGKEY_RUN);
  return !GetRegPathString(key, appName).empty();
}


void Config::PutStartWithWindows(const wchar_t* appName, const wchar_t* appPath)
{
  CRegKey key;
  key.Open(HKEY_CURRENT_USER, REGKEY_RUN);

  if (!appPath) {
    key.DeleteValue(appName);
  }
  else {
    key.SetStringValue(appName, appPath);
  }
}


bool Config::GetMinimizeToTray()
{
  int value = true;

  try {
    value = root_.get<bool>(KEY_MINIMIZE_TO_TRAY);
  }
  catch (const pt::ptree_error& /* e */) {
    PutMinimizeToTray(value);
  }

  return value;
}


void Config::PutMinimizeToTray(bool value, bool save)
{
  root_.put(KEY_MINIMIZE_TO_TRAY, value);
  dirty_ = true;
  if (save) { Save(); }
}


bool Config::GetCloseToTray()
{
  int value = false;

  try {
    value = root_.get<bool>(KEY_CLOSE_TO_TRAY);
  }
  catch (const pt::ptree_error& /* e */) {
    PutCloseToTray(value);
  }

  return value;
}


void Config::PutCloseToTray(bool value, bool save)
{
  root_.put(KEY_CLOSE_TO_TRAY, value);
  dirty_ = true;
  if (save) { Save(); }
}


std::wstring Config::GetRegPathString(CRegKey& key, const wchar_t* queryValue)
{
  DWORD bufferSize = MAX_PATH + 1;
  std::vector<wchar_t> buffer(bufferSize);
  DWORD nChars = bufferSize;

  LONG result = key.QueryStringValue(queryValue, &buffer[0], &nChars);

  while (result == ERROR_MORE_DATA) {
    if (bufferSize > 0x7fff) {
      break;
    }
    bufferSize *= 2;
    buffer.resize(bufferSize);
    nChars = bufferSize;
    result = key.QueryStringValue(queryValue, &buffer[0], &nChars);
  }

  if (result != ERROR_SUCCESS) {
    return L"";
  }

  return &buffer[0];
}
