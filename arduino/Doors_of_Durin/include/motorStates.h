#ifndef _motorStates_h
#define _motorStates_h

  #pragma once
  #include <Arduino.h>

// State definitions
#define MOTOR_STATE_INIT 1
#define MOTOR_STATE_CLOSING 2
#define MOTOR_STATE_CLOSED 3
#define MOTOR_STATE_RUNSPEED 4
#define MOTOR_STATE_RUN 5
#define MOTOR_STATE_RUN_HOME 6
#define MOTOR_STATE_STOP 7
#define MOTOR_STATE_STOP_NOW 8
#define MOTOR_STATE_STOP_BY_CLOSE_LIMIT 9
#define MOTOR_STATE_OPENING 10
#define MOTOR_STATE_OPEN 11

#endif
