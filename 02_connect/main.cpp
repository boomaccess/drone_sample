#include <iostream>
#include <thread>
#include <chrono>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <future>
#include <string.h>


using namespace std;
using namespace mavsdk;
using namespace chrono;
using namespace std::this_thread;


/*
 *
 */
void GetComponent(ComponentType component_type)
{
    string str[] = { "UNKNOWN", "AUTOPILOT", "CAMERA", "GIMBAL" };
    cout << "검색된 component : " << str[int(component_type)] << endl;
}



/*
 *
 */
int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    shared_ptr<System> system;

    ConnectionResult connectionResult;
    bool bFindSystem = false;
    
    string strUrl;
    int nRet = 1;



    // URL 설정하기
    if( argc == 1 )
    {
        strUrl = "udp://14540";
    }
    else
    {
        strUrl = argv[1];
    }    

    // URL을 이용해 드론에 연결하기
    if( nRet )
    {
        connectionResult = mavsdk.add_any_connection(strUrl);
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

        mavsdk.subscribe_on_new_system([&mavsdk, &bFindSystem]() {
            const auto systemObj = mavsdk.systems().at(0);

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
        system = mavsdk.systems().at(0);

        system->register_component_discovered_callback(GetComponent);
    }

    return 0;
}
