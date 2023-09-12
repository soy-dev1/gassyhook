#pragma once
#include <vector>
#include "../valve/cvector.h"
class CEntity;

namespace globals
{
	struct aimpoint {
		CVector pos;
		int hbtype;
	};

	struct hbSelstruct {
		bool head;
		bool body;
		bool arms;
		bool legs;
	};


	inline CEntity* localPlayer = nullptr;
	extern bool aimbot;
	extern bool silent;
	extern bool RCS;
	extern bool minacctoggle;
	extern bool esptoggle;
	extern float minaccuracy;
	extern float aimbotFOV;
	extern float viscolor[3]; // general visuals color. Stuff like fov circle
	extern float espcolor[3];
	extern float chamscolorhidden[3];
	extern float chamscolorvisible[3];
	extern int monitorsizex;
	extern int monitorsizey;
	extern float frac;
	extern bool autoshoot;
	extern float smoothing;
	extern bool autostop;
	extern float pointScale;
	extern bool shootUnscoped;
	extern bool headOnly;
	extern hbSelstruct hbMpToggle;
	extern hbSelstruct hbToggle;
	extern std::vector<aimpoint> aimbotPoints;

	// update the local player pointer
	void UpdateLocalPlayer() noexcept;
}
