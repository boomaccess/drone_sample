#ifndef _DRONE_CONTROL_H
#define _DRONE_CONTROL_H



void GetComponent(mavsdk::ComponentType component_type);
int InitSystem(std::string strUrl, mavsdk::Mavsdk *pMavsdk, std::shared_ptr<mavsdk::System> *pSystem);
int InitTelemetry(std::shared_ptr<mavsdk::System> system, std::shared_ptr<mavsdk::Telemetry> telemetry);

int GetGPSPosition(std::shared_ptr<mavsdk::Telemetry> telemetry, double *pfLatitude, double *pfLongitude, float *pfAltitude);

int ActionArm(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action);
int ActionTakeoff(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action);
int ActionLand(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action);
int ActionRtl(std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<mavsdk::Action> action);

int MissionTestPoint(std::vector<mavsdk::Mission::MissionItem> *pVector, float fAltitude, float fSpeed);
int MissionAddPoint(std::vector<mavsdk::Mission::MissionItem> *pVector, double fLatitude, double fLongitude, float fAltitude, float fSpeed);
int MissionMakePointVector(std::vector<mavsdk::Mission::MissionItem> *pVector, std::vector<POINTTYPE> *pInputVector, float fAltitude, float fSpeed);
int MissionMakePointRectangle(std::vector<POINTTYPE> *pInputVector, double fLatitude1, double fLongitude1, double fLatitude2, double fLongitude2, double fDistance);
int MissionMakePointCircle(std::vector<POINTTYPE> *pInputVector, ENUMDRECTIONTYPE direction, double fLatitude1, double fLongitude1, double fLatitude2, double fLongitude2, int nRotateAngle, double fDistance);
int MissionUpload(std::shared_ptr<mavsdk::Mission> mission, std::vector<mavsdk::Mission::MissionItem> *pVector);
int MissionRun(std::shared_ptr<mavsdk::Mission> mission);

int OffboardControl_SetAttitude(std::shared_ptr<mavsdk::Offboard> offboard, float fDegree, float fThrust, int nSecond);
int OffboardControl_SetVelocityBody(std::shared_ptr<mavsdk::Offboard> offboard, float fDownSpeed, float fYawDegree, int nSecond);

#endif
