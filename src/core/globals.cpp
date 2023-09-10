#include "globals.h"

// expose interfaces to this file
#include "interfaces.h"

bool globals::aimbot = true;
bool globals::silent = true;
bool globals::RCS;
bool globals::minacctoggle = true;
float globals::minaccuracy = 125;
float globals::aimbotFOV = 3.f;
bool globals::esptoggle = true;
//extern int aimpos; <-- to be added
float globals::viscolor[3] = { 255, 255, 255 };
float globals::espcolor[3] = { 255.f, 255.f, 255.f };
float globals::chamscolorhidden[3] = { 0.f, 0.f, 1.f };
float globals::chamscolorvisible[3] = { 1.f, 0.5f, 0.f };
float globals::frac = 0.9f;
int globals::monitorsizex = 1920;
int globals::monitorsizey = 1080;
bool globals::autoshoot = false;
float globals::smoothing = 0.1f;
bool globals::autostop = true;
float globals::pointScale = 0.75f;
std::vector<CVector> globals::aimbotPoints{};

void globals::UpdateLocalPlayer() noexcept
{
	// get local player index from engine
	const std::int32_t localPlayerIndex = interfaces::engine->GetLocalPlayerIndex();

	// get local player entity from client entity list
	localPlayer = interfaces::entityList->GetEntityFromIndex(localPlayerIndex);
}
