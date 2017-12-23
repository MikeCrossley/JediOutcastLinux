#include "FactoryHmdDevice.h"
#include "HmdDevice/HmdDeviceMouse.h"
#ifdef USE_OPENHMD
#include "HmdDevice/HmdDeviceOpenHmd.h"
#include "HmdRenderer/HmdRendererOculusOpenHmd.h"
#endif

#ifdef USE_OVR_1
#include "OculusSdk_1/HmdDeviceOculusSdk.h"
#include "OculusSdk_1/HmdRendererOculusSdk.h"
#include "OculusSdk_1/HmdInputOculusSdk.h"
#endif

#ifdef USE_OVR_0_8
#include "OculusSdk_0.8/HmdDeviceOculusSdk.h"
#include "OculusSdk_0.8/HmdRendererOculusSdk.h"
#endif

#ifdef USE_OVR_0_5
#include "OculusSdk_0.5/HmdDeviceOculusSdk.h"
#include "OculusSdk_0.5/HmdRendererOculusSdk.h"
#endif

#ifdef USE_OPENVR
#include "OpenVR\HmdDeviceOpenVRSdk.h"
#include "OpenVR\HmdRendererOpenVRSdk.h"
#endif


#include "HmdRenderer/HmdRendererOculus.h"

#include "../client/client.h"

#include <vector>
#include <memory.h>

using namespace std;

IHmdDevice* FactoryHmdDevice::CreateHmdDevice(HmdLibrary library, bool allowDummyDevice)
{
	VID_Printf(PRINT_ALL, "[FactoryHmdDevice] Creating HMD Device\n");

	vector<IHmdDevice*> devices;

#ifdef USE_OPENVR
	if (library == LIB_OPENVR || library == LIB_UNDEFINED)
	{
		devices.push_back(new OpenVRSDK::HmdDeviceOpenVRSdk());
	}
#endif


#ifdef USE_OVR_1
    if (library == LIB_OVR || library == LIB_UNDEFINED)
    {
        devices.push_back(new OvrSdk_1::HmdDeviceOculusSdk());
    }
#endif

#ifdef USE_OVR_0_8
    if (library == LIB_OVR || library == LIB_UNDEFINED)
    {
        devices.push_back(new OvrSdk_0_8::HmdDeviceOculusSdk());
    }
#endif
    
#ifdef USE_OVR_0_5
    if (library == LIB_OVR || library == LIB_UNDEFINED)
    {
        devices.push_back(new OvrSdk_0_5::HmdDeviceOculusSdk());
    }
#endif
    
#ifdef USE_OPENHMD
    if (library == LIB_OPENHMD || library == LIB_UNDEFINED)
    {
        devices.push_back(new HmdDeviceOpenHmd());
    }
#endif
    
    if (library == LIB_MOUSE_DUMMY)
    {
        // only use mouse dummy if it is forced
        devices.push_back(new HmdDeviceMouse());
    }

    IHmdDevice* pSelectedDevice = NULL;

    for (unsigned int i=0; i<devices.size(); i++)
    {
        if (pSelectedDevice == NULL)
        {
            bool worked = devices[i]->Init(allowDummyDevice);
            if (worked)
            {
                pSelectedDevice = devices[i];
            }
            else
            {
                devices[i]->Shutdown();
            }
        }

        if (pSelectedDevice != devices[i])
        {
            delete devices[i];
            devices[i] = NULL;
        }
    }

    devices.clear();

    return pSelectedDevice;
}

IHmdRenderer* FactoryHmdDevice::CreateRendererForDevice(IHmdDevice* pDevice)
{
	VID_Printf(PRINT_ALL, "[FactoryHmdDevice] Creating HMD Renderer\n");

    if (pDevice == NULL)
    {
        return NULL;
    }

    int width = 0;
    int height = 0;
    bool isRotated = false;
    bool isExtendedMode = false;

    bool needsRenderer = pDevice->GetDeviceResolution(width, height, isRotated, isExtendedMode);
    if (!needsRenderer)
    {
        return NULL;
    }

#ifdef USE_OPENVR
	OpenVRSDK::HmdDeviceOpenVRSdk* pOpenVRDevice = dynamic_cast<OpenVRSDK::HmdDeviceOpenVRSdk*>(pDevice);
	if (pOpenVRDevice != NULL)
	{
		OpenVRSDK::HmdRendererOpenVRSdk* pRenderer = new OpenVRSDK::HmdRendererOpenVRSdk(pOpenVRDevice);
		return pRenderer;
	}
#endif


#ifdef USE_OVR_1
    OvrSdk_1::HmdDeviceOculusSdk* pOculusSdk_1 = dynamic_cast<OvrSdk_1::HmdDeviceOculusSdk*>(pDevice);
    if (pOculusSdk_1 != NULL)
    {
        //HmdRendererOculus* pRenderer = new HmdRendererOculus();
        OvrSdk_1::HmdRendererOculusSdk* pRenderer = new OvrSdk_1::HmdRendererOculusSdk(pOculusSdk_1);
        return pRenderer;
    }
#endif

#ifdef USE_OVR_0_8
    OvrSdk_0_8::HmdDeviceOculusSdk* pOculusSdk_0_8 = dynamic_cast<OvrSdk_0_8::HmdDeviceOculusSdk*>(pDevice);
    if (pOculusSdk_0_8 != NULL)
    {
        //HmdRendererOculus* pRenderer = new HmdRendererOculus();
        OvrSdk_0_8::HmdRendererOculusSdk* pRenderer = new OvrSdk_0_8::HmdRendererOculusSdk(pOculusSdk_0_8);
        return pRenderer;
    }
#endif

#ifdef USE_OVR_0_5
    OvrSdk_0_5::HmdDeviceOculusSdk* pOculusSdk_0_5 = dynamic_cast<OvrSdk_0_5::HmdDeviceOculusSdk*>(pDevice);
    if (pOculusSdk_0_5 != NULL)
    {
        //HmdRendererOculus* pRenderer = new HmdRendererOculus();
        OvrSdk_0_5::HmdRendererOculusSdk* pRenderer = new OvrSdk_0_5::HmdRendererOculusSdk(pOculusSdk_0_5);
        return pRenderer;
    }
#endif

#ifdef USE_OPENHMD
    HmdDeviceOpenHmd* pOpenHmd = dynamic_cast<HmdDeviceOpenHmd*>(pDevice);
    if (pOpenHmd != NULL)
    {
        //HmdRendererOculus* pRenderer = new HmdRendererOculus();
        HmdRendererOculusOpenHmd* pRenderer = new HmdRendererOculusOpenHmd(pOpenHmd);
        return pRenderer;
    }
#endif
    
    HmdDeviceMouse* pHmdMouse = dynamic_cast<HmdDeviceMouse*>(pDevice);
    if (pHmdMouse != NULL)
    {
        HmdRendererOculus* pRenderer = new HmdRendererOculus();
        return pRenderer;
    }

    return NULL;
}

IHmdInput*FactoryHmdDevice::CreateInputForDevice(IHmdDevice* pDevice)
{
    if (pDevice == nullptr)
    {
        return nullptr;
    }

    bool handlesControllerInput = pDevice->HandlesControllerInput();
    if (!handlesControllerInput)
    {
        return nullptr;
    }

#ifdef USE_OVR_1
    OvrSdk_1::HmdDeviceOculusSdk* pOculusSdk_1 = dynamic_cast<OvrSdk_1::HmdDeviceOculusSdk*>(pDevice);
    if (pOculusSdk_1 != nullptr)
    {
        OvrSdk_1::HmdInputOculusSdk* pInput = new OvrSdk_1::HmdInputOculusSdk(pOculusSdk_1);
        return pInput;
    }
#endif

    return nullptr;
}
