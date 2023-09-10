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

bool gui::SetupWindowClass(const char* windowClassName) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW || CS_VREDRAW;
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
				ImGui::Checkbox("Enable Autoshoot", &globals::autoshoot);
				ImGui::Checkbox("Silent", &globals::silent);
				ImGui::Checkbox("RCS (enable if aimbot is disabled)", &globals::RCS);
				ImGui::Checkbox("autostop", &globals::autostop);
				ImGui::Checkbox("Accuracy Toggle (silent only)", &globals::minacctoggle);
				ImGui::SliderFloat("Aimbot Fov", &globals::aimbotFOV, 0.f, 360.f);
				ImGui::SliderFloat("fraction (pseudo wallbang checker, turn down to shoot through walls)", &globals::frac, 0.f, 1.f);
				ImGui::SliderFloat("smoothing", &globals::smoothing, 0, 1);
				ImGui::SliderFloat("Minimum Accuracy", &globals::minaccuracy, 0.f, 500.f);
				ImGui::SliderFloat("Aimpoint Tolerance", &globals::pointScale, 0.f, 0.85f);

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