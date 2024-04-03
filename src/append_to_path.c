#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int setSystemPath(const char *newPathSegment)
{
    HKEY hKey;
    const char *subkey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
    DWORD bufSize = 0;
    char *currentPath = NULL;
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

    // Check if the new path segment is already in the PATH
    if (strstr(currentPath, newPathSegment) == NULL) {
        // Append the new path segment to the current PATH
        char *newPath = (char *)malloc(bufSize + strlen(newPathSegment) + 1);
        if (newPath == NULL) {
            printf("Memory allocation failed\n");
            free(currentPath);
            RegCloseKey(hKey);
            return 1;
        }

        strcpy_s(newPath, bufSize + strlen(newPathSegment) + 1, currentPath);
        
		// Add a semicolon if the current PATH doesn't already end with one
        if (currentPath[strlen(currentPath) - 1] != ';') {
            strcat_s(newPath, bufSize + strlen(newPathSegment) + 1, ";");
        }
        
		strcat_s(newPath, bufSize + strlen(newPathSegment) + 1, newPathSegment);

        // Set the new PATH value
        setRes = RegSetValueEx(hKey, "Path", 0, REG_EXPAND_SZ, (LPBYTE)newPath, strlen(newPath) + 1);
        free(newPath);

        if (setRes != ERROR_SUCCESS) {
            printf("Error setting PATH\n");
            free(currentPath);
            RegCloseKey(hKey);
            return 1;
        }
        printf("New path added successfully.\n");
    } else {
        printf("The path segment is already in the PATH.\n");
    }

    // Cleanup
    free(currentPath);
    closeRes = RegCloseKey(hKey);
    if (closeRes != ERROR_SUCCESS) {
        printf("Error closing registry key\n");
        return 1;
    }

    // Notify the system of the change
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, 5000, NULL);

    return 0;
}

int main(int argc, char *argv[])
{	
	if(argc != 2) {
		printf("Usage: %s <path>\n", argv[0]);
		return 1;
	}
	
    const char *newPathSegment = argv[1];

	printf("Adding %s to the PATH\n", newPathSegment);

    if (setSystemPath(newPathSegment) == 0) {
        printf("System PATH updated successfully.\n");
    } else {
        printf("Failed to update System PATH.\n");
    }

    return 0;
}
