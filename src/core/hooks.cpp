#include "hooks.h"
#include <iostream>

//#include <Windows.h>

// include minhook for epic hookage
#include "../../ext/minhook/minhook.h"

#include <intrin.h>

#include "../hacks/misc.h"
#include "../hacks/aimbot.h"
#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_win32.h"
#include "../../ext/imgui/imgui_impl_dx9.h"

void hooks::Setup() noexcept
{
	MH_Initialize();

	// AllocKeyValuesMemory hook
	MH_CreateHook(
		memory::Get(interfaces::keyValuesSystem, 1),
		&AllocKeyValuesMemory,
		reinterpret_cast<void**>(&AllocKeyValuesMemoryOriginal)
	);

	// CreateMove hook
	MH_CreateHook(
		memory::Get(interfaces::clientMode, 24),
		&CreateMove,
		reinterpret_cast<void**>(&CreateMoveOriginal)
	);

	// PaintTraverse hook
	MH_CreateHook(
		memory::Get(interfaces::panel, 41),
		&PaintTraverse,
		reinterpret_cast<void**>(&PaintTraverseOriginal)
	);

	// imgui crap
	MH_CreateHook(
		memory::Get(gui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	);

	MH_CreateHook(
		memory::Get(gui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	);

	// DrawModel hook
	MH_CreateHook(
		memory::Get(interfaces::studioRender, 29),
		&DrawModel,
		reinterpret_cast<void**>(&DrawModelOriginal)
	);

	MH_EnableHook(MH_ALL_HOOKS);

	gui::DestroyDX();
}

void hooks::Destroy() noexcept
{
	// restore hooks
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	// uninit minhook
	MH_Uninitialize();
}

void* __stdcall hooks::AllocKeyValuesMemory(const std::int32_t size) noexcept
{
	// if function is returning to speficied addresses, return nullptr to "bypass"
	if (const std::uint32_t address = reinterpret_cast<std::uint32_t>(_ReturnAddress());
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesEngine) ||
		address == reinterpret_cast<std::uint32_t>(memory::allocKeyValuesClient)) 
		return nullptr;

	// return original
	return AllocKeyValuesMemoryOriginal(interfaces::keyValuesSystem, size);
}

bool __stdcall hooks::CreateMove(float frameTime, CUserCmd* cmd) noexcept
{
	// make sure this function is being called from CInput::CreateMove
	if (!cmd->commandNumber)
		return CreateMoveOriginal(interfaces::clientMode, frameTime, cmd);

	// this would be done anyway by returning true
	if (CreateMoveOriginal(interfaces::clientMode, frameTime, cmd))
		interfaces::engine->SetViewAngles(cmd->viewAngles);

	// get our local player here
	globals::UpdateLocalPlayer();

	if (globals::localPlayer && globals::localPlayer->IsAlive())
	{
		// example bhop
		hacks::RunBunnyHop(cmd);

		hacks::RunAutoStrafe(cmd);

		if (globals::autostop)
			hacks::AutoStop(cmd);

		hacks::RunAimbot(cmd);
	}

	if (globals::silent && !globals::RCS)
		return false;

	return true;
}

void __stdcall hooks::PaintTraverse(uintptr_t vguiPanel, bool forceRepaint, bool allowForce) noexcept {

	// check if we have the right panel
	if (vguiPanel == interfaces::engineVGui->GetPanel(PANEL_TOOLS)) {
		if (globals::localPlayer && interfaces::engine->IsInGame() && globals::esptoggle) {
			for (int i = 1; i < interfaces::globals->maxClients; i++) {
				CEntity* player = interfaces::entityList->GetEntityFromIndex(i);
				
				if (!player)
					continue;

				if (player->IsDormant() || !player->IsAlive())
					continue;

				if (player->GetTeam() == globals::localPlayer->GetTeam())
					continue;

				if (!globals::localPlayer->IsAlive())
					if (globals::localPlayer->GetObserverTarget() == player)
						continue;

				CMatrix3x4 bones[128];
				if (!player->SetupBones(bones, 128, 0x7FF0, interfaces::globals->currentTime))
					continue;

				CVector top;
				if (interfaces::debugOverlay->ScreenPosition(bones[8].Origin() + CVector{ 0.f, 0.f, 11.f }, top))
					continue;

				CVector bottom;
				if (interfaces::debugOverlay->ScreenPosition(player->GetAbsOrigin() - CVector{ 0.f, 0.f, 9.f }, bottom))
					continue;

				hacks::hitboxPoints(globals::aimbotPoints, player, bones, HITBOX_HEAD, 0.85f);


				const float h = bottom.y - top.y;

				const float w = h * 0.3f;

				const auto left = static_cast<int>(top.x - w);
				const auto right = static_cast<int>(top.x + w);

				interfaces::surface->DrawSetColor(globals::espcolor[0], globals::espcolor[1], globals::espcolor[2], 255);
				
				// esp
				interfaces::surface->DrawOutlinedRect(left, top.y, right, bottom.y);

				// name esp
				static unsigned long hfont = interfaces::surface->CreateFont1();

				player_info_struct playerinfo;
				interfaces::engine->getPlayerInfo(i, playerinfo);

				wchar_t name[128];

				std::mbstowcs(name, playerinfo.name, sizeof(name) / sizeof(wchar_t));

				interfaces::surface->SetFontGlyphSet(hfont, "Arial", 12, 0, 0, 0, FONT_FLAG_ANTIALIAS);
				interfaces::surface->DrawSetTextFont(hfont);
				interfaces::surface->DrawSetTextColor(255, 255, 255, 255);
				interfaces::surface->DrawSetTextPos(left, top.y - 10);
				interfaces::surface->DrawPrintText(name, wcslen(name));

				// reset color
				interfaces::surface->DrawSetColor(0, 0, 0, 255);

				// outlines
				interfaces::surface->DrawOutlinedRect(left - 1, top.y - 1, right + 1, bottom.y + 1);
				interfaces::surface->DrawOutlinedRect(left + 1, top.y + 1, right - 1, bottom.y - 1);

				// health bar outline
				interfaces::surface->DrawOutlinedRect(left - 8, top.y - 1, left -3 , bottom.y + 1);

				// health is 0-100, get percent.
				const float healthFrac = player->GetHealth() * 0.01f;

				// set health bar color to split between red/green
				interfaces::surface->DrawSetColor((1.f - healthFrac) * 255, 255 * healthFrac, 0, 255);

				// drawing health bar
				interfaces::surface->DrawFilledRect(left - 7, bottom.y - (h * healthFrac), left - 4, bottom.y);


			}

			interfaces::surface->DrawSetColor((int)globals::viscolor[0], (int)globals::viscolor[1], (int)globals::viscolor[2], 255);


			interfaces::surface->DrawOutlinedCircle(globals::monitorsizex / 2, globals::monitorsizey / 2, (int)(globals::aimbotFOV*12.5), 100);

		}

		interfaces::surface->DrawSetColor(255, 255, 255, 255);

		for (CVector pos : globals::aimbotPoints) {
			static CVector h;

			interfaces::debugOverlay->ScreenPosition(pos, h);
			interfaces::surface->DrawFilledRect(h.x, h.y, h.x + 2, h.y + 2);
		}

	}

	// original PaintTraverse hook
	PaintTraverseOriginal(interfaces::panel, vguiPanel, forceRepaint, allowForce);
	globals::aimbotPoints.clear();
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnaddr = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	if (_ReturnAddress() == returnaddr)
		return result;

	if (!gui::setup)
		gui::SetupMenu(device);

	if (gui::open)
		gui::Render();
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

void __stdcall hooks::DrawModel(
	void* results,
	const CDrawModelInfo& info,
	CMatrix3x4* bones,
	float* flexWeights,
	float* flexDelayedWeights,
	const CVector& modelOrigin,
	const int32_t flags
) noexcept {
	if (globals::localPlayer&& info.renderable&& globals::esptoggle) {

		CEntity* entity = info.renderable->GetIClientUnknown()->GetBaseEntity();

		if (entity && entity->IsPlayer() && entity->GetTeam() != globals::localPlayer->GetTeam()) {

			// get material to override
			static IMaterial* material = interfaces::materialSystem->FindMaterial("debug/debugambientcube");

			// set alpha
			interfaces::studioRender->SetAlphaModulation(1.f);

			// show through walls 
			material->SetMaterialVarFlag(IMaterial::IGNOREZ, true);
			interfaces::studioRender->SetColorModulation(globals::chamscolorhidden);
			interfaces::studioRender->ForcedMaterialOverride(material);
			DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

			// don't show through walls 
			material->SetMaterialVarFlag(IMaterial::IGNOREZ, false);
			interfaces::studioRender->SetColorModulation(globals::chamscolorvisible);
			interfaces::studioRender->ForcedMaterialOverride(material);
			DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);

			// reset the material override + return to original hook
			return interfaces::studioRender->ForcedMaterialOverride(nullptr);

		}
	}

	// call original drawmodel for things that aren't getting chammed
	DrawModelOriginal(interfaces::studioRender, results, info, bones, flexWeights, flexDelayedWeights, modelOrigin, flags);
}
