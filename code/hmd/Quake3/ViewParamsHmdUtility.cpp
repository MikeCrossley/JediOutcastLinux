#include "ViewParamsHmdUtility.h"

#include "../ClientHmd.h"
#include "../HmdRenderer/IHmdRenderer.h"

#include "../../client/client.h"

void ViewParamsHmdUtility::UpdateRenderParams(trGlobals_t* trRef, bool isSkyBoxPortal, bool &rViewMatrixCreated)
{
    static float mViewYaw = 0.0f;
    rViewMatrixCreated = false;

    IHmdRenderer* pHmdRenderer = ClientHmd::Get()->GetRenderer();
    if (pHmdRenderer)
    {
        // handle keyhole yaw for decoupled aiming, if the signed difference between the view angle 
        // and the body angle exceeds the keyhole width, move the view angle in that direction
        if (hmd_decoupleAim->integer && !ClientHmd::Get()->HasHand(true))
        {
            float keyholeWidth = hmd_moveAimKeyholeWidth->value / 2.0f;
            float angleDiffRad = DEG2RAD(trRef->viewParms.bodyYaw - mViewYaw);
            float angleDiff = RAD2DEG(atan2(sin(angleDiffRad), cos(angleDiffRad)));
            if (angleDiff > keyholeWidth)
            {
                mViewYaw += trRef->viewParms.bodyYaw - (mViewYaw + keyholeWidth);
            }
            else if (angleDiff < -keyholeWidth)
            {
                mViewYaw += trRef->viewParms.bodyYaw - (mViewYaw - keyholeWidth);
            }
        }
        else
        {
            mViewYaw = trRef->viewParms.bodyYaw;
        }

		vec3_t origin;
		VectorCopy(trRef->viewParms.orient.origin, origin);

        // check if the renderer handles the view matrix creation
        bool matrixCreated = pHmdRenderer->GetCustomViewMatrix(trRef->orient.modelMatrix,
				origin,
                mViewYaw, 
				isSkyBoxPortal);

		// easier just to retrieve it from here than recompute in game code
		ClientHmd::Get()->SetHMDWorldPosition(origin[0], origin[1], origin[2]);

        if (matrixCreated)
        {
			VectorCopy(origin, trRef->viewParms.orient.origin);
            VectorCopy(origin, trRef->viewParms.orient.viewOrigin);
            VectorCopy(origin, trRef->viewParms.pvsOrigin);
        }

        rViewMatrixCreated = matrixCreated;
    }
}
