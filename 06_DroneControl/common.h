#ifndef _COMMON_H
#define _COMMON_H



#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <iostream>
#include <thread>
#include <future>
#include <cmath>



// 데이터형 선언하기
typedef struct tagPosition
{
    double x;
    double y;
} POINTTYPE;

//
typedef enum {
    DIRECTION_CW = 0,
    DIRECTION_CCW
} ENUMDRECTIONTYPE;



#include "Position.h"
#include "DroneControl.h"



#endif
