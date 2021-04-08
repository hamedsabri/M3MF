// Copyright (C) 2021 Hamed Sabri
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "ThreeMFImport.h"

#define WANT_ATTACH_CONSOLE

#ifdef WANT_ATTACH_CONSOLE
#include <stdio.h>
#include <stdlib.h>
#endif

extern ClassDesc2* GetThreeMFImportDesc();
extern TCHAR* GetString(int id);

HINSTANCE hInstance;

// This function is called by Windows when the DLL is loaded.  This
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID /*lpvReserved*/)
{
#ifdef WANT_ATTACH_CONSOLE
    // attach a output console to see my debugging message
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
#endif

    if (fdwReason == DLL_PROCESS_ATTACH) {
        // MaxSDK::Util::UseLanguagePackLocale();
        // Hang on to this DLL's instance handle.
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hInstance);
        // DO NOT do any initialization here. Use LibInitialize() instead.
    }
    return (TRUE);
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec(dllexport) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}

// This function returns the number of plug-in classes this DLL
// TODO: Must change this number when adding a new class
__declspec(dllexport) int LibNumberClasses()
{
    return 1;
}

// This function returns the number of plug-in classes this DLL
__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
    switch (i) {
    case 0:
        return GetThreeMFImportDesc();
    default:
        return 0;
    }
}

// This function returns a pre-defined constant indicating the version of
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec(dllexport) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

// This function is called once, right after your plugin has been loaded by 3ds Max.
// Perform one-time plugin initialization in this method.
// Return TRUE if you deem your plugin successfully loaded, or FALSE otherwise. If
// the function returns FALSE, the system will NOT load the plugin, it will then call FreeLibrary
// on your DLL, and send you a message.
__declspec(dllexport) int LibInitialize(void)
{
    return TRUE;
}

// This function is called once, just before the plugin is unloaded.
// Perform one-time plugin un-initialization in this method."
// The system doesn't pay attention to a return value.
__declspec(dllexport) int LibShutdown(void)
{
    return TRUE;
}

TCHAR* GetString(int id)
{
    static TCHAR buf[256];

    if (hInstance) {
        return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
    }

    return NULL;
}
