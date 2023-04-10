//#define DEBUG

#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <TlHelp32.h>
#include <dwmapi.h>
#include <Uxtheme.h>
#include "draw.h"
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"// made by android#1337
#include "imgui/imgui_internal.h"
#include "imgui/imgui_custom.h"

#include "utils.h"
#include "comm.h"
#include"vector.h"
#include "ue4.h"



IDirect3D9Ex* d3d9_object;
IDirect3DDevice9Ex* d3d9_device;

bool show_menu = true;

uintptr_t process_id;
uintptr_t virtualaddy;

INT32 find_process(LPCTSTR process_name) {
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, process_name)) {
				CloseHandle(hsnap);
				process_id = pt.th32ProcessID;
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);


	return { NULL };
}

#define check_in_screen(pos) pos.x != 0 && pos.y != 0//pos.x > 0 && pos.x < screen_width && pos.y > 0 && pos.y < screen_height

void draw_line(ImVec2 pos1, ImVec2 pos2, ImColor color, int thickness = 1.f)
{
	if (check_in_screen(pos1) && check_in_screen(pos2))
		ImGui::GetOverlayDrawList()->AddLine(pos1, pos2, color, thickness);
}

void draw_outlined_text(ImVec2 pos, ImColor color, std::string text)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;// made by android#1337

	while (std::getline(stream, line))
	{
		ImVec2 textSize = ImGui::CalcTextSize(line.c_str());

		ImGui::GetOverlayDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

		ImGui::GetOverlayDrawList()->AddText(ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
}

namespace settings
{
	inline bool bHitler;
	inline bool bAimbot;// made by android#1337
	inline float fAimSmooth;
	inline int iFovCircle;
	bool Distance = true;
	inline bool bSkeleton;
	inline bool bSnapline;
}


void move_mouse_at(ImVec2 pos)
{
	float ScreenCenterX = (screen_width / 2);
	float ScreenCenterY = (screen_height / 2);
	float TargetX = 0;
	float TargetY = 0;

	if (pos.x != 0)
	{
		if (pos.x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - pos.x);
			TargetX /= settings::fAimSmooth;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (pos.x < ScreenCenterX)
		{
			TargetX = pos.x - ScreenCenterX;
			TargetX /= settings::fAimSmooth;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (pos.y != 0)
	{
		if (pos.y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - pos.y);
			TargetY /= settings::fAimSmooth;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (pos.y < ScreenCenterY)
		{// made by android#1337
			TargetY = pos.y - ScreenCenterY;
			TargetY /= settings::fAimSmooth;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
}
namespace Screen {


	bool IsInScreen(Vector3 pos, int over = 30) {
		if (pos.x > -over && pos.x < screen_width + over && pos.y > -over && pos.y < screen_height + over) {
			return true;
		}
		else {
			return false;
		}
	}
}

#define FN_VECTOR Vector3
FN_VECTOR LocalActorPos;

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
}; Camera vCamera;

struct FMinimalViewInfo
{
	Vector3 Location;
	Vector3 Rotation;
	float FOV;
	float DesiredFOV;
};



void loop()
{
	Sleep(1);// made by android#1337

	auto world = KmDrv->read<uintptr_t>(virtualaddy + 0xEEDAB90);
	if (!world) return;
	std::cout << "uworld passed" << std::endl;
	auto owning_game_instance = KmDrv->read<uintptr_t>(world + 0x1b8);
	if (!owning_game_instance) return;
	std::cout << "owning_game_instance passed" << std::endl;

	auto local_players = KmDrv->read<uintptr_t>(owning_game_instance + 0x38);
	if (!local_players) return;
	std::cout << "local_players passed" << std::endl;

	auto local_player = KmDrv->read<uintptr_t>(local_players);
	if (!local_player) return;
	std::cout << "local_player passed" << std::endl;
	auto player_controller = KmDrv->read<uintptr_t>(local_player + 0x30);
	if (!player_controller) return;
	std::cout << "player_controller passed" << std::endl;
	auto camera_manager = KmDrv->read<uintptr_t>(player_controller + 0x340);
	if (!camera_manager) return;

	auto acknowledged_pawn = KmDrv->read<uintptr_t>(player_controller + 0x330);
	if (!acknowledged_pawn)
	{
		std::cout << "in lobby" << std::endl;
		auto last_frame_camera_cache_private = camera_manager + 0x2a80;
		auto pov = last_frame_camera_cache_private + 0x10;
		unreal::defines::camera_location = KmDrv->read<unreal::fvector>(pov + 0x0);
		unreal::defines::camera_rotation = KmDrv->read<unreal::frotator>(pov + 0x18);
		unreal::defines::fov_angle = KmDrv->read<float>(pov + 0x30);
	}
	else// made by android#1337
	{
		std::cout << "in game" << std::endl;
		auto root_component = KmDrv->read<uintptr_t>(acknowledged_pawn + 0x190);
		if (!root_component) return;

		unreal::functions::update_camera(world, local_player, root_component);
	}

	auto root_component = KmDrv->read<uintptr_t>(acknowledged_pawn + 0x190);

	LocalActorPos = KmDrv->read<Vector3>(root_component + 0x128);
	std::cout << "camera_manager passed" << std::endl;

	auto current_weapon = KmDrv->read<uintptr_t>(acknowledged_pawn + 0x8f8);
	auto current_vehicle = KmDrv->read<uintptr_t>(acknowledged_pawn + 0x2428);

	std::cout << ("Uworld : ") << world << std::endl;
	std::cout << ("game_instance : ") << owning_game_instance << std::endl;
	//std::cout << ("persistentlevel : ") << pointer::PersistentLevel << std::endl;
	std::cout << ("local_players : ") << local_players << std::endl;
	std::cout << ("local_player : ") << local_player << std::endl;
	//std::cout << ("player_controller : ") << pointer::PlayerController << std::endl;
	std::cout << ("local_pawn : ") << acknowledged_pawn << std::endl;
	//std::cout << ("player_state : ") << pointer::PlayerState << std::endl;
	//std::cout << ("root_comp : ") << rootcomp << std::endl;


	auto game_state = KmDrv->read<uintptr_t>(world + 0x158);
	if (!game_state) return;

	auto player_array = KmDrv->read<unreal::tarray<uintptr_t>>(game_state + 0x2a0);
	if (!player_array.size()) return;

	float target_pawn_dist = FLT_MAX;
	uintptr_t target_pawn = 0;

	for (auto i = 0; i < player_array.size(); i++)
	{
		auto state = player_array[i];
		if (!state) continue;

		auto pawn = KmDrv->read<uintptr_t>(state + 0x300); // PawnPrivate
		if (!pawn || pawn == acknowledged_pawn) continue;

		auto mesh = KmDrv->read<uintptr_t>(pawn + 0x310);
		if (!mesh) continue;

		ImColor col = ImColor(255, 0, 0, 255);

		auto world_head = unreal::functions::get_bone_location(mesh, 68);
		auto head = unreal::functions::world_to_screen(world_head);
		auto neck = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 66));
		auto pelvis = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 2));



		// skeleton
		if (settings::bSkeleton)
		{

			auto chest_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 8));
			auto shoulder_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 38));
			auto elbow_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 39));
			auto wrist_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 62));
			auto ass_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 78));
			auto knee_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 79));
			auto foot_left = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 82));
			// made by android#1337
			auto chest_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 8));
			auto shoulder_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 9));
			auto elbow_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 10));
			auto wrist_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 33));
			auto ass_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 71));
			auto knee_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 72));
			auto foot_right = unreal::functions::world_to_screen(unreal::functions::get_bone_location(mesh, 86));

			// neck->pelvis
			draw_line(neck, pelvis, col);

			// left side
			draw_line(neck, chest_left, col);
			draw_line(chest_left, shoulder_left, col);
			draw_line(shoulder_left, elbow_left, col);
			draw_line(elbow_left, wrist_left, col);
			draw_line(pelvis, ass_left, col);
			draw_line(ass_left, knee_left, col);
			draw_line(knee_left, foot_left, col);

			// right side
			draw_line(neck, chest_right, col);
			draw_line(chest_right, shoulder_right, col);
			draw_line(shoulder_right, elbow_right, col);
			draw_line(elbow_right, wrist_right, col);
			draw_line(pelvis, ass_right, col);
			draw_line(ass_right, knee_right, col);
			draw_line(knee_right, foot_right, col);

		}

		// snapline
		if (settings::bSnapline)
		{
			draw_line({ screen_width / 2, screen_height - 1 }, pelvis, col);
		}

		// aimbot stuff
		{
			auto dx = head.x - (screen_width / 2);
			auto dy = head.y - (screen_height / 2);
			auto dist = sqrtf(dx * dx + dy * dy);

			if (dist < settings::iFovCircle && dist < target_pawn_dist) {
				target_pawn_dist = dist;
				target_pawn = pawn;
			}
		}
	}

	// memory aimbot
	{
		if (settings::bAimbot)
		{
			if (target_pawn && GetAsyncKeyState(VK_RBUTTON))
			{
				auto target_mesh = KmDrv->read<uintptr_t>(target_pawn + 0x310);
				if (!target_mesh) return;

				auto target_head = unreal::functions::world_to_screen(unreal::functions::get_bone_location(target_mesh, 68));

				move_mouse_at(target_head);
			}
		}

		/*// made by android#1337
		static bool AnglesResetted = true;

		if (target_pawn && GetAsyncKeyState(0x02))
		{
			auto target_mesh = KmDrv->read<uintptr_t>(target_pawn + 0x2F0);
			if (!target_mesh) return;

			auto target_head = unreal::functions::get_bone_location(target_mesh, 98);

			auto delta = target_head - unreal::defines::camera_location;

			float distance = (double)(sqrtf(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z));

			unreal::frotator rot;
			rot.Pitch = -((acosf(delta.Z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
			rot.Yaw = atan2f(delta.Y, delta.X) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
// made by android#1337
			memory::write<float>(camera_manager + 0x3174, rot.Pitch); //ViewPitchMin
			memory::write<float>(camera_manager + 0x3178, rot.Pitch); //ViewPitchMax
			memory::write<float>(camera_manager + 0x317c, rot.Yaw); //ViewYawMin
			memory::write<float>(camera_manager + 0x3180, rot.Yaw); //ViewYawMax

			AnglesResetted = false;
		}
		else if (!AnglesResetted)
		{
			memory::write<float>(camera_manager + 0x3174, -89.9f); //ViewPitchMin
			memory::write<float>(camera_manager + 0x3178, 89.9f); //ViewPitchMax
			memory::write<float>(camera_manager + 0x317c, 0.f); //ViewYawMin
			memory::write<float>(camera_manager + 0x3180, 359.999f); //ViewYawMax

			AnglesResetted = true;
		}
		*/
	}
}

int rotation_degrees;

void koordinateswitch()
{
	while (true)
	{
		if (!settings::bHitler) continue;

		rotation_degrees++;

		if (rotation_degrees > 89)
			rotation_degrees = 0;

		Sleep(5);
	}
}
// made by android#1337
void main()
{
	std::thread(koordinateswitch).detach();
	utils::log("memory::initialize");

	system("pause");

	process_id = find_process(L"FortniteClient-Win64-Shipping.exe");
	KmDrv = new Memory(process_id);
	virtualaddy = KmDrv->GetModuleBase("FortniteClient-Win64-Shipping.exe");

	// made by android#1337
	utils::log("memory::base_address ->  %llx", virtualaddy);

	if (!find_process(L"mspaint.exe"))
		system("start mspaint.exe");

	Sleep(1000);

	auto hWnd = utils::get_gdi_window(find_process(L"mspaint.exe"));
	if (!hWnd) return;

	const MARGINS tmp;
	ShowWindow(hWnd, SW_SHOW);
	DwmExtendFrameIntoClientArea(hWnd, &tmp);
	SetWindowLongW(hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9_object);

	D3DPRESENT_PARAMETERS pParamsInfo = {
		screen_width, screen_height, D3DFMT_A8R8G8B8, 0,
		D3DMULTISAMPLE_TYPE(0), D3DMULTISAMPLE_NONE,
		D3DSWAPEFFECT_DISCARD, hWnd, TRUE, TRUE, D3DFMT_D16, 0,
		0, D3DPRESENT_INTERVAL_IMMEDIATE
	};

	d3d9_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pParamsInfo, 0, &d3d9_device),

		ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(d3d9_device);
	d3d9_object->Release();

	//ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 16);

	pParamsInfo.BackBufferWidth = screen_width;
	pParamsInfo.BackBufferHeight = screen_height;
	SetWindowPos(hWnd, (HWND)0, 0, 0, screen_width, screen_height, SWP_NOREDRAW);
	d3d9_device->Reset(&pParamsInfo);

	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 14);

	while (true)
	{
		if (GetAsyncKeyState(VK_INSERT) & 1)
			show_menu = !show_menu;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (show_menu)
		{
			ImGuiIO& io = ImGui::GetIO();

			POINT point;
			GetCursorPos(&point);
			io.MousePos.x = point.x;
			io.MousePos.y = point.y;

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].y = io.MousePos.y;
			}
			else
				io.MouseDown[0] = false;


			ImGui::SetNextWindowSize(ImVec2(700, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Windows 10", &show_menu, ImGuiWindowFlags_NoResize);
			{
				ImGui::Checkbox("Aim", &settings::bAimbot);
				ImGui::SliderFloat("f41mSm00th", &settings::fAimSmooth, 0, 10);
				ImGui::SliderInt("i", &settings::iFovCircle, 10, 1337);
				ImGui::Checkbox("Skeleton", &settings::bSkeleton);
				ImGui::Checkbox("bSn4pl1n3", &settings::bSnapline);
				ImGui::Text("m4d3 w1th <3 by EpicGames");
				ImGui::Text("h4ndl3 cr3at3d ->  \\Device\\vgk_PLZNOHACK");
			}

			ImGui::End();

			ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y), 2.5f, ImColor(255, 255, 255, 255));
		}

		loop(); // made by android#1337

		ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(screen_width / 2, screen_height / 2), settings::iFovCircle, ImColor(0, 0, 0, 100));
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(screen_width / 2, screen_height / 2), settings::iFovCircle, ImColor(255, 255, 255, 255));

		if (settings::bHitler)
		{
			int center_x = screen_width / 2;
			int center_y = screen_height / 2;

			int a = (int)(screen_height / 2 / 30);
			float gamma = atan(1);

			for (int i = 0; i < 4; i++)
			{
				int Ax = a * sin((3.14159265358979323846264338327950288419716939937510 / 180) * (rotation_degrees + (i * 90)));
				int Ay = a * cos((3.14159265358979323846264338327950288419716939937510 / 180) * (rotation_degrees + (i * 90)));

				int Bx = (a / cos(gamma)) * sin((3.14159265358979323846264338327950288419716939937510 / 180) * (rotation_degrees + (i * 90) + (180 / 3.14159265358979323846264338327950288419716939937510) * gamma));
				int By = (a / cos(gamma)) * cos((3.14159265358979323846264338327950288419716939937510 / 180) * (rotation_degrees + (i * 90) + (180 / 3.14159265358979323846264338327950288419716939937510) * gamma));

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(center_x, center_y), ImVec2(center_x + Ax, center_y - Ay), ImColor(255, 0, 0, 255));
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(center_x + Ax, center_y - Ay), ImVec2(center_x + Bx, center_y - By), ImColor(255, 0, 0, 255));
			}
		}

		ImGui::EndFrame();


		// clear window
		d3d9_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		// initialize scene
		if (d3d9_device->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			d3d9_device->EndScene();
		}

		// call original
		d3d9_device->Present(NULL, NULL, NULL, NULL);
	}

	// shutdown imgui frame
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (d3d9_device)
		d3d9_device->Release();

	if (d3d9_object)
		d3d9_object->Release();

	DestroyWindow(hWnd);
}