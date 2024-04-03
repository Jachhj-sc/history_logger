#include <stdio.h>
#include <windows.h>

//int main()

extern int __argc;    // Count of arguments
extern char **__argv; // Array of arguments

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    int i = 0;
    while (1) {
        // FreeConsole();
        //  printf("Hello, World!\n");
        //  system("pause");
        Sleep(5000);
        i++;
    }
    return 0;
}
