#include<windows.h>
using namespace std;

int main()
{
	HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口
    }
    while(true)
    {
    	MessageBox(NULL,"黑奴嗯嗯","便便公司",MB_OK);
	}
}
