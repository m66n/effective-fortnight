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
#include "utility.h"

#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

#include <sstream>
#include <boost/filesystem.hpp>


namespace fs = boost::filesystem;


template <typename T>
class SafeMalloc
{
public:
  SafeMalloc() : ptr_(nullptr) {}
  ~SafeMalloc() { Free(); }

  void Malloc(size_t size)
  {
    Free();
    ptr_ = malloc(size);
  }

  void Free()
  {
    if (ptr_) {
      free(ptr_);
      ptr_ = nullptr;
    }
  }

  operator T*() const { return reinterpret_cast<T*>(ptr_); }

  T* operator->() const { return reinterpret_cast<T*>(ptr_); }

private:
  void* ptr_;
};


template <typename T>
class SafeCoTaskMemPtr {
public:
  SafeCoTaskMemPtr() : ptr_(nullptr) {}
  ~SafeCoTaskMemPtr() { Free(); }

  void Free()
  {
    if (ptr_) {
      CoTaskMemFree(ptr_);
      ptr_ = nullptr;
    }
  }

  T* operator &() { return reinterpret_cast<T*>(&ptr_); }

  operator T() const { return reinterpret_cast<T>(ptr_); }

private:
  void* ptr_;
};


std::wstring util::Address::ToStr() const
{
  std::wstringstream ss;

  ss << description;
  ss << L" (";
  ss << static_cast<int>(a);
  ss << L".";
  ss << static_cast<int>(b);
  ss << L".";
  ss << static_cast<int>(c);
  ss << L".";
  ss << static_cast<int>(d);
  ss << L")";

  return ss.str();
}


bool util::LoadStringResource(HINSTANCE hInstance, UINT uID,
  std::wstring& value)
{
  value.clear();
  LPWSTR buffer;
  int numChars = LoadStringW(hInstance, uID, (LPWSTR)&buffer, 0);
  if (numChars) {
    // if successful, buffer points to a not necessarily null-terminated string
    value.assign(buffer, numChars);
    return true;
  }
  return false;
}


bool IsVirtual(const BYTE* buffer, DWORD length)
{
  if (length != 6) {
    return false;
  }

  /* VirtualBox v5 MAC begins with 0A-00-27 */
  if (buffer[0] == 0x0a &&
    buffer[1] == 0x00 &&
    buffer[2] == 0x27) {
    return true;
  }

  return false;
}


bool util::GetAddresses(Addresses& value)
{
  value.clear();

  SafeMalloc<IP_ADAPTER_ADDRESSES> addresses;
  ULONG size = 0;

  ULONG family = AF_INET;

  ULONG result = GetAdaptersAddresses(family, 0, NULL, addresses, &size);

  if (result == ERROR_BUFFER_OVERFLOW) {
    addresses.Malloc(size);
    result = GetAdaptersAddresses(family, 0, NULL, addresses, &size);
  }

  if (result != NO_ERROR) {
    return false;
  }

  PIP_ADAPTER_ADDRESSES curr = addresses;

  while (curr) {
    bool valid = (curr->OperStatus == IfOperStatusUp) &&
      (curr->IfType & IF_TYPE_ETHERNET_CSMACD) && 
      !IsVirtual(curr->PhysicalAddress, curr->PhysicalAddressLength);
    if (valid) {
      PIP_ADAPTER_UNICAST_ADDRESS unicast = curr->FirstUnicastAddress;
      while (unicast) {
        LPSOCKADDR sockaddr = unicast->Address.lpSockaddr;
        if (sockaddr->sa_family == AF_INET) {
          PSOCKADDR_IN endpoint = reinterpret_cast<PSOCKADDR_IN>(sockaddr);
          Address endpointAddress;
          endpointAddress.a = endpoint->sin_addr.S_un.S_un_b.s_b1;
          endpointAddress.b = endpoint->sin_addr.S_un.S_un_b.s_b2;
          endpointAddress.c = endpoint->sin_addr.S_un.S_un_b.s_b3;
          endpointAddress.d = endpoint->sin_addr.S_un.S_un_b.s_b4;
          endpointAddress.description = curr->Description;
          value.push_back(endpointAddress);
        }
        unicast = unicast->Next;
      }
    }
    curr = curr->Next;
  }

  return value.size() > 0;
}


std::wstring util::GetConfigPath(const wchar_t* appName, const wchar_t* fileName)
{
  SafeCoTaskMemPtr<PWSTR> folderPath;

  if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &folderPath) == S_OK) {
    fs::wpath configPath(folderPath);
    configPath /= appName;
    if (!fs::exists(configPath)) {
      if (!fs::create_directory(configPath)) {
        goto error;
      }
    }
    configPath /= fileName;
    return configPath.c_str();
  }

error:
  return L"";
}
