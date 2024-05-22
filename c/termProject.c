/**************************************************************
 * Class: CSC-615-01 Spring 2024
 * Name: Collins Gichohi, Yakoub Alkabsh, Mohammad Dahbour, Diego 
 * Flores
 * Student ID: 922440815
 * Github-Name: gsnilloC
 * Project: Final Project
 *
 * File: termProject.c
 *
 * Description: This file is the main controller for the PI.
 * It utilizes threads to read from many pins, controlling
 * the turning of the car based of the values read from the
 * Sensors. It also manages going around an obstacle.
 **************************************************************/
#include "termProject.h"

volatile sig_atomic_t cleaned_up = 0;
//threads for all the sensors
pthread_t leftLineThread, rightLineThread, frontObstacleThread, sideObstacleThread;
volatile int buttonPressed = 0;

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

    // Configure the button pin
    gpioSetMode(BUTTON_PIN, PI_INPUT);
    // Enable pull-up resistor
    gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP); 

    printf("Waiting for button press...\n");

    while (!buttonPressed)
    {
        int currentButtonState = gpioRead(BUTTON_PIN); // Read the button state
        if (currentButtonState == PI_LOW)
        {
            // Button was just pressed
            buttonPressed = 1;
            printf("BUTTON PRESSED!\n");
        }
        gpioDelay(10000);// 10 ms delay
    }

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
    pthread_create(&leftLineThread, NULL, routine, (void *)&leftLine);
    pthread_create(&rightLineThread, NULL, routine, (void *)&rightLine);
    pthread_create(&frontObstacleThread, NULL, measureDistance, (void *)&frontObstacle);
    pthread_create(&sideObstacleThread, NULL, measureDistance, (void *)&sideObstacle);

    // Control loop to manage motor based on sensor inputs
    while (!cleaned_up)
    {
        printf("front obstacle distance: %d\n", frontObstacle.distance);
        //if obstacle, forget about line sensing and avoid obstacle
        if (frontObstacle.distance < 17 && frontObstacle.distance > 5)
        {
            avoidObstacle();
        }
        // sensors do not sense line: go straight
        else if (leftLine.val == 0 && rightLine.val == 0)
        {
            motorOn(FORWARD, MOTORA, SPEED);
            motorOn(FORWARD, MOTORB, SPEED);
        }
        // turn left until sensor is clear
        else if (leftLine.val != 0)
        {
            turnCar(MOTORB, &leftLine);
        }
        // turn right until sensor is clear
        else if (rightLine.val != 0)
        {
            turnCar(MOTORA, &rightLine);
        }
    }

    // Wait for threads to finish
    pthread_join(leftLineThread, NULL);
    pthread_join(rightLineThread, NULL);
    pthread_join(frontObstacleThread, NULL);
    pthread_join(sideObstacleThread, NULL);

    return 0;
}

//manages wheel turning for a specific amount
void avoidTurn(int tick)
{
    int pin = WHEEL_SENSOR;
    gpioSetMode(WHEEL_SENSOR, PI_INPUT);
    int val = 0;
    motorOn(BACKWARD, MOTORB, 30);
    motorOn(FORWARD, MOTORA, 60);
    int flick = gpioRead(pin); // Initial read of the pin
    int prevFlick = flick;     // Store the initial state of flick

    while (val < tick)
    {
        flick = gpioRead(pin); // Read the current state of the pin
        if (flick != prevFlick)
        {                      // Check if the state has changed
            val++;             // Increment val
            prevFlick = flick; // Update the previous state
            printf("val: %d\n", val);
        }
    }
    printf("Turn Finished\n");
    motorStop(MOTORA);
    motorStop(MOTORB);
}

// interrupt handler, clean up libraries and join threads.
void handler(int signal)
{
    printf("Motor Stop\r\n");

    // Stop both motors
    motorStop(MOTORA);
    motorStop(MOTORB);

    printf("Interrupt... %d\n", signal);
    // Perform cleanup
    cleanup();

    // Exit program
    exit(0);
}

void cleanup()
{
    cleaned_up = 1;
    buttonPressed = 1;

    pthread_cancel(leftLineThread);
    pthread_cancel(rightLineThread);
    pthread_cancel(frontObstacleThread);
    pthread_cancel(sideObstacleThread);

    pthread_join(leftLineThread, NULL);
    pthread_join(rightLineThread, NULL);
    pthread_join(frontObstacleThread, NULL);
    pthread_join(sideObstacleThread, NULL);

    // Clean up resources on program exit
    DEV_ModuleExit();
    gpioTerminate();
}

void turnCar(UBYTE motor, Sensor *sensor)
{
    // printf("LINE SENSOR, TURNING\n");
    UBYTE stopMotor = (motor == MOTORA) ? MOTORB : MOTORA;
    motorOn(BACKWARD, stopMotor, 35);

    while (sensor->val == 1)
    {
        motorOn(FORWARD, motor, SPEED);
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

    // turn to its side
    printf("side obstacle: %d\n", sideObstacle.distance);
    while (sideObstacle.distance > 13 && !cleaned_up)
    {
        turnCarDistance(MOTORB, &sideObstacle, 30);
    }
    // go straight until sensor's range is too large
    while (sideObstacle.distance < 50)
    {
        motorOn(FORWARD, MOTORA, 60);
        motorOn(FORWARD, MOTORB, 60);
    }
    // first corner
    avoidTurn(25);
    //small wait for pins
    usleep(500000);
    //straight
    motorOn(FORWARD, MOTORA, 50);
    motorOn(FORWARD, MOTORB, 50);
    usleep(3000000);
    // go straight until we need to turn for next corner
    while (sideObstacle.distance < 44)
    {
        motorOn(FORWARD, MOTORA, 55);
        motorOn(FORWARD, MOTORB, 55);
        printf("SIDE DISTANCE2: %d\n", sideObstacle.distance);
    }
    //  second corner
    avoidTurn(23);
    // go back in line the correct way
    while (leftLine.val == 0)
    {
        motorOn(FORWARD, MOTORA, 60);
        motorOn(FORWARD, MOTORB, 60);
    }
    while (leftLine.val == 1)
    {
        turnCar(MOTORB, &leftLine);
    }
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

// for line sensor and wheel sensor
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
// thread function for sonic sensor
void *measureDistance(void *arg)
{
    Sensor *obstacleSensor = (Sensor *)arg;
    int startTick, endTick, diffTick;

    while (!cleaned_up)
    {
        // Ensure the trigger pin is low for a better first high pulse
        gpioWrite(obstacleSensor->trigPin, 0);
        gpioDelay(2); // Added delay to ensure the line is low for at least 2 microseconds

        // Send ultrasonic pulse using gpioTrigger
        gpioTrigger(obstacleSensor->trigPin, 10, 1); // Changed to use gpioTrigger for sending pulse

        // Wait for the start of the echo signal
        while (gpioRead(obstacleSensor->echoPin) == 0 && !cleaned_up);

        startTick = gpioTick();

        // Wait for the end of the echo signal
        while (gpioRead(obstacleSensor->echoPin) == 1 && !cleaned_up);

        endTick = gpioTick();

        // Calculate distance in centimeters
        diffTick = endTick - startTick;
        obstacleSensor->distance = diffTick / 58;

        usleep(60000); // Adjust delay for appropriate polling frequency
    }
    return NULL;
}
