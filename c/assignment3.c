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

// Handler for SIGINT signal (Ctrl+C)
void handler(int signal)
{
    // Print message indicating motor stop
    printf("Motor Stop\r\n");
    // Stop the motor
    motorStop();
    // Clean up module resources
    DEV_ModuleExit();
    // Terminate GPIO library
    gpioTerminate();

    // Exit program
    exit(0);
}

// Function for button press events
void buttonFunction(int gpio, int level, uint32_t tick)
{
    // Check if the button press is detected (PI_LOW indicates pressed state)
    if (level == PI_LOW)
    {
        // Start the motor in forward direction at full speed
        printf("Motor forward at 100%%\r\n");
        motorOn(FORWARD, 100);
        // Wait for 2 seconds
        gpioSleep(PI_TIME_RELATIVE, 2, 0);

        // Loop to gradually decrease the motor speed to 15%
        for (int speed = 100; speed >= 15; speed -= 5)
        {
            motorOn(FORWARD, speed);
            // Pause for 100ms between each speed adjustment
            gpioSleep(PI_TIME_RELATIVE, 0, 100 * 1000);
        }

        // Stop the motor and notify
        printf("Motor stopping...\r\n");
        motorStop();
        // Wait for 1 second
        gpioSleep(PI_TIME_RELATIVE, 1, 0);

        // Loop to gradually increase the motor speed to maximum in reverse direction
        for (int speed = 0; speed <= 100; speed += 5)
        {
            motorOn(BACKWARD, speed);
            // Pause for 100ms between each speed adjustment
            gpioSleep(PI_TIME_RELATIVE, 0, 100 * 1000);
        }
    }
}

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
    printf("\n\nPress button to start motor.\n");

    // Initialize motor and set up GPIO pin 23 as input with pull-up resistor
    motorInit();
    gpioSetMode(BUTTON_PIN, PI_INPUT);
    gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP);

    int initialButtonState = 1; // Assume button is not pressed initially
    signal(SIGINT, handler);

    while (1)
    {
        int currentButtonState = gpioRead(BUTTON_PIN); // Read the button state
        if (currentButtonState == PI_LOW && initialButtonState == PI_HIGH)
        {
            // Button was just pressed
            buttonFunction(BUTTON_PIN, currentButtonState, 0); // Call the function
        }

        initialButtonState = currentButtonState; // Update the previous state for the next loop iteration
        gpioDelay(10000);
    }

    // Clean up resources on program exit
    DEV_ModuleExit();
    gpioTerminate();

    return 0;
}
