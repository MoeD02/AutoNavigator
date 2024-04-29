/**************************************************************
 * Class: CSC-615-01 Spring 2024
 * Name: Collins Gichohi
 * Student ID: 922440815
 * Github-Name: gsnilloC
 * Project: Assignment 3 - Start Your Motors
 *
 * File: assignment3.h
 *
 * Description: This code controls a motor connected to a
 * Raspberry Pi, responding to button presses. Upon pressing
 * the button, it initiates a motor sequence: starting
 * at full speed, gradually slowing down, stopping, then
 * reversing direction while ramping up to full speed again.
 * The process showcases forward and backward motion with
 * speed control. System initialization sets up GPIO
 * configurations and interrupt handling for the button,
 * ensuring a clean shutdown on program exit or interruption.
 **************************************************************/
#include "assignment3.h"
volatile sig_atomic_t cleaned_up = 0;

int main(void)
{
    // Initialize system modules
    if (DEV_ModuleInit())
        exit(0); // Exit if initialization fails
    // Initialize GPIO library, exit with error if fails
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "pigpio initialization failed!\n");
        return 1;
    }
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    initStructs();
    // Initialize motor and set up GPIO pin 23 as input with pull-up resistor
    motorInit();
    pthread_t leftLineThread;
    pthread_t rightLineThread;
    pthread_t frontObstacleThread;
    // start threads
    pthread_create(&leftLineThread, NULL, routine, (void *)&leftLine);
    pthread_create(&rightLineThread, NULL, routine, (void*)&rightLine);
    pthread_create(&frontObstacleThread, NULL, routine, (void*)&frontObstacle);
    // testing
    // motorOn(FORWARD, MOTORB, 75);
    // motorOn(BACKWARD, MOTORA, 5);
    // sleep(5);
    // motorStop(MOTORB);
    // motorStop(MOTORA);
    /////
     while (!cleaned_up) {
        //straight line
        if(leftLine.val == 0 && rightLine.val == 0){
            motorOn(FORWARD, MOTORA, 50);
            motorOn(FORWARD, MOTORB, 50);
        }
        else if(leftLine.val != 0){
            turnCar(MOTORB, leftLine);
        }
        else if(rightLine.val != 0){
            turnCar(MOTORA, rightLine);
        }
    }
    
    // Clean up resources on program exit
    DEV_ModuleExit();
    gpioTerminate();

    return 0;
}

void *routine(void* arg){
    Sensor *sensor = (Sensor *)arg; 
    // read pin for sensor indefinetly.
    while(!cleaned_up){
        sensor->val = gpioRead(sensor->pin);
        usleep(100000);
        }
}

// Handler for SIGINT signal (Ctrl+C)
static void handler(int signal)
{
    printf("Motor Stop\r\n");
    // Stop the motor
    motorStop(MOTORA);
    motorStop(MOTORB);
    // change the flag to terminate loops and threads.
    cleaned_up = 1;
    printf("Interrupt... %d\n", signal);
    // Clean up module resources
    DEV_ModuleExit();
    // Terminate GPIO library
    gpioTerminate();
    // Exit program
    exit(0);
}

void initStructs(){
    leftLine.pin = LEFT_LINE_PIN;
    leftLine.val = 0; // 0 is no line
    rightLine.pin = RIGHT_LINE_PIN;
    rightLine.val = 0; // 0 is no line
    frontObstacle.pin = FRONT_OBSTACLE_PIN;
    frontObstacle.val = 1; // 1 is no obstacle
    pthread_t lineThread;
    pthread_t obstacleThread;
}

// turn car either left or right slowly: if turn right, stops motor left first.
void turnCar(UBYTE motor, Sensor sensor){
    UBYTE stopMotor;
    if(motor == MOTORA){
        stopMotor = MOTORB;
    }else{
        stopMotor = MOTORA;
    }
    motorStop(stopMotor);
    while (sensor.val == 1)
    {
        motorOn(FORWARD, motor, 35);
        printf("val: %d\n", sensor.val);
        printf("pin: %d\n", sensor.pin);
        usleep(1000);
    }
    
}