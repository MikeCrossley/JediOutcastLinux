/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRRENDERER_H
#define OPENVRRENDERER_H

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

namespace OpenVR
{
class COpenVRDevice;

class COpenVRRenderer : public IHmdRenderer
{
public:
	COpenVRRenderer(COpenVRDevice* pDevice);
    virtual ~COpenVRRenderer();

    virtual bool Init(int windowWidth, int windowHeight, PlatformInfo platformInfo) override;
    virtual void Shutdown() override;

	virtual std::string GetInfo() override { return "OpenVRRenderer"; }

	virtual bool HandlesSwap() override { return false; }
    virtual bool GetRenderResolution(int& rWidth, int& rHeight) override;

    virtual void StartFrame() override;
    virtual void BeginRenderingForEye(bool leftEye) override;
    virtual void EndFrame() override;

    virtual bool GetCustomProjectionMatrix(float* rProjectionMatrix, float zNear, float zFar, float fov) override;
    virtual bool GetCustomViewMatrix(float* rViewMatrix, vec3_t vCameraOrigin, float bodyYaw, bool noPosition) override;

    virtual bool Get2DViewport(int& rX, int& rY, int& rW, int& rH) override;
    virtual bool Get2DOrtho(double &rLeft, double &rRight, double &rBottom, double &rTop, double &rZNear, double &rZFar) override;

	virtual void SetCurrentHmdMode(HmdMode mode) override;
    virtual bool HasQuadWorldPosSupport() override { return true; }
protected:
	bool CreateTextureSwapChain(int nWidth, int nHeight, GLuint* Texture);
	void ConvertMatrix(const vr::HmdMatrix44_t& from, float* rTo);

private:
	COpenVRDevice* m_pDevice;

    void PreparePlatform();

    static const int FBO_COUNT = 2;
    RenderTool::FrameBufferInfo mFboInfos[FBO_COUNT];
    RenderTool::FrameBufferInfo mFboMenuInfo;

    bool m_bIsInitialized;

	bool m_bStartedFrame;
	bool m_bStartedRendering;

	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	vr::VROverlayHandle_t m_ulOverlayHandle;
	//

	int mEyeId;

    int mWindowWidth;
    int mWindowHeight;

    int mRenderWidth;
    int mRenderHeight;

    float mMeterToGameUnits;

	GLuint mEyeTextureSet[2];
    GLuint mEyeStencilBuffer[2]; 
    GLuint mMenuStencilDepthBuffer;

	GLuint mMenuTextureSet;

    GLuint mReadFBO;
    
    HmdMode mCurrentHmdMode;
};
}
#endif
