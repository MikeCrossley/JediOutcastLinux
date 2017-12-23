/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef HMDDEVICEOPENVRSDK_H
#define HMDDEVICEOPENVRSDK_H

#include "../HmdDevice/IHmdDevice.h"

#include <openvr.h>
#include <openvr_capi.h>

namespace OpenVRSDK
{
class HmdDeviceOpenVRSdk : public IHmdDevice
{
public:

	HmdDeviceOpenVRSdk();
    virtual ~HmdDeviceOpenVRSdk();

    virtual bool Init(bool allowDummyDevice = false);
    virtual void Shutdown();

    virtual std::string GetInfo();

    virtual bool HasDisplay();
    virtual bool HandlesControllerInput() { return true; }
    virtual std::string GetDisplayDeviceName();
    virtual bool GetDisplayPos(int& rX, int& rY);

    virtual bool GetDeviceResolution(int& rWidth, int& rHeight, bool& rIsRotated, bool& rIsExtendedMode);
    virtual bool GetOrientationRad(float& rPitch, float& rYaw, float& rRoll);
    virtual bool GetPosition(float& rX, float& rY, float& rZ);
    virtual bool GetHandOrientationRad(bool rightHand, float& rPitch, float& rYaw, float& rRoll);
    virtual bool GetHandPosition(bool rightHand, float& rX, float& rY, float& rZ);
    virtual bool HasHand(bool rightHand);
    virtual void Recenter();

    bool IsDebugHmd() { return mUsingDebugHmd; }

	vr::IVRSystem* GetHMDSystem() { return m_pVRSystem;	};

private:
    // disable copy constructor
	HmdDeviceOpenVRSdk(const HmdDeviceOpenVRSdk&);
	HmdDeviceOpenVRSdk& operator=(const HmdDeviceOpenVRSdk&);

    void ConvertQuatToEuler(const float* quat, float& rYaw, float& rPitch, float& rRoll);
    int GetCpuCount();
    
    bool mIsInitialized;
    bool mUsingDebugHmd;
    bool mPositionTrackingEnabled;
    bool mIsRotated;


    std::string mInfo;

	vr::IVRSystem* m_pVRSystem;
};
}
#endif
