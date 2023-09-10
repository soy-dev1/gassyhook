#include "misc.h"
#include <iostream>
#include <math.h>

// access global variables
#include "../core/globals.h"

// access interfaces
#include "../valve/centity.h"
#include "../core/interfaces.h"
#include "../valve/ienginetrace.h"
#include "../valve/istudiorender.h"
#include <vector>

void hacks::RunBunnyHop(CUserCmd* cmd) noexcept
{

	if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND) && globals::localPlayer->IsAlive())
		cmd->buttons &= ~CUserCmd::IN_JUMP;
}

void hacks::RunAutoStrafe(CUserCmd* cmd) noexcept
{
	if (!globals::localPlayer->IsAlive())
		return;

	if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND))
	{
		if (cmd->mouseDeltaX < 0)
			cmd->sideMove = -450.0f;
		else if (cmd->mouseDeltaX > 0)
			cmd->sideMove = 450.0f;
	}
}

void hacks::AutoStop(CUserCmd* cmd) noexcept
{
    if (!globals::autostop)
        return;

    if (!globals::localPlayer || !globals::localPlayer->IsAlive())
        return;

    if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND) || cmd->buttons & CUserCmd::IN_JUMP)
        return;

    if (cmd->buttons & (CUserCmd::IN_MOVELEFT | CUserCmd::IN_MOVERIGHT | CUserCmd::IN_FORWARD | CUserCmd::IN_BACK))
        return;

    const auto velocity = globals::localPlayer->Velocity();

    const auto speed = velocity.length2D();
    if (speed < 15.0f)
        return;

    CVector direction = velocity.ToAngle();
    direction.y = cmd->viewAngles.y - direction.y;

    const auto negatedDirection = CVector::fromAngle(direction).Scale(-speed);
    cmd->forwardMove = negatedDirection.x;
    cmd->sideMove = negatedDirection.y;
}

void MatrixCopy(const CMatrix3x4& in, CMatrix3x4& out) {
	std::memcpy(out.Base(), in.Base(), sizeof(CMatrix3x4));
}

void ConcatTransforms(const CMatrix3x4& in1, const CMatrix3x4& in2, CMatrix3x4& out) {
	if (&in1 == &out) {
		CMatrix3x4 in1b;
		MatrixCopy(in1, in1b);
		ConcatTransforms(in1b, in2, out);
		return;
	}

	if (&in2 == &out) {
		CMatrix3x4 in2b;
		MatrixCopy(in2, in2b);
		ConcatTransforms(in1, in2b, out);
		return;
	}

	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

void VectorTransform(const CVector& in, const CMatrix3x4& matrix, CVector& out) {
	out = {
		in.dotProduct(CVector(matrix[0][0], matrix[0][1], matrix[0][2])) + matrix[0][3],
		in.dotProduct(CVector(matrix[1][0], matrix[1][1], matrix[1][2])) + matrix[1][3],
		in.dotProduct(CVector(matrix[2][0], matrix[2][1], matrix[2][2])) + matrix[2][3]
	};
}

void hacks::hitboxPoints(std::vector<CVector>& points, CEntity* player, CMatrix3x4* bones, int iHitbox, float flPointScale) noexcept {

	const CModel* model = player->GetModel();
	if (!model)
		return;

	CStudioHdr* hdr = interfaces::modelInfo->GetStudioModel(model);
	if (!hdr)
		return;

	CStudioHitboxSet* set = hdr->GetHitboxSet(0);
	if (!set)
		return;

	const CStudioBoundingBox* hitbox = set->GetHitbox(iHitbox);
	if (!hitbox)
		return;

	if (iHitbox == HITBOX_HEAD) {

		float radius = hitbox->capsuleRadius * flPointScale;

		CVector center = (hitbox->bbMin + hitbox->bbMax).Scale(0.5f);

		const float rotation = 0.70710678f;

		points.push_back(center);

		// top/back 45 deg. Best place to shoot at
		points.push_back({ hitbox->bbMax.x + (rotation * radius), hitbox->bbMax.y + (-rotation * radius), hitbox->bbMax.z });

		// right
		points.push_back({ hitbox->bbMax.x, hitbox->bbMax.y, hitbox->bbMax.z + radius });
		// left
		points.push_back({ hitbox->bbMax.x, hitbox->bbMax.y, hitbox->bbMax.z - radius });
		// back
		points.push_back({ hitbox->bbMax.x, hitbox->bbMax.y - radius, hitbox->bbMax.z });


	}

	if (points.empty())
		return;

	std::vector<CVector> newPoints;
	for (size_t i = points.size() - 5; i < points.size(); ++i) {
		CVector p = points[i];
		VectorTransform(p, bones[hitbox->bone], p);
		newPoints.push_back(p);
	}

	// Replace the last five points in the original vector with the transformed points
	for (size_t i = points.size() - 5, j = 0; i < points.size(); ++i, ++j) {
		points[i] = newPoints[j];
	}
}
