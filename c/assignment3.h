/**************************************************************
 * Class: CSC-615-01 Spring 2024
 * Name: Collins Gichohi
 * Student ID: 922440815
 * Github-Name: gsnilloC
 * Project: Assignment 3 - Start Your Motors
 *
 * File: assignment3.h
 *
 * Description: Header file for assignment3.c
 **************************************************************/
#ifndef __ASSIGNMENT3_H__
#define __ASSIGNMENT3_H__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>
#include "DEV_Config.h"
#include <time.h>
#include "DEV_Config.h"
#include "myMotorDriver.h"
#include <pthread.h>

#define LEFT_LINE_PIN 21
#define RIGHT_LINE_PIN 17
#define FRONT_OBSTACLE_PIN 27

//struct for threads
typedef struct {
    int pin;
    volatile short val;
} Sensor;

Sensor leftLine;
Sensor rightLine;
Sensor frontObstacle;

static void handler(int signal);

// Function prototypes
void *routine(void* arg);
void initStructs();
void turnCar(UBYTE motor, Sensor sensor);
#endif