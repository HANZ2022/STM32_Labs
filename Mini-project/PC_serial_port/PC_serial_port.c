#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdint.h>

int main()
{
    HANDLE hCom;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytesWritten;
    DWORD bytesRead;
    char inputBuffer[100];
    char readBuffer[100];

    // Open COM3
    hCom = CreateFile(L"COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hCom == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening COM3\n");
        return 1;
    }

    // Set serial Port
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hCom, &dcbSerialParams)) {
        fprintf(stderr, "Error getting current serial port settings\n");
        CloseHandle(hCom);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hCom, &dcbSerialParams)) {
        fprintf(stderr, "Error setting serial port settings\n");
        CloseHandle(hCom);
        return 1;
    }

    //Setting over time
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hCom, &timeouts)) {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hCom);
        return 1;
    }

    int counter = 0;
    // Get user input
    int sig = 0;

    while (1) {

        if (_kbhit()) { // check keyboard hitting
            char ch = _getch(); // read keyboard
            inputBuffer[counter] = ch;
            counter++;
           

            if (ch == '\r')
            {
                inputBuffer[counter] = '\0';

                printf("Sennding %s\n", inputBuffer);
                if (!WriteFile(hCom, inputBuffer, strlen(inputBuffer), &bytesWritten, NULL)) {
                    fprintf(stderr, "Error writing to COM3\n");
                    break;
                }
                ZeroMemory(inputBuffer, sizeof(inputBuffer));
                counter = 0;
            }
        }

        // 从串行端口读取响应
        DWORD dwErrors;
        COMSTAT stat;

        if (ClearCommError(hCom, &dwErrors, &stat)) {
            if (stat.cbInQue > 0) {
                if (!ReadFile(hCom, readBuffer, min(sizeof(readBuffer) - 1, stat.cbInQue), &bytesRead, NULL)) {
                    fprintf(stderr, "Error reading from COM3\n");
                    break;
                }

 
                
               
                for (int i = 0; i < bytesRead; i++)
                {
                    switch ((uint8_t)readBuffer[i])
                    {
                    case 252:
                        printf("Depth: ");
                        sig = 0;
                        break;

                    case 253:
                        printf("Oxygen: ");
                        sig = 1;
                        break;

                    case 254:
                        printf("Temperature: ");
                        sig = 2;
                        break;

                    case 255:
                        printf("PH: ");
                        sig = 3;
                        break;
                    case 251:
                        printf("NANA ");
                        if (sig == 3)
                        {
                            printf("\n");
                            sig = 0;
                        }
                        break;

                    default:
                        switch (sig)
                        {
                            case 0: //Depth
                                printf("%um | ", (uint8_t)readBuffer[i]);
                                sig = 0;
                                break;
                            case 1: //Oxygen
                                printf("%u.%umg/L | ", (uint8_t)readBuffer[i] / 10, (uint8_t)readBuffer[i] % 10);
                                sig = 0;
                                break;
                            case 2: //Temperature
                                printf("%u ^C| ", (uint8_t)readBuffer[i]);
                                sig = 0;
                                break;
                            case 3: //PH
                                printf("%u.%u |\n", (uint8_t)readBuffer[i] / 10, (uint8_t)readBuffer[i] % 10);
                                sig = 0;
                                break;
                        }
   
                        break;
                    }
                }


                

                if (bytesRead > 0)
                {
                    ZeroMemory(readBuffer, sizeof(readBuffer));
                    bytesRead = 0;
                }
            }
        }
    }

    // 关闭串行端口
    CloseHandle(hCom);
    return 0;
}
