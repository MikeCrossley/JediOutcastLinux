/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRDEVICE_H
#define OPENVRDEVICE_H

#include "../HmdDevice/IHmdDevice.h"
#include "OpenVRTracking.h"

#include <openvr.h>
#include <openvr_capi.h>

namespace OpenVR
{
class COpenVRDevice : public IHmdDevice
{
public:

	COpenVRDevice();
	virtual ~COpenVRDevice() {};

    virtual bool Init(bool allowDummyDevice = false);
    virtual void Shutdown();

	virtual std::string GetInfo() { return "OpenVRDevice"; }

	virtual bool HasDisplay() { return m_bIsInitialized; }
    virtual bool HandlesControllerInput() { return true; }
	virtual std::string GetDisplayDeviceName() { return ""; }
	virtual bool GetDisplayPos(int& rX, int& rY) { return false; }

    virtual bool GetDeviceResolution(int& rWidth, int& rHeight, bool& rIsRotated, bool& rIsExtendedMode);
    virtual bool GetOrientationRad(float& rPitch, float& rYaw, float& rRoll);
    virtual bool GetPosition(float& rX, float& rY, float& rZ);
    virtual bool GetHandOrientationRad(bool rightHand, float& rPitch, float& rYaw, float& rRoll);
    virtual bool GetHandPosition(bool rightHand, float& rX, float& rY, float& rZ);
    virtual bool HasHand(bool rightHand);
    virtual void Recenter();

	vr::IVRSystem* GetHMDSystem() { return m_pVRSystem;	}
	COpenVRTracking* GetTrackedDevices() { return m_pTrackedDevices; }

private:
    // disable copy constructor
	COpenVRDevice(const COpenVRDevice&);
	COpenVRDevice& operator=(const COpenVRDevice&);

    bool m_bIsInitialized;
    bool m_bPositionTrackingEnabled;

	COpenVRTracking* m_pTrackedDevices;

	vr::IVRSystem* m_pVRSystem;
};
}
#endif
