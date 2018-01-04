#include "OpenVRRenderer.h"

#include "OpenVRDevice.h"

#include "../../renderer/tr_local.h"
#include "../HmdRenderer/PlatformInfo.h"

#include <math.h>
#include <iostream>
#include <algorithm>

#ifdef _WINDOWS
#include <stdlib.h>
#include <dwmapi.h>
#endif

#include "OpenVRHelper.h"

#include "../../client/client.h"

using namespace OpenVR;

COpenVRRenderer::COpenVRRenderer(COpenVRDevice* pDevice)
	: m_bIsInitialized(false)
	, m_bStartedFrame(false)
	, m_bStartedRendering(false)
	, m_ulOverlayHandle(vr::k_ulOverlayHandleInvalid)
    , mEyeId(-1)
    , mWindowWidth(0)
    , mWindowHeight(0)
    , mRenderWidth(0)
    , mRenderHeight(0)
    , mMeterToGameUnits(IHmdDevice::METER_TO_GAME)
    , m_pDevice(pDevice)
    , mMenuStencilDepthBuffer(0)
    , mCurrentHmdMode(GAMEWORLD)
{

}

COpenVRRenderer::~COpenVRRenderer()
{

}

bool COpenVRRenderer::Init(int windowWidth, int windowHeight, PlatformInfo platformInfo)
{
    if (!m_pDevice)
        return false;

	PreparePlatform();

    mWindowWidth = windowWidth;
    mWindowHeight = windowHeight;

	uint32_t nWidth, nHeight;

	//m_pDevice->GetDeviceResolution(nWidth, nHeight, bA, bA);

	m_pDevice->GetHMDSystem()->GetRecommendedRenderTargetSize(&nWidth, &nHeight);

	mRenderWidth = nWidth;
    mRenderHeight = nHeight;
    
	if (!vr::VRCompositor())
	{
		VID_Printf(PRINT_ALL, "[HMD][OpenVR] Compositor initialization failed. See log file for details\n");
		return false;
	}

	vr::VRCompositor()->ShowMirrorWindow();

	for (int i = 0; i < FBO_COUNT; i++)
	{
		bool bSuccess = RenderTool::CreateFrameBufferWithoutTextures(mFboInfos[i], mRenderWidth, mRenderHeight);
		if (!bSuccess)
		{
			return false;
		}

		CreateTextureSwapChain(mRenderWidth, mRenderHeight, &mEyeTextureSet[i]);

		qglGenTextures(1, &(mEyeStencilBuffer[i]));
		qglBindTexture(GL_TEXTURE_2D, mEyeStencilBuffer[i]);
		//qglTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, mRenderWidth, mRenderHeight, 0, GL_STENCIL, GL_UNSIGNED_BYTE, 0);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	}

	bool success = RenderTool::CreateFrameBufferWithoutTextures(mFboMenuInfo, mRenderWidth, mRenderHeight);
	if (!success)
	{
		return false;
	}

	CreateTextureSwapChain(mRenderWidth, mRenderHeight, &mMenuTextureSet);

	qglGenTextures(1, &mMenuStencilDepthBuffer);
	qglBindTexture(GL_TEXTURE_2D, mMenuStencilDepthBuffer);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	if (vr::VROverlay())
	{
		vr::VROverlayError overlayError = vr::VROverlay()->CreateOverlay("Key", "Menu", &m_ulOverlayHandle);
		if (overlayError != vr::VROverlayError_None)
		{
			VID_Printf(PRINT_ALL, "[HMD][OpenVR] Failed to create overlay!\n");
		}
		else
		{
			vr::HmdMatrix34_t mOffsetMatrix = CreateMatrix34();

			mOffsetMatrix.m[0][3] = 0.f;
			mOffsetMatrix.m[1][3] = 1.35f;
			mOffsetMatrix.m[2][3] = -1.1f;

			vr::VROverlay()->SetOverlayWidthInMeters(m_ulOverlayHandle, 1.5f);
			vr::VROverlay()->SetOverlayInputMethod(m_ulOverlayHandle, vr::VROverlayInputMethod_Mouse);
			vr::VROverlay()->SetOverlayTransformAbsolute(m_ulOverlayHandle, vr::VRCompositor()->GetTrackingSpace(), &mOffsetMatrix);

			vr::Texture_t menuTexture = { (void*)mMenuTextureSet, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
			vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &menuTexture);

			VID_Printf(PRINT_ALL, "[HMD][OpenVR] OpenVR Overlay Initialized!\n");
		}
	}

	m_bStartedRendering = false;
	m_bIsInitialized = true;

    return true;
}

void COpenVRRenderer::Shutdown()
{
	if (!m_bIsInitialized)
		return;

    for (int i = 0; i < FBO_COUNT; i++)
    {
        qglDeleteFramebuffers(1, &mFboInfos[i].Fbo);
		qglDeleteTextures(1, &mEyeTextureSet[i]);
	}

	m_bIsInitialized = false;
}

bool COpenVRRenderer::CreateTextureSwapChain(int nWidth, int nHeight, GLuint* Texture)
{
	qglGenTextures(1, Texture);
	qglBindTexture(GL_TEXTURE_2D, *Texture);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	return true;
}

bool COpenVRRenderer::GetRenderResolution(int& rWidth, int& rHeight)
{
    rWidth = mRenderWidth;
    rHeight = mRenderHeight;

    return true;
}

void COpenVRRenderer::StartFrame()
{
	m_bStartedFrame = true;
}

void COpenVRRenderer::BeginRenderingForEye(bool leftEye)
{
	if (!m_bIsInitialized || !m_bStartedFrame)
		return;

	int fboId = 0;
	if (!leftEye && FBO_COUNT > 1)
	{
		fboId = 1;
	}

	mEyeId = fboId;

	if (!m_bStartedRendering)
	{
		m_bStartedRendering = true;
		// render begin
		//qglDisable(GL_SCISSOR_TEST);
		qglBindVertexArray(0);
		qglUseProgramObjectARB(0);
		qglFrontFace(GL_CCW);

		qglDisable(GL_FRAMEBUFFER_SRGB);

		// Update the tracked device events
		m_pDevice->GetTrackedDevices()->UpdateEvents();

		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
			{
				m_pDevice->GetTrackedDevices()->SetTrackingMatrix(nDevice, m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			}

			vr::ETrackedDeviceClass devClass = m_pDevice->GetHMDSystem()->GetTrackedDeviceClass(nDevice);
			switch (devClass)
			{
				case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
				{
					vr::VRControllerState_t state;
					if (m_pDevice->GetHMDSystem()->GetControllerState(nDevice, &state, sizeof(vr::VRControllerState_t)))
					{
						m_pDevice->GetTrackedDevices()->UpdateController(nDevice, m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking, state);
					}
				}
				break;
			}
		}

		for (int i = 0; i < FBO_COUNT; i++)
		{
			qglBindFramebuffer(GL_FRAMEBUFFER, mFboInfos[i].Fbo);
			qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mEyeTextureSet[i], 0);
			qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mEyeStencilBuffer[i], 0);

			RenderTool::ClearFBO(mFboInfos[i]);
		}

		qglBindFramebuffer(GL_FRAMEBUFFER, mFboMenuInfo.Fbo);
		qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMenuTextureSet, 0);
		qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mMenuStencilDepthBuffer, 0);

		RenderTool::ClearFBO(mFboMenuInfo);
	}

	// Bind framebuffer
	// This part can be called multiple times before the end of the frame

	if (mCurrentHmdMode == GAMEWORLD)
	{
		qglBindFramebuffer(GL_FRAMEBUFFER, mFboInfos[mEyeId].Fbo);
	}
	else
	{
		if (leftEye)
		{
			qglBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			qglBindFramebuffer(GL_FRAMEBUFFER, mFboMenuInfo.Fbo);
		}
	}
}

void COpenVRRenderer::EndFrame()
{
	if (!m_bIsInitialized || !m_bStartedFrame)
		return;

	if (m_bStartedFrame)
	{
		GLboolean depth_test = qglIsEnabled(GL_DEPTH_TEST);
		GLboolean blend = qglIsEnabled(GL_BLEND);
		GLboolean texture_2d = qglIsEnabled(GL_TEXTURE_2D);
		GLboolean texture_coord_array = qglIsEnabled(GL_TEXTURE_COORD_ARRAY);
		GLboolean color_array = qglIsEnabled(GL_COLOR_ARRAY);

		qglDisable(GL_SCISSOR_TEST);
		qglDisable(GL_STENCIL_TEST);
		qglBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (mCurrentHmdMode == MENU_QUAD_WORLDPOS)
		{
			ControllerEvents();

			vr::VROverlay()->HandleControllerOverlayInteractionAsMouse(m_ulOverlayHandle, m_pDevice->GetTrackedDevices()->GetControllerDeviceId(EHmdController::eController_OpenVR_2));
			vr::VROverlay()->SetGamepadFocusOverlay(m_ulOverlayHandle);

			vr::Texture_t menuTexture = { (void*)mMenuTextureSet, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
			vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &menuTexture);
		}

		vr::Texture_t leftEyeTexture = { (void*)mEyeTextureSet[0], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)mEyeTextureSet[1], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		vr::VRCompositor()->PostPresentHandoff();


		// Mirror
		qglBindBuffer(GL_ARRAY_BUFFER, 0);
		qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		qglViewport(0, 0, mWindowWidth, mWindowHeight);

		qglBindFramebuffer(GL_READ_FRAMEBUFFER, (mCurrentHmdMode == GAMEWORLD_QUAD_WORLDPOS || mCurrentHmdMode == MENU_QUAD_WORLDPOS) ? mMenuTextureSet : mEyeTextureSet[0]);
		qglBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		qglDrawBuffer(GL_BACK);

		qglBlitFramebuffer(0, 0, mRenderWidth, mRenderHeight,
			0, 0, mWindowWidth, mWindowHeight,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

		qglBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		// ~Mirror

		m_bStartedFrame = false;
		m_bStartedRendering = false;

		if (depth_test)
		{
			qglEnable(GL_DEPTH_TEST);
		}
		if (blend)
		{
			qglEnable(GL_BLEND);
		}
		if (!texture_2d)
		{
			qglDisable(GL_TEXTURE_2D);
		}
		if (!texture_coord_array)
		{
			qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (color_array)
		{
			qglEnableClientState(GL_COLOR_ARRAY);
		}
	}
	mEyeId = -1;
}

#include "../../ui/ui_local.h"

void COpenVRRenderer::ControllerEvents()
{
	vr::VREvent_t vrEvent;
	while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayHandle, &vrEvent, sizeof(vrEvent)))
	{
		switch (vrEvent.eventType)
		{
			case vr::VREvent_MouseMove:
			{
				int nWidth = SCREEN_WIDTH;
				int nHeight = SCREEN_HEIGHT;

				_UI_MouseSet(int(vrEvent.data.mouse.x * nWidth), int(vrEvent.data.mouse.y * nHeight));
			}
			break;
			case vr::VREvent_MouseButtonDown:
			{
				_UI_KeyEvent(A_MOUSE1, true);
			}
			break;
			case vr::VREvent_MouseButtonUp:
			{
				_UI_KeyEvent(A_MOUSE1, false);
			}
			break;
		}
	}
}

bool COpenVRRenderer::GetCustomProjectionMatrix(float* rProjectionMatrix, float zNear, float zFar, float fov)
{
	if (!m_bIsInitialized)
		return false;

	zNear = 0.1f;

	vr::HmdMatrix44_t projMatrix = m_pDevice->GetHMDSystem()->GetProjectionMatrix((vr::EVREye)mEyeId, zNear, zFar);
	ConvertMatrix(projMatrix, rProjectionMatrix);

    return true;
}

void COpenVRRenderer::ConvertMatrix(const vr::HmdMatrix44_t& from, float* rTo)
{
	rTo[0] = from.m[0][0];
	rTo[4] = from.m[0][1];
	rTo[8] = from.m[0][2];
	rTo[12] = from.m[0][3];

	rTo[1] = from.m[1][0];
	rTo[5] = from.m[1][1];
	rTo[9] = from.m[1][2];
	rTo[13] = from.m[1][3];

	rTo[2] = from.m[2][0];
	rTo[6] = from.m[2][1];
	rTo[10] = from.m[2][2];
	rTo[14] = from.m[2][3];

	rTo[3] = from.m[3][0];
	rTo[7] = from.m[3][1];
	rTo[11] = from.m[3][2];
	rTo[15] = from.m[3][3];
}

bool COpenVRRenderer::GetCustomViewMatrix(float* rViewMatrix, vec3_t &vOrigin, float bodyYaw, bool noPosition)
{
	if (!m_bIsInitialized)
		return false;

	vr::HmdMatrix34_t mHmdPose = m_pDevice->GetTrackedDevices()->GetMatrix(vr::k_unTrackedDeviceIndex_Hmd);
	vr::HmdQuaternion_t mHmdQuat = GetRotation(mHmdPose);

	glm::quat currentOrientation = glm::quat(mHmdQuat.w, mHmdQuat.x, mHmdQuat.y, mHmdQuat.z);
	glm::vec3 currentPosition = glm::vec3(mHmdPose.m[0][3], mHmdPose.m[1][3], mHmdPose.m[2][3]);

	if (mCurrentHmdMode == MENU_QUAD_WORLDPOS || mCurrentHmdMode == GAMEWORLD_QUAD_WORLDPOS || mCurrentHmdMode == MENU_QUAD)
	{
		currentOrientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// get current hmd rotation 
	glm::quat hmdRotation = glm::inverse(currentOrientation);

	// change hmd orientation to game coordinate system
	glm::quat convertHmdToGame = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), (float)DEG2RAD(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	convertHmdToGame = glm::rotate(convertHmdToGame, (float)DEG2RAD(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 hmdRotationMat = glm::mat4_cast(hmdRotation) * glm::mat4_cast(convertHmdToGame);


	// convert body transform to matrix
	glm::mat4 bodyPositionMat = glm::translate(glm::mat4(1.0f), glm::vec3(-vOrigin[0], -vOrigin[1], -vOrigin[2]));
	glm::quat bodyYawRotation = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), (float)(DEG2RAD(-bodyYaw)), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 hmdPosition;
	hmdPosition.x = currentPosition.z * mMeterToGameUnits;
	hmdPosition.y = currentPosition.x * mMeterToGameUnits;
	hmdPosition.z = currentPosition.y * -mMeterToGameUnits;

	glm::mat4 hmdPositionMat = glm::translate(glm::mat4(1.0f), hmdPosition);

	// create view matrix
	glm::mat4 viewMatrix;
	if (noPosition)
	{
		viewMatrix = hmdRotationMat * glm::mat4_cast(bodyYawRotation) * bodyPositionMat;
	}
	else
	{
		viewMatrix = hmdRotationMat * hmdPositionMat * glm::mat4_cast(bodyYawRotation) * bodyPositionMat;
	}


	memcpy(rViewMatrix, &viewMatrix[0][0], sizeof(float) * 16);

	if (noPosition)
	{
		return true;
	}

	// add hmd offset to body pos

	glm::quat bodyYawRotationReverse = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), (float)(DEG2RAD(bodyYaw)), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 offsetMat = glm::mat4_cast(bodyYawRotationReverse) * hmdPositionMat;
	glm::vec3 offsetPos = glm::vec3(offsetMat[3]);

	//Vector3f hmdPos2 = Vector3f(hmdPos.x, hmdPos.y, hmdPos.z);

	//Matrix4f bodyYawRotationReverse = Matrix4f::RotationZ(DEG2RAD(bodyYaw));
	//Vector3f offsetPos = (bodyYawRotationReverse * Matrix4f::Translation(hmdPos2)).GetTranslation();

	/// TODO: do we need this?
	offsetPos *= -1;

	vOrigin[0] += offsetPos.x;
	vOrigin[1] += offsetPos.y;
	vOrigin[2] += offsetPos.z;

	return true;
}

bool COpenVRRenderer::Get2DViewport(int& rX, int& rY, int& rW, int& rH)
{
	if (mCurrentHmdMode == MENU_QUAD_WORLDPOS || mCurrentHmdMode == GAMEWORLD_QUAD_WORLDPOS || mCurrentHmdMode == MENU_QUAD)
	{
		return true;
	}

	float fGuiScale = 1.0f;

	rW = mRenderWidth * fGuiScale;
	rH = mRenderWidth * fGuiScale;

	rX = (mRenderWidth - rW) / 2.0f;
	rY = (mRenderHeight - rH) / 2.0f;

    return true;
}

bool COpenVRRenderer::Get2DOrtho(double &rLeft, double &rRight, double &rBottom, double &rTop, double &rZNear, double &rZFar)
{
    rLeft = 0;
    rRight = 640;
    rBottom = 480;
    rTop = 0;
    rZNear = 0;
    rZFar = 1;

    return true;
}

void COpenVRRenderer::SetCurrentHmdMode(HmdMode mode)
{
	if (mode == GAMEWORLD_QUAD_WORLDPOS || mode == MENU_QUAD_WORLDPOS)
	{
		vr::VROverlay()->ShowOverlay(m_ulOverlayHandle);
	}
	else
	{
		vr::VROverlay()->HideOverlay(m_ulOverlayHandle);
	}

	mCurrentHmdMode = mode;
}

void COpenVRRenderer::PreparePlatform()
{
    RenderTool::SetVSync(false);
}

