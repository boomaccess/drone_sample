#include "common.h"



/*
 *
 */
void GetComponent(mavsdk::ComponentType component_type)
{
    std::string str[] = { "UNKNOWN", "AUTOPILOT", "CAMERA", "GIMBAL" };
    std::cout << "검색된 component : " << str[unsigned(component_type)] << std::endl;
}


/*
 *
 */
int InitSystem(std::string strUrl, mavsdk::Mavsdk *pMavsdk, std::shared_ptr<mavsdk::System> *pSystem)
{
    mavsdk::ConnectionResult connectionResult;
    bool bFindSystem = false;
    
    int nRet = 1;
    


    // URL을 이용해 드론에 연결하기
    if( nRet )
    {
        connectionResult = pMavsdk->add_any_connection(strUrl);
        if( connectionResult == mavsdk::ConnectionResult::Success )
        {
            std::cout << "연결 성공 : " << strUrl << std::endl;
        }
        else
        {
            std::cout << "연결 실패 : " << strUrl << std::endl;
            nRet = 0;
        }
    }

    // System 연결 여부 검사하기
    if( nRet )
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        pMavsdk->subscribe_on_new_system([pMavsdk, &bFindSystem]() {
            const auto systemObj = pMavsdk->systems().at(0);

            if( systemObj->is_connected() )
            {
                bFindSystem = true;
            }
        });

        if( bFindSystem != true )
        {
            std::cout << "System 찾기 실패" << std::endl;
            nRet = 0;
        }
    }

    // System 객체 얻기
    if( nRet )
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        *pSystem = pMavsdk->systems().at(0);
        (*pSystem)->register_component_discovered_callback(GetComponent);
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return nRet;
}



/*
 *
 */
int InitTelemetry(std::shared_ptr<mavsdk::System> system, std::shared_ptr<mavsdk::Telemetry> telemetry)
{
    mavsdk::Telemetry::Result resultTelemetry;

    bool bTelemetryStatus = false;

    int nRet = 1;



    // 위치 설정 rate 설정하기
    if( nRet )
    {
        resultTelemetry = telemetry->set_rate_position(2.0);
        if( resultTelemetry == mavsdk::Telemetry::Result::Success )
        {
            std::cout << "위치 설정 rate 성공 : " << resultTelemetry << std::endl;
        }
        else
        {
            std::cout << "위치 설정 rate 실패 : " << resultTelemetry << std::endl;
            nRet = 0;
        }
    }

    // 위도, 경도, 상대 고도 얻기
    if( nRet )
    {
        telemetry->subscribe_position([](mavsdk::Telemetry::Position position) {
            std::cout << "위도 : " << position.latitude_deg << std::endl;
            std::cout << "경도 : " << position.longitude_deg << std::endl;
            std::cout << "상대 고도 : " << position.relative_altitude_m << std::endl;
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if( nRet )
    {
        while (bTelemetryStatus != true)
        {
            bTelemetryStatus = telemetry->health_all_ok();
            std::cout << "Vehicle is getting ready to arm" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    return nRet;
}


/*
 *
 */
int GetGPSPosition(std::shared_ptr<mavsdk::Telemetry> telemetry, double *pfLatitude, double *pfLongitude, float *pfAltitude)
{
    mavsdk::Telemetry::Position position;

    int nRet = 1;



    if( telemetry == NULL )
    {
        nRet = 0;
    }

    if( nRet )
    {
        position = telemetry->position();

        *pfLatitude = position.latitude_deg;
        *pfLongitude = position.longitude_deg;
        *pfAltitude = position.relative_altitude_m;
    }

    return nRet;
}


/*
 *
 */
int ActionArm(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action)
{
    mavsdk::Action::Result resultAction;
    int nRet = 1;



    if( nRet )
    {
        resultAction = action->arm();

        if (resultAction == mavsdk::Action::Result::Success)
        {
            std::cout << "Arm 성공 :" << resultAction << std::endl;
        }
        else
        {
            std::cout << "Arm 실패 :" << resultAction << std::endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int ActionTakeoff(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action)
{
    int nRet = 1;
    mavsdk::Action::Result resultAction;


    if( nRet )
    {
        resultAction = action->takeoff();

        if (resultAction == mavsdk::Action::Result::Success)
        {
            std::cout << "Takeoff 성공 :" << resultAction << std::endl;
        }
        else
        {
            std::cout << "Takeoff 실패 :" << resultAction << std::endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int ActionLand(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action)
{
    mavsdk::Action::Result resultAction;

    int nRet = 1;


    // 착륙하기
    if( nRet )
    {
        resultAction = action->land();

        if (resultAction == mavsdk::Action::Result::Success)
        {
            std::cout << "Land 성공 :" << resultAction << std::endl;
        }
        else
        {
            std::cout << "Land 실패 :" << resultAction << std::endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 드론이 공중에 있는지 검사하기
        while (telemetry->in_air())
        {
            std::cout << "내려가는 중..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << "착륙 완료" << std::endl;
    }

    // disarm 기다리기
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    return nRet;
}



/*
 *
 */
int ActionRtl(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action)
{
    mavsdk::Action::Result resultAction;

    int nRet = 1;


    // 착륙하기
    if( nRet )
    {
        resultAction = action->return_to_launch();

        if (resultAction == mavsdk::Action::Result::Success)
        {
            std::cout << "Return To Launch 성공 :" << resultAction << std::endl;
        }
        else
        {
            std::cout << "Return To Launch 실패 :" << resultAction << std::endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 드론이 공중에 있는지 검사하기
        while (telemetry->in_air())
        {
            std::cout << "내려가는 중..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << "착륙 완료" << std::endl;
    }

    // disarm 기다리기
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    return nRet;
}



/*
 *
 */
int MissionTestPoint(std::vector<mavsdk::Mission::MissionItem> *pVector, float fAltitude, float fSpeed)
{
    mavsdk::Mission::MissionItem mission_item;

    int nRet = 1;



    // 기본값 넣기
    mission_item.relative_altitude_m = fAltitude;
    mission_item.speed_m_s = fSpeed;

    // waypoint 추가하기
    if( nRet )
    {
        // range: -90 to +90
        mission_item.latitude_deg = 47.398170327054473;
        // range: -180 to +180
        mission_item.longitude_deg = 8.5456490218639658;

#if 0
        // takeoff altitude
        mission_item.relative_altitude_m = 10.0f;          
        mission_item.speed_m_s = 20.0f;
        // stop on the waypoint
        mission_item.is_fly_through = false;                
        mission_item.gimbal_pitch_deg = 20.0f;
        mission_item.gimbal_yaw_deg = 60.0f;
        mission_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
#endif
        pVector->push_back(mission_item);

        mission_item.latitude_deg = 47.398139363821485;
        mission_item.longitude_deg = 8.5453846156597137;

#if 0
        mission_item.relative_altitude_m = 10.0f;
        mission_item.speed_m_s = 20.0f;

        mission_item.is_fly_through = true;
        mission_item.gimbal_pitch_deg = -45.0f;
        mission_item.gimbal_yaw_deg = 0.0f;
        mission_item.camera_action = Mission::MissionItem::CameraAction::StartVideo;
#endif
        pVector->push_back(mission_item);

    }

    return nRet;
}



int MissionAddPoint(std::vector<mavsdk::Mission::MissionItem> *pVector, double fLatitude, double fLongitude, float fAltitude, float fSpeed)
{
    mavsdk::Mission::MissionItem mission_item;

    int nRet = 1;



    if( nRet )
    {
        // range: -90 to +90
        mission_item.latitude_deg = fLatitude;
        // range: -180 to +180
        mission_item.longitude_deg = fLongitude;


        // takeoff altitude
        mission_item.relative_altitude_m = fAltitude;          
        mission_item.speed_m_s = fSpeed;
        // stop on the waypoint
#if 0
        mission_item.is_fly_through = false;                
        mission_item.gimbal_pitch_deg = 20.0f;
        mission_item.gimbal_yaw_deg = 60.0f;
        mission_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
#endif
        pVector->push_back(mission_item);
    }

    return nRet;
}

/*
 *
 */
int MissionMakePointVector(std::vector<mavsdk::Mission::MissionItem> *pVector, std::vector<POINTTYPE> *pInputVector, float fAltitude, float fSpeed)
{
    mavsdk::Mission::MissionItem mission_item;

    std::vector<POINTTYPE>::iterator i;

    int nRet = 1;



    // waypoint 추가하기
    if( nRet )
    {
        for (i = pInputVector->begin(); i != pInputVector->end(); i = i + 1)
        {
            // range: -90 to +90
            mission_item.latitude_deg = i->x;
            // range: -180 to +180
            mission_item.longitude_deg = i->y;
            // takeoff altitude
            mission_item.relative_altitude_m = fAltitude;          
            mission_item.speed_m_s = fSpeed;
            // stop on the waypoint
            mission_item.is_fly_through = false;                
    #if 0
            mission_item.gimbal_pitch_deg = 20.0f;
            mission_item.gimbal_yaw_deg = 60.0f;
            mission_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    #endif
            pVector->push_back(mission_item);
        }
    }

    return nRet;
}



/*
 *
 */
int MissionMakePointRectangle(std::vector<POINTTYPE> *pInputVector, double fLatitude1, double fLongitude1, double fLatitude2, double fLongitude2, double fDistance)
{
    POINTTYPE pos1;
    POINTTYPE pos2;

    POINTTYPE pos3;
    POINTTYPE pos4;

    int nRet = 1;



    if( nRet )
    {
        pos1.x = fLatitude1;
        pos1.y = fLongitude1;

        pos2.x = fLatitude2;
        pos2.y = fLongitude2;

        nRet = PositionFootPerpendicular(pos1, pos2, fDistance, &pos3, &pos4);
        pInputVector->push_back(pos3);
        pInputVector->push_back(pos4);
        
        nRet = PositionFootPerpendicular(pos2, pos1, fDistance, &pos3, &pos4);
        pInputVector->push_back(pos3);
        pInputVector->push_back(pos4);
    }

    return nRet;
}


/*
 *
 */
int MissionMakePointCircle(std::vector<POINTTYPE> *pInputVector, ENUMDRECTIONTYPE direction, double fLatitude1, double fLongitude1, double fLatitude2, double fLongitude2, int nRotateAngle, double fDistance)
{
    POINTTYPE centerPoint;
    POINTTYPE pos;
    double angle = 0.0f;
    int nAngle = 0;
    int i = 0;

    int nRet = 1;



    if( nRet )
    {
        centerPoint.x = (fLatitude2 - fLatitude1) / 2.0f + fLatitude1;
        centerPoint.y = (fLongitude2 - fLongitude1) / 2.0f + fLongitude1;

        angle = atan2((fLatitude2 - fLatitude1), (fLongitude2 - fLongitude1));
        nAngle = angle * 180.0 / M_PI;

        if( direction == DIRECTION_CCW )
        {
            nAngle = nAngle + 180;
        }

        for (i = nAngle; i <= nAngle + nRotateAngle; i = i + 10)
        {
            pos.x = centerPoint.x + sin(i * M_PI / 180.0f) * fDistance;
            pos.y = centerPoint.y + cos(i * M_PI / 180.0f) * fDistance;

            pInputVector->push_back(pos);
        }        
    }

    return nRet;
}



int MissionUpload(std::shared_ptr<mavsdk::Mission> mission, std::vector<mavsdk::Mission::MissionItem> *pVector)
{
    std::shared_ptr<std::promise<mavsdk::Mission::Result>> prom;
    mavsdk::Mission::MissionPlan missionPlan;
    mavsdk::Mission::Result resultMission;
    std::future<mavsdk::Mission::Result> future_result;
    
    int nRet = 1;



    if( nRet )
    {
        std::cout << "미션 업로드 중..." << std::endl;
        
        prom = std::make_shared<std::promise<mavsdk::Mission::Result>>();
        future_result = prom->get_future();
        
        missionPlan.mission_items = *pVector;
        mission->upload_mission_async(missionPlan, [prom](mavsdk::Mission::Result result) { prom->set_value(result); });

        resultMission = future_result.get();
        if (resultMission == mavsdk::Mission::Result::Success)
        {
            std::cout << "미션 업로드 완료 : " << resultMission << std::endl;
        }
        else
        {
            std::cout << "미션 업로드 실패 : " << resultMission << std::endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int MissionRun(std::shared_ptr<mavsdk::Mission> mission)
{
    std::shared_ptr<std::promise<mavsdk::Mission::Result>> prom;
    std::future<mavsdk::Mission::Result> future_result;
    mavsdk::Mission::Result result;

    int nRet = 1;



    if( nRet )
    {
        std::cout << "미션 준비" << std::endl;

        prom = std::make_shared<std::promise<mavsdk::Mission::Result>>();
        future_result = prom->get_future();
        mission->start_mission_async([prom](mavsdk::Mission::Result result) {
            prom->set_value(result);
            std::cout << "미션 시작" << std::endl;
        });

        result = future_result.get();
        if (result != mavsdk::Mission::Result::Success)
        {
            std::cout << "미션 실패 : " << result << std::endl;
            nRet = 0;
        }

        while (!mission->is_mission_finished().second)
        {
            std::cout << "미션 수행 중" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    return nRet;
}



/*
 *
 */
int OffboardControl_SetAttitude(std::shared_ptr<mavsdk::Offboard> offboard, float fDegree, float fThrust, int nSecond)
{
    mavsdk::Offboard::Attitude control_stick {};
    mavsdk::Offboard::Result resultOffboard;

    int nRet = 1;

    

    if( nRet )
    {
        // 조종기 스틱 초기화하기
        offboard->set_attitude(control_stick);

        // Offboard 시작하기
        resultOffboard = offboard->start();
        if( resultOffboard == mavsdk::Offboard::Result::Success )
        {
            std::cout << "Offboard 성공 : " << resultOffboard << std::endl;
        }
        else
        {
            std::cout << "Offboard 실패 : " << resultOffboard << std::endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 스틱 값 설정하기
        control_stick.roll_deg = fDegree;
        control_stick.thrust_value = fThrust;

        // 스틱 적용하기
        offboard->set_attitude(control_stick);

        // 스틱을 적용할 시간 설정하기
        std::this_thread::sleep_for(std::chrono::seconds(nSecond));

        // 정지하기
        offboard->stop();
    }

    return nRet;
}



/*
 *
 */
int OffboardControl_SetVelocityBody(std::shared_ptr<mavsdk::Offboard> offboard, float fDownSpeed, float fYawDegree, int nSecond)
{
    mavsdk::Offboard::VelocityBodyYawspeed control_stick{};
    mavsdk::Offboard::Result resultOffboard;

    int nRet = 1;

    

    if( nRet )
    {
        // 조종기 스틱 초기화하기
        offboard->set_velocity_body(control_stick);

        // Offboard 시작하기
        resultOffboard = offboard->start();
        if( resultOffboard == mavsdk::Offboard::Result::Success )
        {
            std::cout << "Offboard 성공 : " << resultOffboard << std::endl;
        }
        else
        {
            std::cout << "Offboard 실패 : " << resultOffboard << std::endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 스틱 값 설정하기
        control_stick.down_m_s = fDownSpeed;
        control_stick.yawspeed_deg_s = fYawDegree;

        // 스틱 적용하기
        offboard->set_velocity_body(control_stick);

        // 스틱을 적용할 시간 설정하기
        std::this_thread::sleep_for(std::chrono::seconds(nSecond));

        // 정지하기
        offboard->stop();
    }

    return nRet;
}
