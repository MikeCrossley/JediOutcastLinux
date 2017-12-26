#include "OpenVRTracking.h"
#include "OpenVRDevice.h"

#include "../../client/client.h"

using namespace OpenVR;

void COpenVRTracking::Init(COpenVRDevice* pDevice)
{
	m_pDevice = pDevice;

	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
		m_ControllerIndex[i] = vr::k_unMaxTrackedDeviceCount;

	// Look for already connected devices
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		vr::ETrackedDeviceClass devClass = m_pDevice->GetHMDSystem()->GetTrackedDeviceClass(i);
		switch (devClass)
		{
			case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
			{
				VID_Printf(PRINT_ALL, "[HMD][OpenVR] - Controller Already Connected: %d \n",i);
				OnControllerConnect(i);
			}
			break;
		}
	}
}

void COpenVRTracking::UpdateEvents()
{
	if (!m_pDevice)
		return;

	vr::IVRSystem* pSystem = m_pDevice->GetHMDSystem();
	vr::VREvent_t event;
	while (pSystem->PollNextEvent(&event, sizeof(vr::VREvent_t)))
	{
		switch (event.eventType)
		{
			case vr::VREvent_TrackedDeviceActivated:
			{
				vr::ETrackedDeviceClass devClass = pSystem->GetTrackedDeviceClass(event.trackedDeviceIndex);
				switch (devClass)
				{
					case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
					{
						VID_Printf(PRINT_ALL, "[HMD][OpenVR] - Controller Connected: %d \n", event.trackedDeviceIndex);
						OnControllerConnect(event.trackedDeviceIndex);
					}
					break;
				}
			}
			break;
		case vr::VREvent_TrackedDeviceDeactivated:
		{
			vr::ETrackedDeviceClass devClass = pSystem->GetTrackedDeviceClass(event.trackedDeviceIndex);
			switch (devClass)
			{
				case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
				{
					VID_Printf(PRINT_ALL, "[HMD][OpenVR] - Controller Disconnected: %d \n", event.trackedDeviceIndex);

					OnControllerDisconnect(event.trackedDeviceIndex);
				}
				break;
			}
		}
		break;
		}
	}
}

void COpenVRTracking::OnControllerConnect(vr::TrackedDeviceIndex_t controllerId)
{
	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
	{
		if (m_ControllerIndex[i] >= vr::k_unMaxTrackedDeviceCount)
		{
			m_ControllerIndex[i] = controllerId;
			break;
		}
	}
}

void COpenVRTracking::OnControllerDisconnect(vr::TrackedDeviceIndex_t controllerId)
{
	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
	{
		if (m_ControllerIndex[i] == controllerId)
		{
			m_ControllerIndex[i] = vr::k_unMaxTrackedDeviceCount;
			break;
		}
	}
}

vr::HmdMatrix34_t COpenVRTracking::GetControllerTransform(EHmdController id)
{
	return m_rmat4DevicePose[m_ControllerIndex[id]];
}

bool COpenVRTracking::IsConnected(EHmdController id) const
{
	return m_pDevice->GetHMDSystem()->IsTrackedDeviceConnected(m_ControllerIndex[id]);
}

void COpenVRTracking::UpdateController(int nDeviceId, vr::HmdMatrix34_t mMatrix, vr::VRControllerState_t state)
{
	/*EHmdController index = eController_OpenVR_MaxNum;
	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
	{
		if (m_ControllerIndex[i] == nDeviceId)
		{
			index = static_cast<EHmdController>(i);
			break;
		}
	}*/
}