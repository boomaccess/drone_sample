#include "common.h"



/*
 *
 */
int main(int argc, char** argv)
{
    mavsdk::Mavsdk mavsdk;
    std::shared_ptr<mavsdk::System> system;
    std::shared_ptr<mavsdk::Telemetry> telemetry;
    std::shared_ptr<mavsdk::Action> action;
    std::shared_ptr<mavsdk::Mission> mission;

    std::vector<mavsdk::Mission::MissionItem> misionVector;
    std::vector<POINTTYPE> inputVector;

    double fLatitude = 0;
    double fLongitude = 0;
    float fAltitude = 0;
    float fSpeed = 0;

    std::string strUrl;

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
        telemetry = std::make_shared<mavsdk::Telemetry>(system);
    }

    // Telemetry 초기화하기
    if( nRet )
    {
        nRet = InitTelemetry(system, telemetry);
    }

    // 현재 위치 정보 출력하기
    if( nRet )
    {
        nRet = GetGPSPosition(telemetry, &fLatitude, &fLongitude, &fAltitude);
        std::cout << fLatitude << std::endl;
        std::cout << fLongitude << std::endl;
        std::cout << fAltitude << std::endl;
    }

    // Action 객체 얻기
    if( nRet )
    {
        action = std::make_shared<mavsdk::Action>(system);
    }

#if 0
    // Arm 하기
    if( nRet )
    {
        nRet = ActionArm(telemetry, action);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // 이륙하기
    if( nRet )
    {
        nRet = ActionTakeoff(telemetry, action);
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }

    // 착륙하기
    if( nRet )
    {
        nRet = ActionLand(telemetry, action);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
#endif

    // Mission 객체 초기화하기
    if( nRet )
    {
        mission = std::make_shared<mavsdk::Mission>(system);
    }

    // Mission WayPoint 생성하기
    if( nRet )
    {
        fLatitude = 47.3979634;
        fLongitude = 8.5455553;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        fLatitude = 47.3980652;
        fLongitude = 8.5452689;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

#if 1
        MissionMakePointCircle(&inputVector, DIRECTION_CCW, 47.3978793, 8.5451496, 47.3977277, 8.5450557, 180, 0.0001);
        MissionMakePointVector(&misionVector, &inputVector, 5.0f, 20.0f);
#else
        // 사각형
        fLatitude = 47.3978793;
        fLongitude = 8.5451496;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        inputVector.clear();
        MissionMakePointRectangle(&inputVector, 47.3978793, 8.5451496, 47.3977277,8.5450557, 0.0001);
        MissionAddPoint(&misionVector, inputVector[0].x, inputVector[0].y, 5.0f, 20.0f);
        MissionAddPoint(&misionVector, inputVector[3].x, inputVector[3].y, 5.0f, 20.0f);

        fLatitude = 47.3977277;
        fLongitude = 8.5450557;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);
#endif

        fLatitude = 47.3974635;
        fLongitude = 8.5449296;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        fLatitude = 47.3972738;
        fLongitude = 8.5451496;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        // 사각형 1
        fLatitude = 47.3973392;
        fLongitude = 8.5452166;
        // MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        // 사각형 2
        fLatitude = 47.3975197;
        fLongitude = 8.5454544;
        // MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        inputVector.clear();
        MissionMakePointCircle(&inputVector, DIRECTION_CCW, 47.3973392, 8.5452166, 47.3975197, 8.5454544, 180, 0.0001);
        MissionMakePointVector(&misionVector, &inputVector, 5.0f, 20.0f);

        fLatitude = 47.3975643;
        fLongitude = 8.5455144;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);

        fLatitude = 47.3977059;
        fLongitude = 8.5455251;
        MissionAddPoint(&misionVector, fLatitude, fLongitude, 5.0f, 20.0f);
    }

    // Mission Upload 하기
    if( nRet )
    {
        nRet = MissionUpload(mission, &misionVector);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // 시동걸기
    if( nRet )
    {
        ActionArm(telemetry, action);
    }

    // Mission 실행하기
    if( nRet )
    {
        nRet = MissionRun(mission);
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 착륙
    if( nRet )
    {
        nRet = ActionLand(telemetry, action);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    return 0;
}
