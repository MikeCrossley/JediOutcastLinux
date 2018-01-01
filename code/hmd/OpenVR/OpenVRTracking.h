/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRTRACKING_H
#define OPENVRTRACKING_H

#include <openvr.h>
#include <openvr_capi.h>

namespace OpenVR
{
	class COpenVRDevice;

	enum EHmdController
	{
		eController_OpenVR_1 = 0,
		eController_OpenVR_2 = 1,
		eController_OpenVR_MaxNum = 2,
	};

class COpenVRTracking
{
public:
	COpenVRTracking() {};
	virtual ~COpenVRTracking() {};

	void Init(COpenVRDevice* pDevice);
	void Shutdown() {};

	void SetTrackingMatrix(int nDevice, vr::HmdMatrix34_t mMatrix) { m_rmat4DevicePose[nDevice] = mMatrix; }
	vr::HmdMatrix34_t GetMatrix(int nDevice) { return m_rmat4DevicePose[nDevice]; }

	void UpdateEvents();
	void UpdateController(int nDeviceId, vr::HmdMatrix34_t mMatrix, vr::VRControllerState_t state);
	void OnControllerConnect(vr::TrackedDeviceIndex_t controllerId);
	void OnControllerDisconnect(vr::TrackedDeviceIndex_t controllerId);
	int GetControllerDeviceId(EHmdController id) { return m_ControllerIndex[id]; }
	bool IsConnected(EHmdController id) const;
	vr::HmdMatrix34_t GetControllerTransform(EHmdController id);
private:
	vr::HmdMatrix34_t m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

	int m_ControllerIndex[eController_OpenVR_MaxNum];

	COpenVRDevice* m_pDevice;
};
}
#endif
