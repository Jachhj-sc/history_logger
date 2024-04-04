#include <winsock2.h>

#include <Windows.h>
#include <iphlpapi.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #pragma comment(lib, "iphlpapi.lib")
// #pragma comment(lib, "ws2_32.lib")
#define LOG_FILE ".\\log.txt"

int checkInternetAccess(const char *ipAddress);

extern int __argc;    // Count of arguments
extern char **__argv; // Array of arguments

FILE *log_fp = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    uint8_t output_to_file = 0;
    char *output_file_name;

    fopen_s(&log_fp, LOG_FILE, "a");
    if (log_fp == NULL) {
        fprintf(log_fp, "Error opening log!\n");
        exit(1);
    }

    if (__argc >= 2) {
        if (strcmp(__argv[1], "-o") == 0) {
            output_to_file = 1;
            output_file_name = __argv[2];
        } else {
            fprintf(log_fp, "Invalid argument\n");
            fprintf(log_fp, "Usage: %s [-o <output file>]\n", __argv[0]);
            return 1;
        }
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(log_fp, "WSAStartup failed.\n");
        return 1;
    }

    ULONG outBufLen = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);
    if (pAdapterInfo == NULL) {
        fprintf(log_fp, "Error allocating memory for IP_ADAPTER_INFO\n");
        exit(1);
    }
    IP_ADAPTER_INFO *pAdapter = NULL;

    FILE *output = NULL;
    if (output_to_file) {
        fopen_s(&output, output_file_name, "w");

        if (output == NULL) {
            fprintf(log_fp, "Error opening file!\n");
            exit(1);
        }
    } else {
        output = stdout;
    }

    if (GetAdaptersInfo(pAdapterInfo, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = NULL;
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);
        if (pAdapterInfo == NULL) {
            fprintf(log_fp, "Error allocating memory for IP_ADAPTER_INFO\n");
            exit(1);
        }
    }

    if (GetAdaptersInfo(pAdapterInfo, &outBufLen) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0) {
                int hasInternet = checkInternetAccess(pAdapter->IpAddressList.IpAddress.String);
                if (hasInternet) {
                    fprintf(output, "%s\n", pAdapter->AdapterName);
                }
            }
            pAdapter = pAdapter->Next;
        }
    }

    free(pAdapterInfo);
	
	fflush(output);
	fflush(log_fp);

    fclose(output);
	fclose(log_fp);
    return 0;
}

int checkInternetAccess(const char *ipAddress)
{
    // Command line for ping
    char cmd[512];
    sprintf(cmd, "ping -n 1 -S %s google.com", ipAddress);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD dwExitCode = 0;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process
    if (!CreateProcess(NULL,             // No module name (use command line)
                       cmd,              // Command line
                       NULL,             // Process handle not inheritable
                       NULL,             // Thread handle not inheritable
                       FALSE,            // Set handle inheritance to FALSE
                       CREATE_NO_WINDOW, // No window for process
                       NULL,             // Use parent's environment block
                       NULL,             // Use parent's starting directory
                       &si,              // Pointer to STARTUPINFO structure
                       &pi)              // Pointer to PROCESS_INFORMATION structure
    ) {
        fprintf(log_fp, "CreateProcess failed (%lu).\n", GetLastError());
        return 0;
    }

    // Wait until child process exits
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Get the exit code of the process
    GetExitCodeProcess(pi.hProcess, &dwExitCode);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return dwExitCode == 0; // Return 1 (true) if ping command succeeded
}
