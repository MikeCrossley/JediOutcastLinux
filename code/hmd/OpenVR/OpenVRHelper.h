/**
 * HMD extension for JediAcademy
 *
 *  Copyright 2015 by Jochen Leopold <jochen.leopold@model-view.com>
 *	OpenVR Extension by Michael Crossley
 */

#ifndef OPENVRHELPER_H
#define OPENVRHELPER_H

#include <openvr.h>

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

namespace OpenVR
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

	static vr::HmdQuaternion_t GetRotation(vr::HmdMatrix34_t matrix)
	{
		vr::HmdQuaternion_t q;

		q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
		q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
		q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
		return q;
	}

	static void ConvertQuatToEuler(const float* quat, float& rYaw, float& rPitch, float& rRoll)
	{
		//https://svn.code.sf.net/p/irrlicht/code/trunk/include/quaternion.h
		// modified to get yaw before pitch

		float W = quat[3];
		float X = quat[1];
		float Y = quat[0];
		float Z = quat[2];

		float sqw = W*W;
		float sqx = X*X;
		float sqy = Y*Y;
		float sqz = Z*Z;

		float test = 2.0f * (Y*W - X*Z);

		if (test > (1.0f - 0.000001f))
		{
			// heading = rotation about z-axis
			rRoll = (-2.0f*atan2(X, W));
			// bank = rotation about x-axis
			rYaw = 0;
			// attitude = rotation about y-axis
			rPitch = float(M_PI / 2.0);
		}
		else if (test < (-1.0f + 0.000001f))
		{
			// heading = rotation about z-axis
			rRoll = (2.0f*atan2(X, W));
			// bank = rotation about x-axis
			rYaw = 0;
			// attitude = rotation about y-axis
			rPitch = float(M_PI / -2.0);
		}
		else
		{
			// heading = rotation about z-axis
			rRoll = atan2(2.0f * (X*Y + Z*W), (sqx - sqy - sqz + sqw));
			// bank = rotation about x-axis
			rYaw = atan2(2.0f * (Y*Z + X*W), (-sqx - sqy + sqz + sqw));
			// attitude = rotation about y-axis
			test = max(test, -1.0f);
			test = min(test, 1.0f);
			rPitch = asin(test);
		}
	}
};
#endif
