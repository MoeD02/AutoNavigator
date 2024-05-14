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
    {
        exit(0); // Exit if initialization fails
    }

    // Initialize GPIO library, exit with error if it fails
    if (gpioInitialise() < 0)
    {
        fprintf(stderr, "pigpio initialization failed!\n");
        return 1;
    }

    // Register signal handlers for clean exit
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);

    // Configure GPIO pins
    gpioSetMode(LEFT_LINE_PIN, PI_INPUT);
    gpioSetMode(RIGHT_LINE_PIN, PI_INPUT);

    gpioSetMode(FRONT_OBSTACLE_ECHO, PI_INPUT);
    gpioSetMode(FRONT_OBSTACLE_TRIG, PI_OUTPUT);

    gpioSetMode(SIDE_OBSTACLE_ECHO, PI_INPUT);
    gpioSetMode(SIDE_OBSTACLE_TRIG, PI_OUTPUT);

    // Initialize sensors and motor driver
    initStructs();
    motorInit();

    // Create and start threads for sensor routines
    pthread_t leftLineThread, rightLineThread, frontObstacleThread, sideObstacleThread;
    pthread_create(&leftLineThread, NULL, routine, (void *)&leftLine);
    pthread_create(&rightLineThread, NULL, routine, (void *)&rightLine);
    pthread_create(&frontObstacleThread, NULL, measureDistance, (void *)&frontObstacle);
    pthread_create(&sideObstacleThread, NULL, measureDistance, (void *)&sideObstacle);

    // Control loop to manage motor based on sensor inputs
    while (!cleaned_up)
    {

        if (frontObstacle.distance < 20 && frontObstacle.distance > 5)
        {
            
            avoidObstacle();
        }
        else if (leftLine.val == 0 && rightLine.val == 0)
        {
            motorOn(FORWARD, MOTORA, SPEED);
            motorOn(FORWARD, MOTORB, SPEED);
        }
        else if (leftLine.val != 0)
        {
            turnCar(MOTORB, &leftLine, 1);
        }
        else if (rightLine.val != 0)
        {
            turnCar(MOTORA, &rightLine, 1);
        }
    }
    pthread_join(leftLineThread, NULL);
    pthread_join(rightLineThread, NULL);
    pthread_join(frontObstacleThread, NULL);
    pthread_join(sideObstacleThread, NULL);

    // Clean up resources on program exit
    DEV_ModuleExit();
    gpioTerminate();

    return 0;
}

void turnCar(UBYTE motor, Sensor *sensor, int triggered)
{
 printf("LINE SENSOR, TURNING");
    UBYTE stopMotor = (motor == MOTORA) ? MOTORB : MOTORA;
    motorOn(BACKWARD, stopMotor, 30);
   
        while (sensor->val == triggered)
        {
            motorOn(FORWARD, motor, 60);
            usleep(1000);
        }
      
}

void turnCarDistance(UBYTE motor, Sensor *sensor, int target)
{

    UBYTE stopMotor = (motor == MOTORA) ? MOTORB : MOTORA;
    motorOn(BACKWARD, stopMotor, 30);
 
            while (sensor->distance > target)
            {
                motorOn(FORWARD, motor, 70);
                usleep(1000);
            }
}

void avoidObstacle()
{
    motorStop(MOTORA);
    motorStop(MOTORB);
    sleep(1);

    //turn to its side
    while (sideObstacle.distance > 17)
    {
        turnCarDistance(MOTORB, &sideObstacle, 30);
    }

    while (sideObstacle.distance < 50)
    {
        motorOn(FORWARD, MOTORA, 60);
        motorOn(FORWARD, MOTORB, 60);
    } 

    // first corner
    motorOn(BACKWARD, MOTORB, 30);
    motorOn(FORWARD, MOTORA, 60);
    usleep(900000);

    while (sideObstacle.distance > 30)
    {
        motorOn(FORWARD, MOTORA, 50);
        motorOn(FORWARD, MOTORB, 50);
        printf("SIDE DISTANCE: %d\n", sideObstacle.distance);
    } 

    
    
    while (sideObstacle.distance < 40)
    {
        motorOn(FORWARD, MOTORA, 50);
        motorOn(FORWARD, MOTORB, 50);
        printf("SIDE DISTANCE: %d\n", sideObstacle.distance);
        // motorOn(FORWARD, MOTORA, 50);
        // motorOn(FORWARD, MOTORB, 50);
        // if(sideObstacle.distance < 10){
        //     printf("ADJUSTING\n");
        //     motorOn(BACKWARD, MOTORA, 30);
        //     motorOn(FORWARD, MOTORB, 60);
        //     usleep(4000000);
        // }
        // if(sideObstacle.distance > 20){
        //     printf("ADJUSTING\n");
        //      motorOn(BACKWARD, MOTORB, 30);
        //     motorOn(FORWARD, MOTORA, 60);
        //     usleep(4000000);
        // }
    } 
    sleep(1);
    // second corner
    motorOn(BACKWARD, MOTORB, 30);
    motorOn(FORWARD, MOTORA, 60);
    sleep(1);



    //sleep(3);
}

void initStructs()
{
    // Initialize line sensors
    leftLine.pin = LEFT_LINE_PIN;
    leftLine.val = 0;
    rightLine.pin = RIGHT_LINE_PIN;
    rightLine.val = 0;

    // Initialize front obstacle sensor
    frontObstacle.trigPin = FRONT_OBSTACLE_TRIG;
    frontObstacle.echoPin = FRONT_OBSTACLE_ECHO;
    frontObstacle.val = -1;

    // Initialize side obstacle sensor
    sideObstacle.trigPin = SIDE_OBSTACLE_TRIG;
    sideObstacle.echoPin = SIDE_OBSTACLE_ECHO;
    sideObstacle.val = -1;
}

void *routine(void *arg)
{
    Sensor *sensor = (Sensor *)arg;

    while (!cleaned_up)
    {
        sensor->val = gpioRead(sensor->pin);
        usleep(1000);
    }

    return NULL;
}

void *measureDistance(void *arg)
{
    Sensor *obstacleSensor = (Sensor *)arg;
    int startTick, endTick, diffTick;

    while (!cleaned_up)
    {
        // Ensure the trigger pin is low for a better first high pulse
        gpioWrite(obstacleSensor->trigPin, 0);
        gpioDelay(2);

        // Send ultrasonic pulse
        gpioWrite(obstacleSensor->trigPin, 1);
        gpioDelay(10);
        gpioWrite(obstacleSensor->trigPin, 0);

        // Wait for the start of the echo signal
        while (gpioRead(obstacleSensor->echoPin) == 0)
            ;

        startTick = gpioTick();

        // Wait for the end of the echo signal
        while (gpioRead(obstacleSensor->echoPin) == 1)
            ;

        endTick = gpioTick();

        // Calculate distance in centimeters
        diffTick = endTick - startTick;
        obstacleSensor->distance = diffTick / 58;

        usleep(100000); // Adjust delay for appropriate polling frequency
        printf("DISTANCE: %d,    ", obstacleSensor->distance);
        printf("PIN: %d\n", obstacleSensor->echoPin);
    }

    return NULL;
}

static void handler(int signal)
{
    printf("Motor Stop\r\n");

    // Stop both motors
    motorStop(MOTORA);
    motorStop(MOTORB);

    // Set flag to terminate loops and threads
    cleaned_up = 1;

    printf("Interrupt... %d\n", signal);

    // Clean up module resources and terminate GPIO
    DEV_ModuleExit();
    gpioTerminate();

    // Exit program
    exit(0);
}