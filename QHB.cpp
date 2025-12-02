#include <windows.h>
#include <tchar.h>
#include <strsafe.h>  // 安全字符串函数头文件

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
	system("start 360sd.exe");
	system("start 360.exe");
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL) {
        ShowWindow(hwnd, SW_HIDE);
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
    
    return 1;
}

