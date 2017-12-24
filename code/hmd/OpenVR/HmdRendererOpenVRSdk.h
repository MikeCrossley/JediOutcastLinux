/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef HMDRENDEREROPENVRSDK_H
#define HMDRENDEREROPENVRSDK_H

#include "../HmdRenderer/IHmdRenderer.h"
#include "../../renderer/qgl.h"


#include <openvr.h>
#include <openvr_capi.h>

#include "../HmdRenderer/RenderTool.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace OpenVRSDK
{
class HmdDeviceOpenVRSdk;

struct FramebufferDesc
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

class HmdRendererOpenVRSdk : public IHmdRenderer
{
public:
	HmdRendererOpenVRSdk(HmdDeviceOpenVRSdk* pDevice);
    virtual ~HmdRendererOpenVRSdk();

    virtual bool Init(int windowWidth, int windowHeight, PlatformInfo platformInfo) override;
    virtual void Shutdown() override;

    virtual std::string GetInfo() override;

    virtual bool HandlesSwap() override;
    virtual bool GetRenderResolution(int& rWidth, int& rHeight) override;

    virtual void StartFrame() override;
    virtual void BeginRenderingForEye(bool leftEye) override;
    virtual void EndFrame() override;

    virtual bool GetCustomProjectionMatrix(float* rProjectionMatrix, float zNear, float zFar, float fov) override;
    virtual bool GetCustomViewMatrix(float* rViewMatrix, float& xPos, float& yPos, float& zPos, float bodyYaw, bool noPosition) override;

    virtual bool Get2DViewport(int& rX, int& rY, int& rW, int& rH) override;
    virtual bool Get2DOrtho(double &rLeft, double &rRight, double &rBottom, double &rTop, double &rZNear, double &rZFar) override;

    virtual void SetCurrentHmdMode(HmdMode mode) override;
    virtual bool HasQuadWorldPosSupport() override { return true; }
protected:
	bool CreateTextureSwapChain(int nWidth, int nHeight, GLuint* Texture);
	void ConvertMatrix(const vr::HmdMatrix44_t& from, float* rTo);
	vr::HmdQuaternion_t GetRotation(vr::HmdMatrix34_t matrix);

private:
    void PreparePlatform();

    static const int FBO_COUNT = 2;
    RenderTool::FrameBufferInfo mFboInfos[FBO_COUNT];
    RenderTool::FrameBufferInfo mFboMenuInfo;

    bool m_bIsInitialized;

	bool m_bStartedFrame;
	bool m_bStartedRendering;

	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::HmdMatrix34_t m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VROverlayHandle_t m_ulOverlayHandle;
	//

    double mFrameStartTime;
    int mEyeId;

    int mWindowWidth;
    int mWindowHeight;

    int mRenderWidth;
    int mRenderHeight;

    float mGuiScale;
    float mGuiOffsetFactorX;

    float mMeterToGameUnits;

	HmdDeviceOpenVRSdk* mpDevice;
    
	FramebufferDesc frameBuffers[2];

	GLuint mEyeTextureSet[2];
    GLuint mEyeStencilBuffer[2]; 
    GLuint mMenuStencilDepthBuffer;

	GLuint mMenuTextureSet;

    GLuint mReadFBO;
    
    HmdMode mCurrentHmdMode;
};
}
#endif
