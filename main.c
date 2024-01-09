#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

void drawSquare(int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("* ");
        }
        printf("\n");
    }
}


int main() {
    

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int terminalWidth = w.ws_col;
    int terminalHeight = w.ws_row;

    int squareSize = 10;

    for(int i = 0; i < 10; i++){
         int startX = (terminalWidth - squareSize) / 2;
        int startY = (terminalHeight - squareSize) / 2;
        system("clear");
        printf("\033[%d;%dH", startY, startX);
        drawSquare(squareSize);
        squareSize += 1;
    }

    return 0;
   
}