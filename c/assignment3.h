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

#define BUTTON_PIN 23

void handler(int signal);
void buttonCallback(int gpio, int level, uint32_t tick);

#endif