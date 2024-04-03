#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int removeDuplicatePaths()
{
    HKEY hKey;
    const char *subkey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
    DWORD bufSize = 0;
    char *currentPath = NULL;
    char *uniquePath = NULL;
    char *token = NULL;
    char *context = NULL;
    const char delimiters[] = ";";
    long openRes, queryRes, setRes, closeRes;

    // Open the registry key
    openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ | KEY_WRITE, &hKey);
    if (openRes != ERROR_SUCCESS) {
        printf("Error opening registry key\n");
        return 1;
    }

    // Query the size of the PATH variable
    queryRes = RegQueryValueEx(hKey, "Path", NULL, NULL, NULL, &bufSize);
    if (queryRes != ERROR_SUCCESS) {
        printf("Error querying size of PATH\n");
        RegCloseKey(hKey);
        return 1;
    }

    // Allocate memory to hold the current PATH
    currentPath = (char *)malloc(bufSize);
    if (currentPath == NULL) {
        printf("Memory allocation failed\n");
        RegCloseKey(hKey);
        return 1;
    }

    // Read the current PATH
    queryRes = RegQueryValueEx(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufSize);
    if (queryRes != ERROR_SUCCESS) {
        printf("Error reading PATH\n");
        free(currentPath);
        RegCloseKey(hKey);
        return 1;
    }

    // Allocate memory for the unique PATH
    uniquePath = (char *)malloc(bufSize);
    if (uniquePath == NULL) {
        printf("Memory allocation failed\n");
        free(currentPath);
        RegCloseKey(hKey);
        return 1;
    }
    uniquePath[0] = '\0'; // Initialize the unique path string

    // Tokenize and process each path entry
    token = strtok_s(currentPath, delimiters, &context);
    while (token != NULL) {
        if (!strstr(uniquePath, token)) { // Check if the token is already in the uniquePath
            if (strlen(uniquePath) > 0) {
                strcat_s(uniquePath, bufSize, ";");
            }
            strcat_s(uniquePath, bufSize, token);
        }
        token = strtok_s(NULL, delimiters, &context);
    }

    // Set the new unique PATH value
    setRes = RegSetValueEx(hKey, "Path", 0, REG_EXPAND_SZ, (LPBYTE)uniquePath, strlen(uniquePath) + 1);
    if (setRes != ERROR_SUCCESS) {
        printf("Error setting PATH\n");
    } else {
        printf("PATH updated successfully with unique entries.\n");
    }

    // Cleanup
    free(currentPath);
    free(uniquePath);
    closeRes = RegCloseKey(hKey);
    if (closeRes != ERROR_SUCCESS) {
        printf("Error closing registry key\n");
    }

    // Notify the system of the change
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, 5000, NULL);

    return (setRes == ERROR_SUCCESS) ? 0 : 1;
}

int main()
{
    if (removeDuplicatePaths() == 0) {
        printf("Successfully removed duplicate paths.\n");
    } else {
        printf("Failed to remove duplicate paths.\n");
    }

    return 0;
}
