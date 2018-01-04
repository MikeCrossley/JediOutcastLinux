/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRINPUT_H
#define OPENVRINPUT_H

#include "../HmdInput/IHmdInput.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "../../code/game/q_shared.h"

#include <openvr.h>
#include <openvr_capi.h>

#include "OpenVRTracking.h"

namespace OpenVR
{
	class COpenVRDevice;

	enum EVRButtons
	{
		eButtonTrigger = 0,
		eButtonTouchPad,
		eButtonApplication,
		eButtonGrip,
		eButtonMax,
	};

	enum EButtonState
	{
		eButtonReleased = 0,
		eButtonPressed,
	};

	struct SControllerState
	{
		uint32_t	nPacketNum;
		uint64_t	nButtonsPressed;
		uint64_t	nButtonsTouched;
		float		fTriggerValue;
		vec2_t		vTouchPad;

		bool IsPressed(vr::EVRButtonId btn)
		{
			return (nButtonsPressed & vr::ButtonMaskFromId(btn)) > 0;
		}

		bool IsTouched(vr::EVRButtonId btn)
		{
			return (nButtonsTouched & vr::ButtonMaskFromId(btn)) > 0;
		}

		float GetTriggerValue() { return fTriggerValue; }

		float GetTouchpadAxis(int nAxis) { return vTouchPad[nAxis]; }
	};

class COpenVRInput : public IHmdInput
{
public:
	COpenVRInput(COpenVRDevice* pDevice);
	virtual ~COpenVRInput() {};

	// IHmdInput
	virtual bool Init() override;
	virtual void Shutdown() override {}
	virtual void Update() override;
	size_t GetButtonCount() override { return 0; }
	bool IsButtonPressed(size_t buttonId) override { return false; }
	virtual size_t GetAxisCount() override { return 2; }
	float GetAxisValue(size_t axisId) override;
	virtual bool PollChangedButton(size_t &rButtonId, bool &pressed);
	virtual bool PollChangedAxis(size_t &rAxisId, float &axisValue);
	//~IHmdInput

	SControllerState GetControllerState(EHmdController id) { return m_ControllerState[id]; }
private:
	COpenVRDevice* m_pDevice;

	vr::VRControllerState_t		m_vrControllerState[eController_OpenVR_MaxNum];
	SControllerState			m_ControllerState[eController_OpenVR_MaxNum];

	EButtonState				m_CurrentButtonState[eButtonMax];
	EButtonState				m_PreviousButtonState[eButtonMax];

	float						m_CurrentAxisState[4];
	float						m_PreviousAxisState[4];
};
}
#endif
