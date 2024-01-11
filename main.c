#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>


// Torus ring radius 
#define R1 5
// Distance from torus ring center to rotating axis
#define R2 10
// STOD is distance from screen to "3D" donut
#define STOD 200
// Field of view, change 0.5 only :D 
#define FOV STOD*STOD/(R1+R2)*0.3
// Alpha is corner which helps to get ring point
#define ALPHA_SPACING 0.03
// Theta is corner which goes around 3D object + used to rotate on y
#define THETA_SPACING 0.08
// Just simplify using of PI :D 
#define PI M_PI


typedef struct{
    int height;
    int width;
} dimensions;

typedef struct{
    double x;
    double y;
    double z;
} vector;


dimensions getTerminalDimensions();
double getLightValue(vector light, double sinAlpha, double cosAlpha, double sinTheta, double cosTheta, double xRotationSin, double xRotationCos, double zRotationSin, double zRotationCos);

int main(){
    
    dimensions terminalDimensions = getTerminalDimensions();

    char symbols[terminalDimensions.width][terminalDimensions.height];
    int zBuffer[terminalDimensions.width][terminalDimensions.height];
    
    //declaring light direction vector here 
    vector lightVector;
    lightVector.x = 0;
    lightVector.y = 1;
    lightVector.z = -1;

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

                double light = getLightValue(lightVector, sinAlpha, cosAlpha, sinTheta, cosTheta, xRotationSin, xRotationCos, zRotationSin, zRotationCos);
                // light vector sometimes is not normalized, to normalize value of light to range (-1, 1), dividing it to length of light direction vector
                light /= sqrt(lightVector.x * lightVector.x + lightVector.y * lightVector.y + lightVector.z * lightVector.z);

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

double getLightValue(vector light, double sinAlpha, double cosAlpha, double sinTheta, double cosTheta, double xRotationSin, double xRotationCos, double zRotationSin, double zRotationCos){

    double firstLine = cosAlpha * (zRotationCos * cosTheta + zRotationSin * xRotationSin * sinTheta) - sinAlpha * zRotationSin * xRotationCos;
    double secondLine = cosAlpha * (zRotationSin * cosTheta - zRotationCos * xRotationSin * sinTheta) + sinAlpha * zRotationCos * xRotationCos;
    double thirdLine = cosAlpha * xRotationCos * sinTheta + sinAlpha * xRotationSin;

    return light.x * firstLine + light.y * secondLine + light.z * thirdLine;
}
