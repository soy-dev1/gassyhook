#include "gui.h"
#include "../core/globals.h"

#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_win32.h"
#include "../../ext/imgui/imgui_impl_dx9.h"

#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideparam,
	LPARAM longparam
);

LRESULT CALLBACK WindowProc(
	HWND window,
	UINT message,
	WPARAM wideparam,
	LPARAM longparam
);

void style() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // This makes the window background slightly transparent
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.24f, 0.24f, 0.24f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.41f, 0.06f, 0.62f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.13f, 0.84f, 0.15f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.12f, 0.80f, 0.06f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.12f, 0.50f, 0.04f, 0.62f); // Fun fact this doesn't work because imgui is being a fucking shit
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.12f, 0.50f, 0.04f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.24f, 0.81f, 0.19f, 0.40f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.05f, 0.95f, 0.04f, 0.40f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.11f, 0.50f, 0.04f, 0.62f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.32f, 0.81f, 0.19f, 0.62f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.75f, 0.15f, 0.62f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_Header]                 = ImVec4(0.05f, 0.41f, 0.06f, 0.62f);

    ImGui::GetStyle().ScrollbarSize = 8;
    ImGui::GetStyle().ScrollbarRounding = 0;
    ImGui::GetStyle().WindowRounding = 6;
    ImGui::GetStyle().ChildRounding = 3;
    ImGui::GetStyle().FrameRounding = 4;
    ImGui::GetStyle().TabRounding = 2;
}

bool gui::SetupWindowClass(const char* windowClassName) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW || CS_VREDRAW; // Redraws windows if the size or position is adjusted.
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass)) {
		return false;
	}
	return true;
}

void gui::DestroyWindowClass() noexcept {
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept {
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0, 
		0,
		100,
		100,
		0,
		0,
		windowClass.hInstance,
		0
	);

	if (!window)
		return false;

	return true;
}

void gui::DestroyWindow() noexcept {
	if (window)
		DestroyWindow(window);
}

bool gui::SetupDX() noexcept {
	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
		return false;

	using createFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<createFn>(GetProcAddress(handle,"Direct3DCreate9"));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = {};

	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.MultiSampleQuality = NULL;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = NULL;
	params.PresentationInterval = NULL;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0)
		return false;

	return true;

}

void gui::DestroyDX() noexcept {
	if (device) {
		device->Release();
		device = NULL;
	}

	if (d3d9) {
		d3d9->Release();
		d3d9 = NULL;
	}
}

void gui::Setup() noexcept {
	if (!SetupWindowClass("hack"))
		throw std::runtime_error("failed to create window class.");

	if (!SetupWindow("gassyhook"))
		throw std::runtime_error("failed to setup window.");

	if (!SetupDX())
		throw std::runtime_error("failed to create device.");

	DestroyWindow();
	DestroyWindowClass();
}

void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept {
	auto params = D3DDEVICE_CREATION_PARAMETERS{};

	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProc = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc))
		);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	style();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void gui::Destroy() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWindowProc));

	DestroyDX();
}

void gui::Render() noexcept {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse 
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin("GassyHook (P don't steal)", &open, windowFlags)) {

		if (ImGui::BeginTabBar("Tabs")) {

			if (ImGui::BeginTabItem("Aimbot")) {

				ImGui::Checkbox("Enable Aimbot", &globals::aimbot);
				ImGui::Checkbox("Shoot Unscoped", &globals::shootUnscoped);
				ImGui::Checkbox("Enable Autoshoot", &globals::autoshoot);
				ImGui::Checkbox("Silent", &globals::silent);
				ImGui::Checkbox("RCS (enable if aimbot is disabled)", &globals::RCS);
				ImGui::Checkbox("autostop", &globals::autostop);
				ImGui::Checkbox("Accuracy Toggle (silent only)", &globals::minacctoggle);
				ImGui::Checkbox("Head Only", &globals::headOnly);

				ImGui::SliderFloat("Aimbot Fov", &globals::aimbotFOV, 0.f, 360.f);
				ImGui::SliderFloat("fraction (pseudo wallbang checker, turn down to shoot through walls)", &globals::frac, 0.f, 1.f);
				ImGui::SliderFloat("smoothing", &globals::smoothing, 0, 1);
				ImGui::SliderFloat("Minimum Accuracy", &globals::minaccuracy, 0.f, 500.f);
				ImGui::SliderFloat("Aimpoint Tolerance", &globals::pointScale, 0.f, 0.85f);

				if (ImGui::BeginChild(1)) {

					ImGui::Columns(2);

					ImGui::Text("Hitbox Toggle");
					ImGui::Checkbox("Head", &globals::hbToggle.head);
					ImGui::Checkbox("Body", &globals::hbToggle.body); 
					ImGui::Checkbox("Legs", &globals::hbToggle.legs);
					ImGui::Checkbox("Arms", &globals::hbToggle.arms);

					ImGui::NextColumn();

					ImGui::Text("Multipoint Toggle");
					ImGui::Checkbox("MP Head", &globals::hbMpToggle.head); // i am adding the MP identifier in the names here because imgui assigns the checkboxes the same ID if I don't. might fix later lol.
					ImGui::Checkbox("MP Body", &globals::hbMpToggle.body);
					ImGui::Checkbox("MP Legs", &globals::hbMpToggle.legs);
					ImGui::Checkbox("MP Arms", &globals::hbMpToggle.arms);

					ImGui::EndChild();
				}

				ImGui::EndTabItem(); // End "Aimbot" tab
			}

			if (ImGui::BeginTabItem("Visuals")) {

				ImGui::Columns(3);

				ImGui::PushItemWidth(150.f);
				ImGui::ColorPicker3("Chams Hidden", globals::chamscolorhidden);
				ImGui::ColorPicker3("Chams Visible", globals::chamscolorvisible);

				ImGui::NextColumn();

				ImGui::InputInt("monitor x", &globals::monitorsizex);
				ImGui::InputInt("monitor y", &globals::monitorsizey);

				ImGui::PushItemWidth(150.f);
				ImGui::ColorPicker3("Visuals color", globals::viscolor);
				ImGui::ColorPicker3("ESP color", globals::espcolor);

				ImGui::NextColumn();

				ImGui::Checkbox("ESP global", &globals::esptoggle);

				ImGui::EndTabItem(); // End "Visuals" tab
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

LRESULT CALLBACK WindowProc(
	HWND window,
	UINT message,
	WPARAM wideparam,
	LPARAM longparam
) {

	if (GetAsyncKeyState(VK_RSHIFT) & 1)
		gui::open = !gui::open;

	if (gui::open && ImGui_ImplWin32_WndProcHandler (window, message, wideparam, longparam)) {
		return 1L;
	}

	return CallWindowProc(
		gui::originalWindowProc,
		window,
		message,
		wideparam,
		longparam
	);
}