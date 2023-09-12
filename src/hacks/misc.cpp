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

void hacks::RunBunnyHop(CUserCmd* cmd) noexcept {

    if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND) && globals::localPlayer->IsAlive())
        cmd->buttons &= ~CUserCmd::IN_JUMP;
}

void hacks::RunAutoStrafe(CUserCmd* cmd) noexcept {
    if (!globals::localPlayer->IsAlive())
        return;

    if (!(globals::localPlayer->GetFlags() & CEntity::FL_ONGROUND)) {
        if (cmd->mouseDeltaX < 0)
            cmd->sideMove = -450.0f;
        else if (cmd->mouseDeltaX > 0)
            cmd->sideMove = 450.0f;
    }
}

void hacks::AutoStop(CUserCmd* cmd) noexcept {
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

    const auto negatedDirection = CVector::fromAngle(direction).Scale(-speed * 2.f);
    cmd->forwardMove = negatedDirection.x;
    cmd->sideMove = negatedDirection.y;
}

void MatrixCopy(const CMatrix3x4& in, CMatrix3x4& out) {
    std::memcpy(out.Base(), in.Base(), sizeof(CMatrix3x4));
}

void ConcatTransforms(const CMatrix3x4& in1,
    const CMatrix3x4& in2, CMatrix3x4& out) {
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

void CrossProduct(const CVector& v1,
    const CVector& v2, CVector& cross) {
    cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
    cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void VectorTransform(const CVector& in,
    const CMatrix3x4& matrix, CVector& out) {
    out = {
        in.dotProduct(CVector(matrix[0][0], matrix[0][1], matrix[0][2])) + matrix[0][3],
        in.dotProduct(CVector(matrix[1][0], matrix[1][1], matrix[1][2])) + matrix[1][3],
        in.dotProduct(CVector(matrix[2][0], matrix[2][1], matrix[2][2])) + matrix[2][3]
    };
}

void hacks::hitboxPoints(std::vector < CVector >& points, CEntity* player, CMatrix3x4* bones, int iHitbox, float flPointScale) noexcept {

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

    float radius = hitbox->capsuleRadius * flPointScale;

    CVector center = (hitbox->bbMin + hitbox->bbMax).Scale(0.5f);

    const float rotation = 0.70710678f;

    size_t originalSize = points.size();

    if (iHitbox == HITBOX_HEAD) {

        if (globals::hbToggle.head)
            points.push_back(center);

        if (globals::hbMpToggle.head) {
            // top/back 45 deg. Best place to shoot at
            points.push_back({
                hitbox->bbMax.x + (rotation * radius),
                hitbox->bbMax.y + (-rotation * radius),
                hitbox->bbMax.z
                });
            // right
            points.push_back({
                hitbox->bbMax.x,
                hitbox->bbMax.y,
                hitbox->bbMax.z + radius
                });
            // left
            points.push_back({
                hitbox->bbMax.x,
                hitbox->bbMax.y,
                hitbox->bbMax.z - radius
                });
            // back
            points.push_back({
                hitbox->bbMax.x,
                hitbox->bbMax.y - radius,
                hitbox->bbMax.z
                });
        }
    }
    else if (iHitbox == HITBOX_STOMACH) {
        // center.
        if (globals::hbToggle.body)
            points.push_back(center);

        // back.
        if (globals::hbMpToggle.body) {

            points.push_back({
                center.x,
                hitbox->bbMax.y - radius,
                center.z
                });

            points.push_back({
                center.x - hitbox->bbMax.x,
                center.y,
                center.z
                });
        }


    }
    else if (iHitbox == HITBOX_PELVIS || iHitbox == HITBOX_UPPER_CHEST) {
        // back.
        if (globals::hbToggle.body)
            points.push_back({
                center.x,
                hitbox->bbMax.y - radius,
                center.z
                });
    }

    else if (iHitbox == HITBOX_THORAX || iHitbox == HITBOX_CHEST) {
        // add center.
        if (globals::hbToggle.body)
            points.push_back(center);

        // add multipoints
        if (globals::hbMpToggle.body) {

            // point on the back
            points.push_back({
                center.x,
                hitbox->bbMax.y - radius,
                center.z
                });

            // right
            points.push_back({center.x, center.y, hitbox->bbMax.z + radius});
            // left 
            points.push_back({center.x, center.y, hitbox->bbMin.z - radius});
        }
    }
    else if (iHitbox == HITBOX_RIGHT_CALF || iHitbox == HITBOX_LEFT_CALF) {
        // add center.
        if (globals::hbToggle.legs)
            points.push_back(center);

        // half bottom.
        if (globals::hbMpToggle.legs)
            points.push_back({
                hitbox->bbMax.x - (hitbox->capsuleRadius / 2.f),
                hitbox->bbMax.y,
                hitbox->bbMax.z
                });
    }
    else if (iHitbox == HITBOX_RIGHT_THIGH || iHitbox == HITBOX_LEFT_THIGH) {
        // add center.
        if (globals::hbToggle.legs)
            points.push_back(center);
    }

    // arms get only one point.
    else if (iHitbox == HITBOX_RIGHT_UPPER_ARM || iHitbox == HITBOX_LEFT_UPPER_ARM) {
        // elbow.
        if (globals::hbMpToggle.arms)
            points.push_back({
                hitbox->bbMax.x + hitbox->capsuleRadius,
                center.y,
                center.z
                });
    }

    if (points.empty())
        return;

    size_t i;
    for (i = originalSize; i < points.size(); i++) {

        CVector p = points[i];

        VectorTransform(p, bones[hitbox->bone], p);

        points[i] = p;
    }
}