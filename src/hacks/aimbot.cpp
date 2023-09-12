#include "aimbot.h"
#include "misc.h"
#include "../core/globals.h"
#include "../core/interfaces.h"
#include <iostream>
#include <Windows.h>

bool visible(CVector eyepos, CVector endpos) noexcept {

	CTrace trace;
	interfaces::engineTrace->TraceRay(
		CRay{ eyepos, endpos },
		MASK_SHOT,
		{ globals::localPlayer },
		trace
	);

	if (trace.fraction > globals::frac)
		return true;

	return false;
}

int calculateDmg() {

}

void hacks::RunAimbot(CUserCmd* cmd) noexcept
{
	if (globals::RCS) {

		CEntity* activeWeapon = globals::localPlayer->GetActiveWeapon();

		if (!activeWeapon)
			return;

		const int weaponType = activeWeapon->GetWeaponType();

		// aimpunch (recoil control)
		CVector aimPunch{};
		static CVector oldAimPunch{};

		switch (weaponType)
		{
		case CEntity::WEAPONTYPE_RIFLE:
		case CEntity::WEAPONTYPE_PISTOL:
		case CEntity::WEAPONTYPE_MACHINEGUN:
		case CEntity::WEAPONTYPE_SUBMACHINEGUN:
		case CEntity::WEAPONTYPE_SNIPER:
			globals::localPlayer->GetAimPunch(aimPunch);
		}

		cmd->viewAngles = (cmd->viewAngles + oldAimPunch) - aimPunch;

		oldAimPunch = aimPunch;
		return;
	}

	if (!globals::aimbot)
		return;

	if (!globals::localPlayer->IsAlive())
		return;

	if (globals::autoshoot)
		goto BRUH;

	if (!GetAsyncKeyState(VK_XBUTTON1))
		return;

	BRUH:

	if (globals::localPlayer->IsDefusing())
		return;


	CEntity* activeWeapon = globals::localPlayer->GetActiveWeapon();

	if (!activeWeapon)
		return;

	const int weaponType = activeWeapon->GetWeaponType();

	switch (weaponType) {
	case CEntity::WEAPONTYPE_MACHINEGUN:
	case CEntity::WEAPONTYPE_SUBMACHINEGUN:
	case CEntity::WEAPONTYPE_SNIPER:
	case CEntity::WEAPONTYPE_SHOTGUN:
	case CEntity::WEAPONTYPE_PISTOL:
	case CEntity::WEAPONTYPE_RIFLE:
	{
		if (!activeWeapon->GetClip())
			return;

		if (weaponType == CEntity::WEAPONTYPE_SNIPER) {
			if (!globals::shootUnscoped) {
				if (!globals::localPlayer->IsScoped()) {
					return;
				}
			}
		}

		float inaccuracy = 1.f / activeWeapon->GetInaccuracy();

		if (inaccuracy < globals::minaccuracy && globals::silent) // max accuracy is 500
			if (globals::minacctoggle)
				return;

		break;
	}

	default:
		break;
	}


	CVector bestAngle{};
	float bestFOV{ globals::aimbotFOV };


	for (int i = 1; i < interfaces::globals->maxClients; ++i) {
		CEntity* player = interfaces::entityList->GetEntityFromIndex(i);

		if (!player)
			continue;

		if (player == globals::localPlayer)
			continue;

		if (player->IsDormant() || !player->IsAlive())
			continue;

		if (player->GetTeam() == globals::localPlayer->GetTeam())
			continue;

		if (player->HasGunGameImmunity())
			continue;

		CMatrix3x4 bones[128];
		if (!player->SetupBones(bones, 128, 256, interfaces::globals->currentTime))
			continue;

		for (int hb = HITBOX_HEAD; hb != HITBOX_MAX; hb++) {
			hacks::hitboxPoints(globals::aimbotPoints, player, bones, hb, globals::pointScale);
		}

		// eye pos
		CVector localEyePos;
		globals::localPlayer->GetEyePosition(localEyePos);

		// aimpunch (recoil control)
		CVector aimPunch{};

		switch (weaponType)
		{
		case CEntity::WEAPONTYPE_RIFLE:
		case CEntity::WEAPONTYPE_PISTOL:
		case CEntity::WEAPONTYPE_MACHINEGUN:
		case CEntity::WEAPONTYPE_SUBMACHINEGUN:
		case CEntity::WEAPONTYPE_SNIPER:
			globals::localPlayer->GetAimPunch(aimPunch);
		}

		CVector enemyAngle{ 0.f, 0.f, 0.f };

		for (const auto& point : globals::aimbotPoints) {
		
		  // Check if point is visible
		  if (visible(localEyePos, point)) {
		
		    // Calculate angle to enemy
		    CVector angleToEnemy = (point - localEyePos).ToAngle() - (cmd->viewAngles + aimPunch);

		    // Calculate FOV
		    float fov = std::hypot(angleToEnemy.x, angleToEnemy.y);

		    // Update best FOV and angle if current FOV is lower
		    if (fov < bestFOV) {
		      bestFOV = fov;
		      bestAngle = angleToEnemy; 
		    }

		  }

		}
	}

	if ((bestAngle.x != 0) || (bestAngle.y != 0)) {

		if (globals::silent) {
			cmd->buttons |= CUserCmd::IN_ATTACK;
			cmd->viewAngles = cmd->viewAngles + bestAngle;
		}
		else {
			cmd->viewAngles = cmd->viewAngles + bestAngle.Scale(globals::smoothing);
		}
	}

	globals::aimbotPoints.clear();
}

