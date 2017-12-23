#include "HmdDeviceOpenVRSdk.h"
#include "../SearchForDisplay.h"

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

#include "../../client/client.h"

using namespace std;
using namespace OpenVRSDK;

HmdDeviceOpenVRSdk::HmdDeviceOpenVRSdk()
    : mIsInitialized(false)
    , mUsingDebugHmd(false)
    , mPositionTrackingEnabled(false)
    , mIsRotated(false)
    //, mpHmd(NULL)
{

}

HmdDeviceOpenVRSdk::~HmdDeviceOpenVRSdk()
{

}

bool HmdDeviceOpenVRSdk::Init(bool allowDummyDevice)
{
    if (mIsInitialized)
    {
        return true;
    }

    bool debugPrint = true;

    if (debugPrint)
    {
		VID_Printf(PRINT_ALL, "[HMD][OpenVR] Intializing OpenVR ...\n");
    }

	vr::HmdError* pError = nullptr;

	vr::EVRInitError eError = vr::EVRInitError::VRInitError_None;
	m_pVRSystem = vr::VR_Init(&eError, vr::EVRApplicationType::VRApplication_Scene);

	if (eError != vr::EVRInitError::VRInitError_None)
	{
		m_pVRSystem = nullptr;
		VID_Printf(PRINT_ALL, "[HMD][OpenVR] Unable to initialize VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}
	

    mInfo = "HmdDeviceOpenVRSdk:";

    mIsInitialized = true;

	VID_Printf(PRINT_ALL, "[HMD][OpenVR] VR Initialized");

    return true;
}

void HmdDeviceOpenVRSdk::Shutdown()
{
    if (!mIsInitialized)
    {
        return;
    }

	vr::VR_Shutdown();
	VID_Printf(PRINT_ALL, "[HMD][OpenVR] Shutdown");

    mIsInitialized = false;
}

std::string HmdDeviceOpenVRSdk::GetInfo()
{
    return mInfo;
}

bool HmdDeviceOpenVRSdk::HasDisplay()
{
    if (!mIsInitialized)
    {
        return false;
    }

    return true;
}

std::string HmdDeviceOpenVRSdk::GetDisplayDeviceName()
{
    return "";
}

bool HmdDeviceOpenVRSdk::GetDisplayPos(int& rX, int& rY)
{
    return false;
}

bool HmdDeviceOpenVRSdk::GetDeviceResolution(int& rWidth, int& rHeight, bool& rIsRotated, bool& rIsExtendedMode)
{
	if (!mIsInitialized)
    {
        return false;
    }

	uint32_t width = 0;
	uint32_t height = 0;

	m_pVRSystem->GetRecommendedRenderTargetSize(&width, &height);

	//rWidth = width;
	//rHeight = height;

	VID_Printf(PRINT_ALL, "%d %d", width, height);

	rWidth = 1280;
	rHeight = 720;

	//rIsRotated = false;
	//rIsExtendedMode = false;

    return true;
}

bool HmdDeviceOpenVRSdk::GetOrientationRad(float& rPitch, float& rYaw, float& rRoll)
{
	/*if (!mIsInitialized || mpHmd == nullptr)
    {
        return false;
    }*/

    // Query the HMD for the sensor state at a given time.
  /*  ovrTrackingState ts  = d_ovr_GetTrackingState(mpHmd, d_ovr_GetTimeInSeconds(), false);
    if ((ts.StatusFlags & ovrStatus_OrientationTracked))
    {
        ovrQuatf orientation = ts.HeadPose.ThePose.Orientation;
        
        float quat[4];
        quat[0] = orientation.x;
        quat[1] = orientation.y;
        quat[2] = orientation.z;
        quat[3] = orientation.w;
        
        ConvertQuatToEuler(&quat[0], rYaw, rPitch, rRoll);
        
        //printf("pitch: %.2f yaw: %.2f roll: %.2f\n", rPitch, rYaw, rRoll);

        return true;
    }*/

    return false;

}

bool HmdDeviceOpenVRSdk::GetPosition(float &rX, float &rY, float &rZ)
{
    /*if (!mIsInitialized || mpHmd == nullptr || !mPositionTrackingEnabled)
    {
        return false;
    }*/

    // Query the HMD for the sensor state at a given time.
    /*ovrTrackingState ts  = d_ovr_GetTrackingState(mpHmd, d_ovr_GetTimeInSeconds(), false);
    if ((ts.StatusFlags & ovrStatus_PositionTracked))
    {
        ovrVector3f pos = ts.HeadPose.ThePose.Position;
        rX = pos.x;
        rY = pos.y;
        rZ = pos.z;

        //printf("pitch: %.2f yaw: %.2f roll: %.2f\n", rPitch, rYaw, rRoll);

        return true;
    }*/

    return false;
}

bool HmdDeviceOpenVRSdk::GetHandOrientationRad(bool rightHand, float& rPitch, float& rYaw, float& rRoll)
{
    /*if (!mIsInitialized || mpHmd == nullptr)
    {
        return false;
    }*/

    // Query for the current tracking state and see if hands are available
   /* ovrTrackingState ts = d_ovr_GetTrackingState(mpHmd, d_ovr_GetTimeInSeconds(), false);
    if ((ts.HandStatusFlags[rightHand ? ovrHand_Right : ovrHand_Left] & ovrStatus_OrientationTracked))
    {
        ovrQuatf orientation = ts.HandPoses[rightHand ? ovrHand_Right : ovrHand_Left].ThePose.Orientation;

        float quat[4];
        quat[0] = orientation.x;
        quat[1] = orientation.y;
        quat[2] = orientation.z;
        quat[3] = orientation.w;

        ConvertQuatToEuler(&quat[0], rYaw, rPitch, rRoll);

        return true;
    }*/

    return false;

}

bool HmdDeviceOpenVRSdk::GetHandPosition(bool rightHand, float &rX, float &rY, float &rZ)
{
   /* if (!mIsInitialized || mpHmd == nullptr || !mPositionTrackingEnabled)
    {
        return false;
    }

    // Query for the current tracking state and see if hands are available
    ovrTrackingState ts = d_ovr_GetTrackingState(mpHmd, d_ovr_GetTimeInSeconds(), false);
    if ((ts.HandStatusFlags[rightHand ? ovrHand_Right : ovrHand_Left] & ovrStatus_PositionTracked))
    {
        ovrVector3f pos = ts.HandPoses[rightHand ? ovrHand_Right : ovrHand_Left].ThePose.Position;
        rX = pos.x;
        rY = pos.y;
        rZ = pos.z;

        return true;
    }*/

    return false;
}

bool HmdDeviceOpenVRSdk::HasHand(bool rightHand)
{
    // Query for the current tracking state and see if hands are available
   // ovrTrackingState ts = d_ovr_GetTrackingState(mpHmd, d_ovr_GetTimeInSeconds(), false);

    //return (ts.HandStatusFlags[rightHand ? ovrHand_Right : ovrHand_Left] & (ovrStatus_PositionTracked | ovrStatus_OrientationTracked));

	return false;
}

void HmdDeviceOpenVRSdk::Recenter()
{
	m_pVRSystem->ResetSeatedZeroPose();
}

void HmdDeviceOpenVRSdk::ConvertQuatToEuler(const float* quat, float& rYaw, float& rPitch, float& rRoll)
{
    //https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
    // modified to get yaw before pitch

    float W = quat[3];
    float X = quat[1];
    float Y = quat[0];
    float Z = quat[2];

    float sqw = W*W;
    float sqx = X*X;
    float sqy = Y*Y;
    float sqz = Z*Z;

    float test = 2.0f * (Y*W - X*Z);

    if (test > (1.0f - 0.000001f))
    {
        // heading = rotation about z-axis
        rRoll = (-2.0f*atan2(X, W));
        // bank = rotation about x-axis
        rYaw = 0;
        // attitude = rotation about y-axis
        rPitch = M_PI/2.0f;
    }
    else if (test < (-1.0f + 0.000001f))
    {
        // heading = rotation about z-axis
        rRoll = (2.0f*atan2(X, W));
        // bank = rotation about x-axis
        rYaw = 0;
        // attitude = rotation about y-axis
        rPitch = M_PI/-2.0f;
    }
    else
    {
        // heading = rotation about z-axis
        rRoll = atan2(2.0f * (X*Y +Z*W),(sqx - sqy - sqz + sqw));
        // bank = rotation about x-axis
        rYaw = atan2(2.0f * (Y*Z +X*W),(-sqx - sqy + sqz + sqw));
        // attitude = rotation about y-axis
        test = max(test, -1.0f);
        test = min(test, 1.0f);
        rPitch = asin(test);
    }
}

int HmdDeviceOpenVRSdk::GetCpuCount()
{
#if defined(OVR_OS_WIN32)
        SYSTEM_INFO sysInfo;

        #if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0501)
            GetNativeSystemInfo(&sysInfo);
        #else
            GetSystemInfo(&sysInfo);
        #endif

        return (int) sysInfo.dwNumberOfProcessors;
#else
    return 1;
#endif
}



