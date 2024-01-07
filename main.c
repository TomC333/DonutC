#include <windows.h>
#include <wincon.h>
#include <stdio.h>

typedef struct{
    int width;
    int height;
} screenDimensions;

screenDimensions getScreenDimensions();

int clearScreen();

int main() {

    int n = clearScreen();
    screenDimensions dim = getScreenDimensions();

    printf("Width %d\n", dim.width);
    printf("Height %d\n", dim.height);
    printf("N %d\n", n);
}

screenDimensions getScreenDimensions(){

    CONSOLE_SCREEN_BUFFER_INFO csbInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if(hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &csbInfo)){

        screenDimensions result = {
                csbInfo.srWindow.Right - csbInfo.srWindow.Left + 1,
                csbInfo.srWindow.Bottom - csbInfo.srWindow.Top + 1
        };

        return result;
    }

    printf("Error while trying to get terminal dimensions\n");
    return (screenDimensions) {-1, -1};
}


int clearScreen(){

    CONSOLE_SCREEN_BUFFER_INFO csbInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if(hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &csbInfo)) {

        DWORD written;
        FillConsoleOutputCharacter(hConsole, ' ', csbInfo.dwSize.X - csbInfo.dwSize.Y, (COORD){0, 0}, &written);
        SetConsoleCursorPosition(hConsole, (COORD) {0, 0});

        return 0;
    }

    printf("Error while trying to clear terminal window\n");
    return -1;
}

