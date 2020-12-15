#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <thread>



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

    double fLatitude;
    double fLongitude;
    float fAltitude;

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
        telemetry->subscribe_position([&fLatitude, &fLongitude, &fAltitude](Telemetry::Position position) {
            fLatitude = position.latitude_deg;
            fLongitude = position.longitude_deg;
            fAltitude = position.relative_altitude_m;

            cout << "위도 : " << fLatitude << endl;
            cout << "경도 : " << fLongitude << endl;
            cout << "상대 고도 : " << fAltitude << endl;
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

int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    shared_ptr<System> system;
    shared_ptr<Telemetry> telemetry;

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

    // Telemetry, Action 객체 얻기
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
        GetPosition(telemetry, &fLatitude, &fLongitude, &fAltitude);
        cout << fLatitude << endl;
        cout << fLongitude << endl;
        cout << fAltitude << endl;
    }

    return 0;
}
