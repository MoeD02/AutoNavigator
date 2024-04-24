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
#ifndef __MYMOTORDRIVER_H_
#define __MYMOTORDRIVER_H_

#include "DEV_Config.h"
#include "PCA9685.h"

#define PWMA PCA_CHANNEL_0
#define AIN1 PCA_CHANNEL_1
#define AIN2 PCA_CHANNEL_2
#define BIN1 PCA_CHANNEL_3
#define BIN2 PCA_CHANNEL_4
#define PWMB PCA_CHANNEL_5

#define PWMC PCA_CHANNEL_6
#define CIN1 PCA_CHANNEL_7
#define CIN2 PCA_CHANNEL_8
#define DIN1 PCA_CHANNEL_9
#define DIN2 PCA_CHANNEL_10
#define PWMD PCA_CHANNEL_11


#define FRONT_I2C_ADDRESS 0x41
#define BACK_I2C_ADDRESS 0x54

#define DEFAULT_PWM_FREQ 100

#define MOTORA 0
#define MOTORB 1 
#define MOTORC 2
#define MOTORD 3

typedef enum
{
    FORWARD = 1,
    BACKWARD = 0
} DIR;

void motorInit(void);
void motorOn(DIR dir, UBYTE motor, int speed);
void motorStop(UBYTE motor);

#endif