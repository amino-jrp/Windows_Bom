#include <windows.h>
#include <thread>
#include <random>
#include <vector>
#include <atomic>

// 全局变量，用于控制消息框数量和随机数生成
std::atomic<int> msgBoxCount(0);
std::random_device rd;
std::mt19937 gen(rd());

// 获取屏幕尺寸
void getScreenSize(int& width, int& height) {
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
}

// 生成指定范围内的随机数
int getRandom(int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// 移动窗口的函数 - 修复边缘反弹问题
void moveWindow(HWND hWnd, int screenWidth, int screenHeight) {
    RECT rect;
    // 获取消息框实际尺寸（之前用了父窗口的尺寸，这是错误的）
	HWND msgBoxWnd = FindWindowExW(NULL, NULL, L"#32770", L"\u516c\u53f8");
    
    // 确保找到消息框窗口
    if (!msgBoxWnd) {
        // 如果没找到，用父窗口继续尝试
        msgBoxWnd = hWnd;
    }
    
    // 获取窗口尺寸
    GetWindowRect(msgBoxWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // 初始随机速度
    int dx = getRandom(9, 10);
    int dy = getRandom(9, 10);
    
    // 随机初始方向
    if (getRandom(0, 1) == 0) dx = -dx;
    if (getRandom(0, 1) == 0) dy = -dy;
    
    // 随机改变方向的计时器
    int changeDirCounter = 0;
    int changeDirInterval = getRandom(10, 40); // 随机间隔(每20ms计数一次)
    
    while (IsWindowVisible(msgBoxWnd)) {
        GetWindowRect(msgBoxWnd, &rect);
        int x = rect.left;
        int y = rect.top;
        
        // 随机改变移动方向
        changeDirCounter++;
        if (changeDirCounter >= changeDirInterval) {
            // 随机改变X方向速度
            dx = getRandom(3, 10);
            if (getRandom(0, 1) == 0) dx = -dx;
            
            // 随机改变Y方向速度
            dy = getRandom(3, 10);
            if (getRandom(0, 1) == 0) dy = -dy;
            
            // 重置计数器和随机间隔
            changeDirCounter = 0;
            changeDirInterval = getRandom(10, 40);
        }
        
        // 计算新位置
        int newX = x + dx;
        int newY = y + dy;
        
        // 碰到左边界或右边界，X方向反弹
        if (newX <= 0 || newX + width >= screenWidth) {
            dx = -dx;
            // 调整位置，确保不会卡在边界外
            newX = (newX <= 0) ? 1 : screenWidth - width - 1;
            // 反弹后稍微调整速度增加随机性
            dx += getRandom(-2, 2);
            if (abs(dx) < 3) dx = (dx > 0) ? 3 : -3;
        }
        
        // 碰到上边界或下边界，Y方向反弹
        if (newY <= 0 || newY + height >= screenHeight) {
            dy = -dy;
            // 调整位置，确保不会卡在边界外
            newY = (newY <= 0) ? 1 : screenHeight - height - 1;
            // 反弹后稍微调整速度增加随机性
            dy += getRandom(-2, 2);
            if (abs(dy) < 3) dy = (dy > 0) ? 3 : -3;
        }
        
        // 移动窗口
        SetWindowPos(msgBoxWnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        
        // 控制移动速度，减小休眠时间让移动更流畅
        Sleep(20);
    }
}

// 显示消息框并启动移动线程
void showMessageBox() {
    msgBoxCount++;
    
    // 获取屏幕尺寸
    int screenWidth, screenHeight;
    getScreenSize(screenWidth, screenHeight);
    
    // 创建一个隐藏窗口作为消息框的父窗口
    HWND hParent = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL);
    
    // 随机初始位置
    int x = getRandom(0, screenWidth - 200);
    int y = getRandom(0, screenHeight - 150);
    
    // 设置父窗口初始位置
    SetWindowPos(hParent, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
    // 启动移动线程
    std::thread mover(moveWindow, hParent, screenWidth, screenHeight);
    mover.detach();
    
    // 显示消息框（使用ANSI版本以匹配字符串）
    MessageBoxA(hParent, "拉粑粑！", "公司", MB_OK | MB_SETFOREGROUND);
    
    // 消息框关闭后，销毁父窗口
    DestroyWindow(hParent);
    
    // 减少计数，并再创建两个新的消息框
    msgBoxCount--;
    std::thread t1(showMessageBox);
    t1.detach();
    std::thread t2(showMessageBox);
    t2.detach();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 初始化随机数生成器
    gen.seed(rd());
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口
    }
    // 初始创建5个消息框
    for (int i = 0; i < 1000; i++) {
        std::thread t(showMessageBox);
        t.detach();
		Sleep(100); // 稍微延迟避免同时创建
    }
    
    // 消息循环，保持程序运行
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

