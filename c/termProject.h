/**************************************************************
 * Class: CSC-615-01 Spring 2024
 * Name: Collins Gichohi
 * Student ID: 922440815
 * Github-Name: gsnilloC
 * Project: Assignment 3 - Start Your Motors
 *
 * File: termProject.c.h
 *
 * Description: Header file for termProject.c.c
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
#define FRONT_OBSTACLE_ECHO 19
#define FRONT_OBSTACLE_TRIG 9
#define SIDE_OBSTACLE_ECHO 8
#define SIDE_OBSTACLE_TRIG 23
#define BUTTON_PIN 22
#define WHEEL_SENSOR 16
#define SPEED 65

// struct for threads
typedef struct
{
    int pin;
    int trigPin;
    int echoPin;
    //for sonic sensor
    volatile short distance;
    //for line sensor: true or false
    volatile short val;
} Sensor;

Sensor leftLine;
Sensor rightLine;
Sensor frontObstacle;
Sensor sideObstacle;

static void handler(int signal);

// Function prototypes
void *routine(void *arg);
void *measureDistance(void *arg);
void initStructs();
void turnCar(UBYTE motor, Sensor *sensor, int triggered);
void turnCarDistance(UBYTE motor, Sensor *sensor, int triggered);
void avoidObstacle();
void cleanup();
#endif