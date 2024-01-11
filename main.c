#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>


// Torus ring radius 
#define R1 2
// Distance from torus ring center to rotating axis
#define R2 5
// STOD is distance from screen to "3D" donut
#define STOD 100
// Field of view should be calculated according to upper values, but for now some random value :D (which fits good to my screen :D) (should be changed )
#define FOV 100*STOD/(R1+R2)*0.5
// Alpha is corner which helps to get ring point
#define ALPHA_SPACING 0.02
// Theta is corner which goes around 3D object + used to rotate on y
#define THETA_SPACING 0.01
// Just simplify using of PI :D 
#define PI M_PI


typedef struct{
    int height;
    int width;
} dimensions;


dimensions getTerminalDimensions();

int main() {
    
    dimensions terminalDimensions = getTerminalDimensions();

    // terminalDimensions.width = 100;
    // terminalDimensions.height =  100;

    char symbols[terminalDimensions.width][terminalDimensions.height + 10];
    int zBuffer[terminalDimensions.width][terminalDimensions.height + 10];
    
    // We need 2 more corners for x an z rotations, alpha and theta should find all point mappings always, 
    // but that 2 value can be changed after one movement is finished 
    double xRotation = 0;
    double zRotation = 0;
    
    printf("\x1b[2J");

    while(1){

        memset(symbols, ' ', sizeof(symbols));
        memset(zBuffer, 0, sizeof(zBuffer));

        double xRotationCos = cos(xRotation);
        double zRotationCos = cos(zRotation);

        double xRotationSin = sin(xRotation);
        double zRotationSin = sin(zRotation);
        
        for(double alpha = 0; alpha < 2 * PI; alpha += ALPHA_SPACING){
            
            double sinAlpha = sin(alpha); 
            double cosAlpha = cos(alpha);

            double circleX = R2 + R1 * cosAlpha;
            double circleY = R1 * sinAlpha;

            for(double theta = 0; theta < 2 * PI; theta += THETA_SPACING){  
                
                double sinTheta = sin(theta);
                double cosTheta = cos(theta);

                double x = circleX * (zRotationCos * cosTheta + xRotationSin * zRotationSin * sinTheta) - circleY * xRotationCos * zRotationSin;
                double y = circleX * (zRotationSin * cosTheta - zRotationCos * xRotationSin * sinTheta) + circleY * xRotationCos * zRotationCos;
                double z = STOD + circleX * xRotationCos * sinTheta + circleY * xRotationSin;

                // lets take (0, 1, -1) as light direction and make dot product of it and surface normal, to determine light strength and direction
                // light vector is (0, 1, -1) and it's not normalized so value of light is from -sqrt(2) to sqrt(2), therefore I divede l with sqrt(2)
                double light = cosTheta * cosAlpha * zRotationSin - xRotationCos * cosAlpha * sinTheta - xRotationSin * sinAlpha + zRotationCos * (xRotationCos * sinAlpha - cosAlpha * xRotationSin * sinTheta);
                // new range from -1 to 1
                light /= sqrt(2);

                int onScreenX = (terminalDimensions.width - (int) ((FOV * x) / (STOD + x))) / 2;
                int onScreenY = (terminalDimensions.height - (int) ((FOV * y) / (STOD + z))) / 2;

                if(light > 0){
                    
                    if(1 / z > zBuffer[onScreenX][onScreenY]){
                        
                        zBuffer[onScreenX][onScreenY] = 1 / z;
                        symbols[onScreenX][onScreenY] = ".,-~:;=!*#$@"[(int)(light * 12)];
                    }
                }
            }
        }

        printf("\x1b[d");
        for(int i = 0; i < terminalDimensions.height; i++){
            for(int j = 0; j < terminalDimensions.width; j++){
                printf("%c", symbols[j][i]);
            }
            printf("\n");
        }

        xRotation += 0.1;
        zRotation += 0.1;
        usleep(100000);
    }

    return 0;
   
}

dimensions getTerminalDimensions(){
    
    struct winsize w;
    dimensions terminalDimensions;
    
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminalDimensions.width = w.ws_col;
    terminalDimensions.height = w.ws_row;
    
    return terminalDimensions;
}
