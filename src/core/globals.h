#pragma once
#include <vector>

class CVector;
class CEntity;
namespace globals
{
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
	extern std::vector<CVector> aimbotPoints;

	// update the local player pointer
	void UpdateLocalPlayer() noexcept;
}
