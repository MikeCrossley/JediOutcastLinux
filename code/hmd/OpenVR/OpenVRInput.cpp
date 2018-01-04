#include "OpenVRInput.h"
#include "OpenVRDevice.h"


#include <math.h>
#include <iostream>
#include <algorithm>

#ifdef _WINDOWS
#include <stdlib.h>
#include <dwmapi.h>
#endif

#include "../../client/client.h"

using namespace OpenVR;

COpenVRInput::COpenVRInput(COpenVRDevice* pDevice)
    : m_pDevice(pDevice)
{
}

bool COpenVRInput::Init()
{
	Cbuf_AddText("exec jk2vive.cfg\n");

	VID_Printf(PRINT_ALL, "[HMD][OpenVR] OpenVR Input Initialized\n");

	return true;
}

void COpenVRInput::Update()
{
	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
	{
		vr::VRControllerState_t state;
		vr::VRSystem()->GetControllerState(m_pDevice->GetTrackedDevices()->GetControllerDeviceId(EHmdController(i)), &state, sizeof(VRControllerState_t));
		m_vrControllerState[i] = state;
	}

	for (int i = 0; i < eController_OpenVR_MaxNum; i++)
	{
		vr::VRControllerState_t &vrState = m_vrControllerState[i];

		m_ControllerState[i].nPacketNum = vrState.unPacketNum;
		m_ControllerState[i].nButtonsPressed = vrState.ulButtonPressed;
		m_ControllerState[i].nButtonsTouched = vrState.ulButtonTouched;

		m_ControllerState[i].fTriggerValue = vrState.rAxis[vr::k_EButton_SteamVR_Trigger & 0x0f].x;
		m_ControllerState[i].vTouchPad[0] = vrState.rAxis[vr::k_EButton_SteamVR_Touchpad & 0x0f].x;
		m_ControllerState[i].vTouchPad[1] = vrState.rAxis[vr::k_EButton_SteamVR_Touchpad & 0x0f].y;
	}

	m_CurrentButtonState[EVRButtons::eButtonTrigger] = GetControllerState(eController_OpenVR_2).IsPressed(vr::EVRButtonId::k_EButton_SteamVR_Trigger) ? EButtonState::eButtonPressed : EButtonState::eButtonReleased;
	m_CurrentButtonState[EVRButtons::eButtonTouchPad] = GetControllerState(eController_OpenVR_2).IsPressed(vr::EVRButtonId::k_EButton_SteamVR_Touchpad) ? EButtonState::eButtonPressed : EButtonState::eButtonReleased;
	m_CurrentButtonState[EVRButtons::eButtonApplication] = GetControllerState(eController_OpenVR_2).IsPressed(vr::EVRButtonId::k_EButton_ApplicationMenu) ? EButtonState::eButtonPressed : EButtonState::eButtonReleased;
	m_CurrentButtonState[EVRButtons::eButtonGrip] = GetControllerState(eController_OpenVR_2).IsPressed(vr::EVRButtonId::k_EButton_Grip) ? EButtonState::eButtonPressed : EButtonState::eButtonReleased;
}

bool COpenVRInput::PollChangedButton(size_t &rButtonId, bool &pressed)
{
	for (int i = 0; i < eButtonMax; i++)
	{
		if (m_CurrentButtonState[i] != m_PreviousButtonState[i])
		{
			rButtonId = i;
			pressed = (m_CurrentButtonState[i] == EButtonState::eButtonPressed) ? true : false;

			m_PreviousButtonState[i] = m_CurrentButtonState[i];

			return true;
		}
	}

	return false;
}

float COpenVRInput::GetAxisValue(size_t axisId)
{
	SControllerState &state = GetControllerState(eController_OpenVR_1);

	float fAxisValue = state.GetTouchpadAxis(axisId);

	if (axisId == AXIS_FORWARD)
		fAxisValue *= -1.f;

	return fAxisValue;
}

bool COpenVRInput::PollChangedAxis(size_t &rAxisId, float &axisValue)
{
	return false;
}