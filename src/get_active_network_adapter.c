#include <winsock2.h>

#include <Windows.h>
#include <iphlpapi.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

int checkInternetAccess(const char *ipAddress);

int main(int argc, char *argv[])
{
    uint8_t output_to_file = 0;
    char *output_file_name;

    if (argc >= 2) {
        if (strcmp(argv[1], "-o") == 0) {
            output_to_file = 1;
            output_file_name = argv[2];
        } else {
            printf("Invalid argument\n");
            printf("Usage: %s [-o <output file>]\n", argv[0]);
            return 1;
        }
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    ULONG outBufLen = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory for IP_ADAPTER_INFO\n");
        exit(1);
    }
    IP_ADAPTER_INFO *pAdapter = NULL;

    FILE *output = NULL;
    if (output_to_file) {
        fopen_s(&output, output_file_name, "w");

        if (output == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
    }else{
		output = stdout;
	}

    if (GetAdaptersInfo(pAdapterInfo, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = NULL;
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory for IP_ADAPTER_INFO\n");
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
    fclose(output);
    return 0;
}

int checkInternetAccess(const char *ipAddress)
{
    char cmd[512];
    sprintf(cmd, "ping -n 1 -S %s google.com", ipAddress);
    return system(cmd) == 0; // 0 indicates success in system call
}
