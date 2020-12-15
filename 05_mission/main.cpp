#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <iostream>
#include <thread>
#include <future>



using namespace mavsdk;
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;



/*
 *
 */
void GetComponent(ComponentType component_type)
{
    string str[] = { "UNKNOWN", "AUTOPILOT", "CAMERA", "GIMBAL" };
    cout << "검색된 component : " << str[unsigned(component_type)] << endl;
}



/*
 *
 */
int InitSystem(string strUrl, Mavsdk *pMavsdk, shared_ptr<System> *pSystem)
{
    ConnectionResult connectionResult;
    bool bFindSystem = false;
    
    int nRet = 1;
    


    // URL을 이용해 드론에 연결하기
    if( nRet )
    {
        connectionResult = pMavsdk->add_any_connection(strUrl);
        if( connectionResult == ConnectionResult::Success )
        {
            cout << "연결 성공 : " << strUrl << endl;
        }
        else
        {
            cout << "연결 실패 : " << strUrl << endl;
            nRet = 0;
        }
    }

    // System 연결 여부 검사하기
    if( nRet )
    {
        sleep_for(seconds(2));

        pMavsdk->subscribe_on_new_system([pMavsdk, &bFindSystem]() {
            const auto systemObj = pMavsdk->systems().at(0);

            if( systemObj->is_connected() )
            {
                bFindSystem = true;
            }
        });

        if( bFindSystem != true )
        {
            cout << "System 찾기 실패" << endl;
            nRet = 0;
        }
    }

    // System 객체 얻기
    if( nRet )
    {
        sleep_for(seconds(2));

        *pSystem = pMavsdk->systems().at(0);
        (*pSystem)->register_component_discovered_callback(GetComponent);
        
        sleep_for(seconds(2));
    }

    return nRet;
}



/*
 *
 */
int InitTelemetry(shared_ptr<System> system, shared_ptr<Telemetry> telemetry)
{
    Telemetry::Result resultTelemetry;

    bool bTelemetryStatus = false;

    int nRet = 1;



    // 위치 설정 rate 설정하기
    if( nRet )
    {
        resultTelemetry = telemetry->set_rate_position(1.0);
        if( resultTelemetry == Telemetry::Result::Success )
        {
            cout << "위치 설정 rate 성공 : " << resultTelemetry << endl;
        }
        else
        {
            cout << "위치 설정 rate 실패 : " << resultTelemetry << endl;
            nRet = 0;
        }
    }

    // 위도, 경도, 상대 고도 얻기
    if( nRet )
    {
        telemetry->subscribe_position([](Telemetry::Position position) {
            cout << "위도 : " << position.latitude_deg << endl;
            cout << "경도 : " << position.longitude_deg << endl;
            cout << "상대 고도 : " << position.relative_altitude_m << endl;
        });
        sleep_for(seconds(1));
    }

    if( nRet )
    {
        while (bTelemetryStatus != true)
        {
            bTelemetryStatus = telemetry->health_all_ok();
            std::cout << "Vehicle is getting ready to arm" << std::endl;
            sleep_for(seconds(1));
        }
    }

    return nRet;
}



/*
 *
 */
int GetPosition(shared_ptr<Telemetry> telemetry, double *pfLatitude, double *pfLongitude, float *pfAltitude)
{
    Telemetry::Position position;

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
int ActionArm(shared_ptr<Telemetry> telemetry, shared_ptr<Action> action)
{
    Action::Result resultAction;
    int nRet = 1;



    if( nRet )
    {
        resultAction = action->arm();

        if (resultAction == Action::Result::Success)
        {
            cout << "Arm 성공 :" << resultAction << endl;
        }
        else
        {
            cout << "Arm 실패 :" << resultAction << endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int ActionTakeoff(shared_ptr<Telemetry> telemetry, shared_ptr<Action> action)
{
    int nRet = 1;
    Action::Result resultAction;


    if( nRet )
    {
        resultAction = action->takeoff();

        if (resultAction == Action::Result::Success)
        {
            cout << "Takeoff 성공 :" << resultAction << endl;
        }
        else
        {
            cout << "Takeoff 실패 :" << resultAction << endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int ActionLand(shared_ptr<Telemetry> telemetry, shared_ptr<Action> action)
{
    Action::Result resultAction;

    int nRet = 1;


    // 착륙하기
    if( nRet )
    {
        resultAction = action->land();

        if (resultAction == Action::Result::Success)
        {
            cout << "Land 성공 :" << resultAction << endl;
        }
        else
        {
            cout << "Land 실패 :" << resultAction << endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 드론이 공중에 있는지 검사하기
        while (telemetry->in_air())
        {
            cout << "내려가는 중..." << endl;
            sleep_for(seconds(1));
        }
        
        cout << "착륙 완료" << endl;
    }

    // disarm 기다리기
    sleep_for(seconds(3));
    
    return nRet;
}



/*
 *
 */
int ActionRtl(shared_ptr<Telemetry> telemetry, shared_ptr<Action> action)
{
    Action::Result resultAction;

    int nRet = 1;


    // 착륙하기
    if( nRet )
    {
        resultAction = action->return_to_launch();

        if (resultAction == Action::Result::Success)
        {
            cout << "Return To Launch 성공 :" << resultAction << endl;
        }
        else
        {
            cout << "Return To Launch 실패 :" << resultAction << endl;
            nRet = 0;
        }
    }

    if( nRet )
    {
        // 드론이 공중에 있는지 검사하기
        while (telemetry->in_air())
        {
            cout << "내려가는 중..." << endl;
            sleep_for(seconds(1));
        }
        
        cout << "착륙 완료" << endl;
    }

    // disarm 기다리기
    sleep_for(seconds(3));
    
    return nRet;
}



/*
 *
 */
int MissionMakePoint(vector<Mission::MissionItem> *pVector, double fLatitude, double fLongitude, float fAltitude)
{
    Mission::MissionItem mission_item;

    int nRet = 1;



    // 기본값 넣기
    mission_item.latitude_deg = fLatitude;
    mission_item.longitude_deg = fLongitude;
    mission_item.relative_altitude_m = fAltitude;

    // waypoint 추가하기
    if( nRet )
    {
        // range: -90 to +90
        mission_item.latitude_deg = 47.398170327054473;
        // range: -180 to +180
        mission_item.longitude_deg = 8.5456490218639658;
        // takeoff altitude
        mission_item.relative_altitude_m = 10.0f;          
        mission_item.speed_m_s = 5.0f;
        // stop on the waypoint
        mission_item.is_fly_through = false;                
#if 0
        mission_item.gimbal_pitch_deg = 20.0f;
        mission_item.gimbal_yaw_deg = 60.0f;
        mission_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
#endif
        pVector->push_back(mission_item);

/*
        mission_item.latitude_deg = 47.398139363821485;
        mission_item.longitude_deg = 8.5453846156597137;
        mission_item.relative_altitude_m = 10.0f;
        mission_item.speed_m_s = 5.0f;
#if 0
        mission_item.is_fly_through = true;
        mission_item.gimbal_pitch_deg = -45.0f;
        mission_item.gimbal_yaw_deg = 0.0f;
        mission_item.camera_action = Mission::MissionItem::CameraAction::StartVideo;
#endif
        pVector->push_back(mission_item);
    }
*/
    }

    return nRet;
}



int MissionUpload(shared_ptr<Mission> mission, vector<Mission::MissionItem> *pVector)
{
    shared_ptr<promise<Mission::Result>> prom;
    Mission::MissionPlan missionPlan;
    Mission::Result resultMission;
    future<Mission::Result> future_result;
    
    int nRet = 1;



    if( nRet )
    {
        cout << "미션 업로드 중..." << endl;
        
        prom = std::make_shared<promise<Mission::Result>>();
        future_result = prom->get_future();
        
        missionPlan.mission_items = *pVector;
        mission->upload_mission_async(missionPlan, [prom](Mission::Result result) { prom->set_value(result); });

        resultMission = future_result.get();
        if (resultMission == Mission::Result::Success)
        {
            cout << "미션 업로드 완료 : " << resultMission << endl;
        }
        else
        {
            cout << "미션 업로드 실패 : " << resultMission << endl;
            nRet = 0;
        }
    }

    return nRet;
}



/*
 *
 */
int MissionRun(shared_ptr<Mission> mission)
{
    shared_ptr<std::promise<Mission::Result>> prom;
    future<Mission::Result> future_result;
    Mission::Result result;

    int nRet = 1;



    if( nRet )
    {
        cout << "미션 준비" << endl;

        prom = std::make_shared<std::promise<Mission::Result>>();
        future_result = prom->get_future();
        mission->start_mission_async([prom](Mission::Result result) {
            prom->set_value(result);
            cout << "미션 시작" << endl;
        });

        result = future_result.get();
        if (result != Mission::Result::Success)
        {
            cout << "미션 실패 : " << result << endl;
            nRet = 0;
        }

        while (!mission->is_mission_finished().second)
        {
            cout << "미션 수행 중" << endl;
            sleep_for(seconds(1));
        }
    }

    return nRet;
}


/*
 *
 */
int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    shared_ptr<System> system;
    shared_ptr<Telemetry> telemetry;
    shared_ptr<Action> action;
    shared_ptr<Mission> mission;

    vector<Mission::MissionItem> misionVector;

    double fLatitude = 0;
    double fLongitude = 0;
    float fAltitude = 0;

    string strUrl;

    int nRet = 0;


     // URL 설정하기
    if( argc == 1 )
    {
        strUrl = "udp://14540";
    }
    else
    {
        strUrl = argv[1];
    }    

    // System 객체 얻기
    nRet = InitSystem(strUrl, &mavsdk, &system);

    // Telemetry 객체 얻기
    if( nRet )
    {
        telemetry = make_shared<Telemetry>(system);
    }

    // Telemetry 초기화하기
    if( nRet )
    {
        nRet = InitTelemetry(system, telemetry);
    }

    // 현재 위치 정보 출력하기
    if( nRet )
    {
        nRet = GetPosition(telemetry, &fLatitude, &fLongitude, &fAltitude);
        cout << fLatitude << endl;
        cout << fLongitude << endl;
        cout << fAltitude << endl;
    }

    // Action 객체 얻기
    if( nRet )
    {
        action = make_shared<Action>(system);
    }

    // Arm 하기
    if( nRet )
    {
        nRet = ActionArm(telemetry, action);
        sleep_for(seconds(3));
    }

    // 이륙하기
    if( nRet )
    {
        nRet = ActionTakeoff(telemetry, action);
        sleep_for(seconds(20));
    }

    // 착륙하기
    if( nRet )
    {
        nRet = ActionLand(telemetry, action);
        sleep_for(seconds(3));
    }

    // Mission 객체 초기화하기
    if( nRet )
    {
        mission = std::make_shared<Mission>(system);
    }

    // Mission WayPoint 생성하기
    if( nRet )
    {
        nRet = MissionMakePoint(&misionVector, fLatitude, fLongitude, fAltitude + 1.0f);
    }

    // Mission Upload 하기
    if( nRet )
    {
        nRet = MissionUpload(mission, &misionVector);
        sleep_for(seconds(2));
    }

    if( nRet )
    {
        ActionArm(telemetry, action);
    }

    // Mission 실행하기
    if( nRet )
    {
        nRet = MissionRun(mission);
    }

    sleep_for(seconds(3));

    // 착륙하기
    if( nRet )
    {
        nRet = ActionRtl(telemetry, action);
        sleep_for(seconds(3));
    }
    
    return 0;
}
