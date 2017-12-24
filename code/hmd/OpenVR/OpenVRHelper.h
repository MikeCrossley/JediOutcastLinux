/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRHELPER_H
#define OPENVRHELPER_H

#include <openvr.h>
#include <openvr_capi.h>

namespace OpenVRSDK
{
	static vr::HmdMatrix34_t CreateMatrix34()
	{
		vr::HmdMatrix34_t mMatrix;

		mMatrix.m[0][0] = 1.f;
		mMatrix.m[0][1] = 0.f;
		mMatrix.m[0][2] = 0.f;
		mMatrix.m[0][3] = 0.f;

		mMatrix.m[1][0] = 0.f;
		mMatrix.m[1][1] = 1.f;
		mMatrix.m[1][2] = 0.f;
		mMatrix.m[1][3] = 0.f;

		mMatrix.m[2][0] = 0.f;
		mMatrix.m[2][1] = 0.f;
		mMatrix.m[2][2] = 1.f;
		mMatrix.m[2][3] = 0.f;

		return mMatrix;
	}
};
#endif
