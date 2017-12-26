#include "OpenVRDevice.h"

#include <cstring>
#include <string>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>

#ifdef _WINDOWS
	#define _USE_MATH_DEFINES
	#include <math.h>
	#include <windows.h>
#endif

#include "OpenVRHelper.h"

#include "../../client/client.h"

using namespace OpenVR;

COpenVRDevice::COpenVRDevice()
    : m_bIsInitialized(false)
    , m_bPositionTrackingEnabled(true)
	, m_pTrackedDevices(nullptr)
{

}

bool COpenVRDevice::Init(bool allowDummyDevice)
{
    if (m_bIsInitialized)
        return true;
 
	VID_Printf(PRINT_ALL, "[HMD][OpenVR] Intializing OpenVR ...\n");

	vr::HmdError* pError = nullptr;

	vr::EVRInitError eError = vr::EVRInitError::VRInitError_None;
	m_pVRSystem = vr::VR_Init(&eError, vr::EVRApplicationType::VRApplication_Scene);

	m_pTrackedDevices = new COpenVRTracking();
	m_pTrackedDevices->Init(this);

	if (eError != vr::EVRInitError::VRInitError_None)
	{
		m_pVRSystem = nullptr;
		VID_Printf(PRINT_ALL, "[HMD][OpenVR] Unable to initialize VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}

	m_bIsInitialized = true;

	VID_Printf(PRINT_ALL, "[HMD][OpenVR] VR Initialized");

    return true;
}

void COpenVRDevice::Shutdown()
{
    if (!m_bIsInitialized)
        return;

	vr::VR_Shutdown();
	VID_Printf(PRINT_ALL, "[HMD][OpenVR] Shutdown");

	m_pTrackedDevices->Shutdown();
	delete m_pTrackedDevices;

	m_bIsInitialized = false;
}

bool COpenVRDevice::GetDeviceResolution(int& rWidth, int& rHeight, bool& rIsRotated, bool& rIsExtendedMode)
{
	if (!m_bIsInitialized)
        return false;

	/*uint32_t width = 0;
	uint32_t height = 0;

	m_pVRSystem->GetRecommendedRenderTargetSize(&width, &height);*/

	rWidth = 1280;
	rHeight = 720;

    return true;
}

bool COpenVRDevice::GetOrientationRad(float& rPitch, float& rYaw, float& rRoll)
{
	if (!m_bIsInitialized)
        return false;

	vr::HmdMatrix34_t matrixHmd = m_pTrackedDevices->GetMatrix(vr::k_unTrackedDeviceIndex_Hmd);
	vr::HmdQuaternion_t orientation = GetRotation(matrixHmd);

	float quat[4];
	quat[0] = orientation.x;
	quat[1] = orientation.y;
	quat[2] = orientation.z;
	quat[3] = orientation.w;

	ConvertQuatToEuler(&quat[0], rYaw, rPitch, rRoll);

	return true;
}

bool COpenVRDevice::GetPosition(float &rX, float &rY, float &rZ)
{
    if (!m_bIsInitialized || !m_bPositionTrackingEnabled)
          return false;

	vr::HmdMatrix34_t matrixHmd = m_pTrackedDevices->GetMatrix(vr::k_unTrackedDeviceIndex_Hmd);

	rX = matrixHmd.m[0][3];
	rY = matrixHmd.m[1][3];
	rZ = matrixHmd.m[2][3];

	return true;
}

bool COpenVRDevice::GetHandOrientationRad(bool rightHand, float& rPitch, float& rYaw, float& rRoll)
{
    if (!m_bIsInitialized)
        return false;

	vr::HmdMatrix34_t matrixController = GetTrackedDevices()->GetControllerTransform(rightHand ? eController_OpenVR_2 : eController_OpenVR_1);
	vr::HmdQuaternion_t orientation = GetRotation(matrixController);

	float quat[4];
	quat[0] = orientation.x;
	quat[1] = orientation.y;
	quat[2] = orientation.z;
	quat[3] = orientation.w;

	ConvertQuatToEuler(&quat[0], rYaw, rPitch, rRoll);

    return true;
}

bool COpenVRDevice::GetHandPosition(bool rightHand, float &rX, float &rY, float &rZ)
{
    if (!m_bIsInitialized || !m_bPositionTrackingEnabled)
          return false;

	vr::HmdMatrix34_t matrixController = GetTrackedDevices()->GetControllerTransform(rightHand ? eController_OpenVR_2 : eController_OpenVR_1);

	rX = matrixController.m[0][3];
	rY = matrixController.m[1][3];
	rZ = matrixController.m[2][3];

    return true;
}

bool COpenVRDevice::HasHand(bool rightHand)
{
	return GetTrackedDevices()->IsConnected(rightHand ? eController_OpenVR_2 : eController_OpenVR_1);
}

void COpenVRDevice::Recenter()
{
	m_pVRSystem->ResetSeatedZeroPose();
}




