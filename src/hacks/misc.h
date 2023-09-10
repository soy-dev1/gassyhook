#pragma once
#include <vector>

class CUserCmd;
class CEntity;
class CMatrix3x4;
class CVector;
namespace hacks
{
	void RunBunnyHop(CUserCmd* cmd) noexcept;
	void RunAutoStrafe(CUserCmd* cmd) noexcept;
	void AutoStop(CUserCmd* cmd) noexcept;
	void hitboxPoints(std::vector<CVector>& points, CEntity* player, CMatrix3x4* bones, int iHitbox, float flPointScale) noexcept;
}
