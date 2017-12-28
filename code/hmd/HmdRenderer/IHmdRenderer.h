/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2014 by Jochen Leopold <jochen.leopold@model-view.com>
 */

#ifndef IHMDRENDERER_H
#define IHMDRENDERER_H

#include <string>
#include "../../game/q_shared.h"

struct PlatformInfo;

class IHmdRenderer
{
public:
    enum HmdMode
    {
        MENU_QUAD,
        MENU_QUAD_WORLDPOS,
        GAMEWORLD,
        GAMEWORLD_QUAD_WORLDPOS
    };

    virtual ~IHmdRenderer() {}

    virtual bool Init(int windowWidth, int windowHeight, PlatformInfo platformInfo) = 0;
    virtual void Shutdown() = 0;

    virtual std::string GetInfo() = 0;

    virtual bool HandlesSwap() = 0;
    virtual bool GetRenderResolution(int& rWidth, int& rHeight) = 0;

    virtual void StartFrame() = 0;
    
    // has to be called before rendering or any call to eye dependend methodes like GetCustomProjectionMatrix
    virtual void BeginRenderingForEye(bool leftEye) = 0;
    virtual void EndFrame() = 0;

    virtual bool GetCustomProjectionMatrix(float* rProjectionMatrix, float zNear, float zFar, float fov) = 0;
    virtual bool GetCustomViewMatrix(float* rViewMatrix, vec3_t vCameraOrigin, float bodyYaw, bool noPosition) = 0;

    virtual bool Get2DViewport(int& rX, int& rY, int& rW, int& rH) = 0;
    virtual bool Get2DOrtho(double &rLeft, double &rRight, double &rBottom, double &rTop, double &rZNear, double &rZFar) = 0;

    virtual void SetCurrentHmdMode(HmdMode mode) = 0;
    virtual bool HasQuadWorldPosSupport() { return false; }


protected:
    const float IPD_SCALE = 0.65f;


};

#endif
