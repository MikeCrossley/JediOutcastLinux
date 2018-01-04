/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2014 by Jochen Leopold <jochen.leopold@model-view.com>
 */

#ifndef CLIENTHMD_H
#define CLIENTHMD_H

#include "../game/q_shared.h"

class IHmdDevice;
class IHmdRenderer;
class IHmdInput;
class GameMenuHmdManager;

class ClientHmd
{
public:
    ClientHmd();
    ~ClientHmd();

    static ClientHmd* Get();
    static void Destroy();

    void UpdateInputView(float yawDiff, float pitchDiff, float& rPitch, float& rYaw, float& rRoll);
    void UpdateGame();
    bool GetOrientation(float& rPitch, float& rYaw, float& rRoll);
    bool GetPosition(float& rX, float& rY, float& rZ);
    bool GetHandOrientation(bool rightHand, float& rPitch, float& rYaw, float& rRoll);
    bool GetHandPosition(bool rightHand, float& rX, float& rY, float& rZ);
    bool HasHand(bool rightHand);

	bool GetHMDWorldPosition(float &fX, float &fY, float &fZ) { fX = m_vHmdWorldPos[0]; fY = m_vHmdWorldPos[1]; fZ = m_vHmdWorldPos[2]; return true; }
	void SetHMDWorldPosition(float fX, float fY, float fZ) { m_vHmdWorldPos[0] = fX; m_vHmdWorldPos[1] = fY; m_vHmdWorldPos[2] = fZ; }

    IHmdDevice* GetDevice() { return mpDevice; }
    void SetDevice(IHmdDevice* pDevice) { mpDevice = pDevice; }

    IHmdRenderer* GetRenderer() { return mpRenderer; }
    void SetRenderer(IHmdRenderer* pRenderer);

    IHmdInput* GetInput() { return mpInput; }
    void SetInput(IHmdInput* pInput);

    float GetYawDiff() { return mViewangleDiff; }

    GameMenuHmdManager* GetGameMenuHmdManager();

private:

    // disable copy constructor
    ClientHmd(const ClientHmd&);
    ClientHmd& operator=(const ClientHmd&);

    IHmdDevice* mpDevice;
    IHmdRenderer* mpRenderer;
    IHmdInput* mpInput;
    GameMenuHmdManager* mpGameMenuHmdManager;
    bool mIsInitialized;
    float mLastViewangleYaw;
    float mViewangleDiff;
    float mLastViewanglePitch;
    float mViewanglePitchDiff;
    float mLastPitch;
    float mMeterToGameUnits;

	vec3_t m_vHmdWorldPos;

    static ClientHmd* sClientHmd;
};

#endif
