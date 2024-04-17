/**************************************************************
 * Class: CSC-615-01 Spring 2024
 * Name: Collins Gichohi
 * Student ID: 922440815
 * Github-Name: gsnilloC
 * Project: Assignment 3 - Start Your Motors
 *
 * File: myMotorDriver.h
 *
 * Description: Header file for myMotorDriver.c
 **************************************************************/
#include "myMotorDriver.h"
#include "Debug.h"

/**
 * Init everything necessary
 *
 */
void motorInit(void)
{
    PCA9685_Init(FRONT_I2C_ADDRESS);
    PCA9685_Init(BACK_I2C_ADDRESS);
    PCA9685_SetPWMFreq(DEFAULT_PWM_FREQ);
}

/**
 * Motor rotation.
 *
 * @param dir: forward and backward
 * @param speed: rotation speed
 *
 */
void motorOn(DIR direction, UBYTE motor, int speed)
{
    
    printf("Motor Speed = %d\r\n", speed);
    PCA9685_SetPwmDutyCycle(PWMA, speed);
    PCA9685_SetPwmDutyCycle(PWMB, speed);
    if (motor == MOTORA)
    {    
        if (direction == FORWARD)
    {
        printf("forward...\r\n");
        PCA9685_SetLevel(AIN1, 0);
        PCA9685_SetLevel(AIN2, 1);
    }
    else
    {
        printf("backward...\r\n");
        PCA9685_SetLevel(AIN1, 1);
        PCA9685_SetLevel(AIN2, 0);
    }
    }else{
        if (direction == FORWARD)
    {
        printf("forward...\r\n");
        PCA9685_SetLevel(BIN1, 1);
        PCA9685_SetLevel(BIN2, 0);
    }
    else
    {
        printf("backward...\r\n");
        PCA9685_SetLevel(BIN1, 0);
        PCA9685_SetLevel(BIN2, 1);
    }

    }
}

/**
 * Motor stop rotation.
 *
 */
void motorStop(UBYTE motor)
{
    if(motor == MOTORA)
    PCA9685_SetPwmDutyCycle(PWMA, 0);  
    else
    PCA9685_SetPwmDutyCycle(PWMB, 0);
}