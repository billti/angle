//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// system_utils_winuwp.cpp: Implementation of OS-specific functions for Windows UWP

#include "system_utils.h"

#include <stdarg.h>
#include <limits.h>
#include <windows.h>
#include <array>
#include <string>

namespace angle
{

bool SetEnvironmentVar(const char *variableName, const char *value)
{
    // Not supported for UWP
    return false;
}

std::string GetEnvironmentVar(const char *variableName)
{
    // Not supported for UWP
    return "";
}

std::wstring ConvertStringToWString(const std::string& input) {
    static_assert(sizeof(wchar_t) == 2, "Incorrect wchar_t size for Windows code");

    std::wstring result;
    if (input.empty()) return result;

    // Ensure the string (inc NULL) length fits in the int type the API uses
    if (input.length() > INT_MAX - 1) std::terminate();
    int input_length = static_cast<int>(input.length()) + 1;

    // Get the buffer size needed
    int utf16_chars =
        ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                              input.c_str(), input_length,
                              nullptr, 0);
    if (utf16_chars <= 0) {
        // Something failed (e.g. invalid chars, insufficient buffer, etc.)
        std::terminate();
     }

    result.reserve(utf16_chars);

    // Shouldn't need this cast since C++17, but current still errors without it
    wchar_t *data = const_cast<wchar_t *>(result.data());

    if (!::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                          input.c_str(), input_length,
                          data, utf16_chars)) {
        std::terminate();
    }

    return result;
}

class UwpLibrary : public Library
{
  public:
    UwpLibrary(const char *libraryName, SearchType searchType)
    {
        std::wstring wideBuffer = ConvertStringToWString(libraryName);

        switch (searchType)
        {
            case SearchType::ApplicationDir:
                mModule = LoadPackagedLibrary(wideBuffer.c_str(), 0);
                break;
            case SearchType::SystemDir:
                // Not supported in UWP
                break;
        }
    }

    ~UwpLibrary() override
    {
        if (mModule)
        {
            FreeLibrary(mModule);
        }
    }

    void *getSymbol(const char *symbolName) override
    {
        if (!mModule)
        {
            return nullptr;
        }

        return reinterpret_cast<void *>(GetProcAddress(mModule, symbolName));
    }

    void *getNative() const override { return reinterpret_cast<void *>(mModule); }

  private:
    HMODULE mModule = nullptr;
};

Library *OpenSharedLibrary(const char *libraryName, SearchType searchType)
{
    char buffer[MAX_PATH];
    int ret = snprintf(buffer, MAX_PATH, "%s.%s", libraryName, GetSharedLibraryExtension());

    if (ret > 0 && ret < MAX_PATH)
    {
        return new UwpLibrary(buffer, searchType);
    }
    else
    {
        fprintf(stderr, "Error loading shared library: 0x%x", ret);
        return nullptr;
    }
}

Library *OpenSharedLibraryWithExtension(const char *libraryName)
{
    // SystemDir is not implemented in UWP.
    fprintf(stderr, "Error loading shared library with extension.\n");
    return nullptr;
}
}  // namespace angle
