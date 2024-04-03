#include <windows.h>

#include <psapi.h>
#include <stdint.h>
#include <stdio.h>
#include <tchar.h>

int str_isnumber(const char *str);
int isdigit(int c);

// Function to check if a process is running
int isProcessRunning(const char *processName)
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 0; // Unable to enumerate processes
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Check the name of each process
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

            // Get the process name
            if (hProcess) {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                }

                // Check if the process name matches the one we're looking for
                if (_stricmp(processName, szProcessName) == 0) {
                    CloseHandle(hProcess);
                    return 1; // Process found
                }

                CloseHandle(hProcess);
            }
        }
    }

    return 0; // Process not found
}

int str_isnumber(const char *str)
{
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

//int main(int argc, char *argv[])

extern int __argc;    // Count of arguments
extern char **__argv; // Array of arguments

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    if (__argc < 3) {
        printf("This program requires 2 arguments, the program to be a watchdog for, and how often to check\n");
        printf("Usage: %s <process name> <interval in ms>\n", __argv[0]);
        return 1;
    }

    uint32_t interval = 5000;

    if (!str_isnumber(__argv[2])) {
        printf("Interval must be a number\n");
        return 1;
    } else {
        interval = atoi(__argv[2]);
    }

    //FreeConsole();
    while (1) {
        if (!isProcessRunning(__argv[1])) {
            // printf("%s is not running.\n", __argv[1]);
			char systemcall[256];
            int result = 0;

            result = sprintf_s(systemcall, sizeof(systemcall), "%s", __argv[1]);//start /b 
            
            if (result > 0) {
                ShellExecute(NULL, "open", systemcall, NULL, NULL, SW_HIDE);// to open hidden
            } else {
                // printf("Error starting %s\n", __argv[1]);
            }
        } else {
            // printf("%s is running.\n", __argv[1]);
        }
        Sleep(interval); // Check every 5 seconds
    }

    return 0;
}
