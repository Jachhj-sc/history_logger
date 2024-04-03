#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


extern int __argc;    // Count of arguments
extern char **__argv; // Array of arguments

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    int i = 0;

	// write to a file
    FILE *f = NULL;
	fopen_s(&f, "C:\\tmp\\log.txt", "a");

    if (f != NULL)
	{
        // Get the current time
        time_t now = time(NULL);
        struct tm t;
        localtime_s(&t, &now);

        // Format the time into a string
        char timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &t);

        fprintf(f, "%s start of h_logger.exe!\n", timeStr);
		fclose(f);
	}


    while (1) {
        // FreeConsole();
        //  printf("Hello, World!\n");
        //  system("pause");

        // run something like
        // tshark -i 6 -o "ssl.keylog_file:C:\tmp\sslkeys.log" -w c:\tmp\history\output.pcap
		
        Sleep(5000);
        i++;
    }
    return 0;
}
