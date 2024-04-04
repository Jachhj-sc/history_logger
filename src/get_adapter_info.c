#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#pragma comment(lib, "iphlpapi.lib")

int main()
{
    DWORD dwRetVal = 0;
    ULONG outBufLen = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);

    // Get the required buffer size
    if (GetAdaptersInfo(pAdapterInfo, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outBufLen);
    }

    // Get the adapter information
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &outBufLen)) == NO_ERROR) {
        IP_ADAPTER_INFO *pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("Description: %s; ID: %s; IP Address: %s\n",
                   pAdapter->Description,
                   pAdapter->AdapterName,
                   pAdapter->IpAddressList.IpAddress.String);

            pAdapter = pAdapter->Next;
        }
    }

    if (pAdapterInfo) {
        free(pAdapterInfo);
    }

    return 0;
}
