#include "cache.cpp"
#include <D3D11.h>
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"
#include <D3DX11core.h>
#include <D3DX11.h>
#include <D3DX11tex.h>
#include "ImGui.hpp"
#include "font.hpp"
#include "fortnitefont.h"
#include "lightfont.h"
#include "particles.hpp"
#include "menucol.h"
#include "Config.h"
#include "driver.h"
#include "crypter.h"
#include "OverlayHelper.h"
#include "lazyyyy.hpp"
#include "fontawesome.h"

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
HWND window_handle;
HWND windows = NULL;
HWND hwnd = NULL;
RECT GameRect = { NULL };

IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD ScreenCenterX;
DWORD ScreenCenterY;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;

void SetWindowToTarget()
{
	while (true)
	{
		if (windows)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(windows, &GameRect);
			screen_width = GameRect.right - GameRect.left;
			screen_height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(windows, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				screen_height -= 39;
			}
			ScreenCenterX = screen_width / 2;
			ScreenCenterY = screen_height / 2;
			MoveWindow(window_handle, GameRect.left, GameRect.top, screen_width, screen_height, true);
		}
		else
		{
			exit(0);
		}
	}
}

namespace n_render {
	class c_render {
	public:
		//static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		auto imgui() -> bool {
			DXGI_SWAP_CHAIN_DESC swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
			swap_chain_description.BufferCount = 2;
			swap_chain_description.BufferDesc.Width = 0;
			swap_chain_description.BufferDesc.Height = 0;
			swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.OutputWindow = window_handle;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Windowed = 1;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL d3d_feature_lvl;

			const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

			ID3D11Texture2D* pBackBuffer;
			D3DX11_IMAGE_LOAD_INFO info;
			ID3DX11ThreadPump* pump{ nullptr };

			d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

			d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

			pBackBuffer->Release();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
			ImFontConfig icons_config;

			ImFontConfig CustomFont;
			CustomFont.FontDataOwnedByAtlas = false;

			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.OversampleH = 3;
			icons_config.OversampleV = 3;

			ImGui_ImplWin32_Init(window_handle);
			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			ImFontConfig font_config;
			font_config.OversampleH = 1;
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;

			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF,
				0x0400, 0x044F,
				0,
			};

			MenuFont = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Verdana.ttf"), 18.f);
			GameFont = io.Fonts->AddFontFromMemoryTTF(font, sizeof(font), 15.f);
			MenuFont2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 13.f); //11 before
			MenuFont3 = io.Fonts->AddFontFromMemoryTTF(&LightFont, sizeof LightFont, 15.f);
			io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 20.1f, &icons_config, icons_ranges);
			io.Fonts->AddFontDefault();
			FortniteFont = io.Fonts->AddFontFromMemoryTTF(FnFont, sizeof(FnFont), 17.f);

			ImGui_ImplWin32_Init(window_handle);

			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			d3d_device->Release();

			return true;
		}

		auto hijack() -> bool {
			//window_handle = FindWindowA(verucryptt("GDI+ Hook Window Class"), verucryptt("GDI+ Window (Greenshot.exe)")); //( not work)
			window_handle = FindWindowA(verucryptt("MedalOverlayClass"), verucryptt("MedalOverlay"));
			//window_handle = FindWindowA(verucryptt("GDI+ Hook Window Class"), verucryptt("GDI+ Window (obs64.exe)")); //streamlabs & obs
			MARGINS Margin = { -1 };
			DwmExtendFrameIntoClientArea(window_handle, &Margin);
			SetWindowPos(window_handle, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
			ShowWindow(window_handle, SW_SHOW);
			UpdateWindow(window_handle);
			return true;
		}

		void AlignForWidth(float width, float alignment = 0.5f)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - width) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}

		auto menu() -> void {
			//if (globals::g_render_menu)
			//{
			//    static float hue = 0.0f; // initialize hue to 0

			//    ImVec4 rainbow_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			//    rainbow_color = ImColor::HSV(hue, 1.0f, 1.0f); // set the color based on the hue

			//    hue += 0.001f;
			//    if (hue > 1.0f)
			//        hue -= 1.0f;

			//    ImU32 lineColor_u32;

			//    lineColor_u32 = ImGui::ColorConvertFloat4ToU32(rainbow_color); // convert it to U32 so we can use it

			//    ImVec4* Colors;
			//    ImVec4* colors = ImGui::GetStyle().Colors;
			//    colors[ImGuiCol_WindowBg] = ImVec4(ImColor(0, 0, 0, 150));
			//    float widhrr = screen_width;
			//    float hidhrr = screen_width;
			//    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			//    ImGui::SetNextWindowSize({ widhrr, hidhrr });
			//    ImGui::Begin("Static Window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
			//    Particles();
			//    // Add your window content here
			//    draw_circles_and_lines(lineColor_u32);
			//    ImGui::End();
			//}

			struct tab
			{
				const char* tab_name;
				int tab_id;
			};

			static int tabs = 0;
			static int settingtab = 0;
			static int current_tab = 0;

			if (GetAsyncKeyState(VK_INSERT) & 1)
				globals::g_render_menu = !globals::g_render_menu;

			if (globals::g_render_menu) {

				//ImGui::PushFont(MenuFont2);
				//ImGui::PushFont(FortniteFont);
				ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y), ImGuiCond_Once);
				ImGui::SetNextWindowBgAlpha(1.0f);

				ImGui::PushStyleColor(ImGuiCol_CheckMark, AccentColor.x());

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.1f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, AccentColor.x());

				ImGui::PushStyleColor(ImGuiCol_SliderGrab, AccentColor.x());
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, AccentColor.x());

				ImGui::PushStyleColor(ImGuiCol_Header, AccentColor.x());
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, AccentColor.x());
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, AccentColor.x());

				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));

				if (globals::menucrosshair)
				{
					ImVec2 mousePos = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
					float size = 8.0f;
					ImVec2 squareMin = ImVec2(mousePos.x - size, mousePos.y - size);
					ImVec2 squareMax = ImVec2(mousePos.x + size, mousePos.y + size);
					ImGui::GetBackgroundDrawList()->AddRectFilled(squareMin, squareMax, IM_COL32(255, 255, 255, 255));
				}

				if (globals::settingswindow)
				{
					ImGui::PushStyleColor(ImGuiCol_CheckMark, AccentColor.x());

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.1f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, AccentColor.x());

					ImGui::PushStyleColor(ImGuiCol_SliderGrab, AccentColor.x());
					ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, AccentColor.x());

					ImGui::PushStyleColor(ImGuiCol_Header, AccentColor.x());
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, AccentColor.x());
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, AccentColor.x());

					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));

					ImGuiStyle& style = ImGui::GetStyle();
					style.WindowRounding = 6.0f;
					ImVec2 buttonSize = ImVec2(75, 30);
					ImGui::SetNextWindowSize(ImVec2(240, 380));
					ImGui::Begin(verucryptt("Settings"), NULL, Settings_flags);
					{
						ImGui::SetCursorPos(ImVec2(85, 0));
						ImGui::TextColored(ImColor(255, 0, 255), verucryptt("Exploits"));


						ImGui::SetCursorPos(ImVec2(55, 40));
						if (ImGui::Button(verucryptt("Player"), buttonSize))
							settingtab = 1;

						if (settingtab == 1)
						{
							ImGui::Checkbox(verucryptt("FirstPerson"), &globals::FirstPerson);
							ImGui::Checkbox(verucryptt("Fov Changer"), &globals::fovchanger);
							ImGui::SliderFloat(verucryptt("FOV Value"), &globals::fovchanger_value, 90, 160);
							//ImGui::Checkbox(verucryptt("Player Fly (Detected)"), &globals::playerfly);
							//ImGui::Checkbox(verucryptt("Player Fly2 (Testing)"), &globals::playerfly2);
							//ImGui::Checkbox(verucryptt("Speed Hack"), &globals::speedhack);
							//ImGui::Checkbox(verucryptt("FreeCam Teleport"), &globals::freecam);

							//No exploits for you!!
						}
					}
					ImGui::End();
				}

				ImGui::Begin(verucryptt(""), NULL, window_flags);
				{

					auto window_pozycja = ImGui::GetWindowPos();
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pozycja.x, window_pozycja.y + 30), ImVec2(window_pozycja.x + window_size.x, window_pozycja.y + 32), ImColor(255, 0, 255, 255), 0.0f);

					float x_distance = 115;
					ImGui::SetCursorPosX(x_distance);
					ImGui::GetBackgroundDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMin(), ImColor(255, 0, 255, 255), 0.0f);

					ImVec2 buttonSize = ImVec2(75, 30);
					ImGui::PushStyleColor(ImGuiCol_Text, AccentColor2.x());

					ImGuiStyle& style = ImGui::GetStyle();
					style.FrameRounding = 4.0f;
					style.ChildRounding = 4.0f;

					ImGui::PushFont(FortniteFont);

					float oldfsize = ImGui::GetFont()->Scale;
					ImGui::GetFont()->Scale = 1.13;
					ImGui::PushFont(ImGui::GetFont());


					ImGui::SetCursorPos(ImVec2(17, 10));
					ImGui::TextColored(ImColor(255, 0, 255), verucryptt("K"));
					ImGui::SetCursorPos(ImVec2(24, 10));
					ImGui::TextColored(ImColor(255, 255, 255), verucryptt("raftNite"));

					ImGui::GetFont()->Scale = oldfsize;
					ImGui::PopFont();
					ImGui::GetFont()->Scale = oldfsize;

					ImGui::PushFont(MenuFont3);

					ImGui::SetCursorPos(ImVec2(115, 40));
					if (ImGui::Button(verucryptt(ICON_FA_CROSSHAIRS"Combat"), buttonSize))
						tabs = 1;
					
																																														             ImDrawList* drawList = ImGui::GetWindowDrawList();
																																												         			drawList->AddLine(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + buttonSize.y),
																																											        				ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + buttonSize.y), IM_COL32_WHITE);

					ImGui::SameLine();
					ImGui::SetCursorPosY(40);
					if (ImGui::Button(verucryptt(ICON_FA_EYE"Visuals"), buttonSize))
						tabs = 2;																																				           				drawList->AddLine(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + buttonSize.y),
																																												          			ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + buttonSize.y), IM_COL32_WHITE);

					ImGui::SameLine();
					ImGui::SetCursorPosY(40);
					if (ImGui::Button(verucryptt(ICON_FA_CLOUD"World"), buttonSize))
						tabs = 3;																																							       	drawList->AddLine(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + buttonSize.y),																																									ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + buttonSize.y), IM_COL32_WHITE);
																																																																														drawList->AddLine(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + buttonSize.y),																																									ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y + buttonSize.y), IM_COL32_WHITE);
					ImGui::SameLine();
					ImGui::SetCursorPosY(40);
					if (ImGui::Button(verucryptt(ICON_FA_COG"Settings"), buttonSize))
						tabs = 4;

					ImGui::SetCursorPos(ImVec2(window_size.x - 32, 8.5));
					if (ImGui::Button(verucryptt(ICON_FA_BUG" "), ImVec2(25, 20)))
					{
						globals::settingswindow = !globals::settingswindow;
					}


					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255,255,255,255));
					if (tabs == 1)
					{
						ImGui::SetCursorPosY(73);
						ImGui::BeginChild("##Combat", ImVec2(ImGui::GetContentRegionAvail().x / 2.1, ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Combat"));

						ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(182 / 255.0f, 1 / 255.0f, 183 / 255.0f, 1.0f));
						ImGui::Separator();
						ImGui::PopStyleColor();

						ImGui::Checkbox(verucryptt("Enable Aimbot"), &globals::g_aimbot);
						ImGui::Checkbox(verucryptt("Enable Prediction"), &globals::prediction);
						ImGui::Checkbox(verucryptt("Enable TriggerBot"), &globals::g_triggerbot);
						ImGui::Checkbox(verucryptt("Visible Check"), &globals::g_visible_check);
						ImGui::Checkbox(verucryptt("Enable Target Text"), &globals::g_target_text);
						ImGui::Text(verucryptt("Smoothing"));
						float MaxSmooth = 20;
						if (!globals::vsync)
						{
							MaxSmooth = 100;
						}
						ImGui::SliderFloat(verucryptt(" "), &globals::g_smooth, 5, MaxSmooth);
						ImGui::Text(verucryptt("Fov Value"));
						ImGui::SliderFloat(verucryptt("  "), &globals::g_aimfov, 10, 300);
						ImGui::EndChild();

						ImGui::SameLine();

						ImGui::BeginChild("##Misc", ImVec2(ImGui::GetContentRegionAvail().x / 1, ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Misc"));
						ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(182 / 255.0f, 1 / 255.0f, 183 / 255.0f, 1.0f));
						ImGui::Separator();
						ImGui::PopStyleColor();

						ImGui::Checkbox(verucryptt("Show Fov"), &globals::g_render_fov);

						ImGui::Text(verucryptt("Aimbot Key"));
						HotkeyButton(aimkey, ChangeKey, keystatus);

						ImGui::Text(verucryptt("TriggerBot Key"));
						ImGui::SameLine();
						HotkeyButton(triggerkey, ChangeKey7, keystatus4);

						ImGui::Text(verucryptt("Prediction Distance"));
						ImGui::SliderFloat(verucryptt("   "), &globals::predictiondistance, 50, 250);


						ImGui::Combo(verucryptt(("Hitbox")), &globals::g_hitbox, ("Head\0\Neck\0\Chest\0\Random"));
						ImGui::EndChild();
					}

					if (tabs == 2)
					{
						ImGui::SetCursorPosY(73);
						ImGui::BeginChild("##Visuals", ImVec2(ImGui::GetContentRegionAvail().x / 2.1, ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Visuals"));
						ImGui::Checkbox(verucryptt("Enable Box Esp"), &globals::g_box_esp);
						if (globals::g_box_esp)
						{
							ImGui::Combo(verucryptt(("Box")), &globals::g_box_type, ("2D \0\Rounded\0\Cornered\0\Bounding\0"));
						}
						ImGui::Checkbox(verucryptt("Skeleton Esp"), &globals::g_skeleton);
						ImGui::Checkbox(verucryptt("Head Esp"), &globals::headesp);
						ImGui::Checkbox(verucryptt("Platform Esp"), &globals::g_platform);
						ImGui::Checkbox(verucryptt("Level + Kill Esp"), &globals::PlayerInfo);
						ImGui::Checkbox(verucryptt("Name Esp"), &globals::g_username);
						ImGui::Checkbox(verucryptt("Radar Esp"), &globals::radaresp);
						ImGui::Checkbox(verucryptt("Nearby Players"), &globals::nearbyplayers);
						ImGui::Checkbox(verucryptt("Enable Current Weapon"), &globals::weaponesp);
						ImGui::Checkbox(verucryptt("Snaplines"), &globals::g_line);

						ImGui::EndChild();
						ImGui::SameLine();

						ImGui::BeginChild("##Misc", ImVec2(ImGui::GetContentRegionAvail().x / 1, ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Misc"));
						ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(182 / 255.0f, 1 / 255.0f, 183 / 255.0f, 1.0f));
						ImGui::Separator();
						ImGui::PopStyleColor();

						if (globals::g_line)
						{
							ImGui::Combo(verucryptt(("Line")), &globals::g_line_type, ("Top\0\Middle\0\Bottom\0"));
						}
						ImGui::Checkbox(verucryptt("Distance Esp"), &globals::g_distance);
						if (globals::g_distance)
						{
							ImGui::Combo(verucryptt(("Distance")), &globals::g_distance_type, ("Bottom\0\Top\0"));
						}
						ImGui::Text(verucryptt("Max Esp Distance"));
						ImGui::SliderFloat(verucryptt("    "), &globals::g_render_distance, 10.f, 275.f, "%.1fM");
						ImGui::Text(verucryptt("Box Thickness"));
						ImGui::SliderFloat(verucryptt("     "), &globals::g_box_thick, 1, 5);
						ImGui::Text(verucryptt("Line Thickness"));
						ImGui::SliderInt(verucryptt("      "), &globals::g_line_thick, 1, 5);
						ImGui::Text(verucryptt("Skeleton Thickness"));
						ImGui::SliderInt(verucryptt("       "), &globals::g_skeletonthickness, 1, 5);

						ImGui::ColorEdit4(verucryptt("Visible Color"), globals::g_color_Visible);
						ImGui::ColorEdit4(verucryptt("Not Visible"), globals::g_color_invisible);
						ImGui::ColorEdit4(verucryptt("Normal Color"), globals::g_color);
						ImGui::Text(verucryptt("Entity Distance"));
						ImGui::SliderFloat(verucryptt("        "), &globals::entitydistance, 50, 250);

						ImGui::EndChild();
					}

					if (tabs == 3)
					{
						ImGui::SetCursorPosY(73);
						ImGui::BeginChild("##Configs", ImVec2(ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Configs"));

						if (ImGui::Button(verucryptt("legit Config")))
						{
							globals::g_smooth, 30;
							globals::g_aimfov, 50;
							globals::g_aimbot = true;
							globals::g_render_fov = true;
							globals::g_platform = true;
							globals::g_username = true;
							globals::g_distance = true;
							globals::g_snapline = true;
							globals::g_visible_check = true;
						}
						ImGui::SameLine();
						if (ImGui::Button(verucryptt("tournament config")))
						{
							globals::g_render_fov = true;
							globals::g_platform = false;
							globals::g_username = false;
							globals::g_distance = true;
							globals::g_snapline = false;
							globals::g_visible_check = true;
							globals::g_smooth, 25;
							globals::g_aimfov, 40;
						}
						


						ImGui::Checkbox(verucryptt("Enable Vsync"), &globals::vsync);
						ImGui::Checkbox(verucryptt("Enable Menu Crosshair"), &globals::menucrosshair);

						ImGui::Checkbox(verucryptt("Enable Watermark"), &globals::g_watermark);
						ImGui::Combo(verucryptt(("Watermark Type")), &globals::g_watermark_type, ("Normal\0\Normal2\0\Normal3\0\Beatiful\0\FpsFov"));

						ImGui::Checkbox(verucryptt("Enable Crosshair"), &globals::crosshair);
						ImGui::ColorEdit3(verucryptt("Crosshair Color"), &globals::crosshair_color.x);
						ImGui::SliderFloat(verucryptt("Crosshair Size"), &globals::crosshair_size, 1, 5);
						ImGui::SliderFloat(verucryptt("Crosshair Length"), &globals::crosshair_thickness, 1, 7);
						ImGui::SliderFloat(verucryptt("Crosshair Thickness"), &globals::crosshair_thickness, 1, 10);

						ImGui::EndChild();
					}

					if (tabs == 4)
					{
						ImGui::SetCursorPosY(73);
						ImGui::BeginChild("##Settings", ImVec2(ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y / 1), true, ImGuiWindowFlags_NoResize);
						ImGui::Text(verucryptt("Settings"));

						ImGui::Text(verucryptt("Menu Accent Color"));
						ImGui::ColorEdit4(verucryptt("Menu Accent"), &AccentColor.r);

						ImGui::SliderFloat(verucryptt("Font Size"), &globals::g_font_size, 15, 30);
						ImGui::Checkbox(verucryptt("Enable Outline"), &globals::g_outline);

						ImGui::SliderFloat(verucryptt("Entity Distance"), &globals::entitydistance, 50, 250);

						ImGui::Text(verucryptt("Radar Settings"));
						ImGui::Separator();
						ImGui::SliderFloat(verucryptt("pos x"), &radar_position_x, 10.f, 2000.f);
						ImGui::Separator();
						ImGui::SliderFloat(verucryptt("pos y"), &radar_position_y, 10.f, 2000.f);
						ImGui::Separator();
						ImGui::SliderFloat(verucryptt("distance"), &RadarDistance, 10.f, 700.f);
						ImGui::Separator();
						ImGui::SliderFloat(verucryptt("size"), &radar_size, 100.f, 500.f);
						ImGui::Text(verucryptt("Radar Style"));
						ImGui::Combo(verucryptt(("                                                 ")), &globals::RadarType, ("Circle\0\Square"));

						ImGui::EndChild();
					}

					

					ImGui::End();
				}
			}
		}

		auto draw() -> void {
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

			ImGui::StyleColorsDark();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			g_main->actor_loop();
			//g_loop::LevelDrawing();

			menu();

			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
			d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (globals::vsync)
			{
				d3d_swap_chain->Present(1, 0);
			}
			else
			{
				d3d_swap_chain->Present(0, 0);
			}
		}

		auto render() -> bool {
			static IDXGISwapChain* pSwapChain;
			IDXGISwapChain* pSwaChain = nullptr;
			static RECT rect_og;
			MSG msg = { NULL };
			ZeroMemory(&msg, sizeof(MSG));

			while (msg.message != WM_QUIT)
			{
				UpdateWindow(window_handle);
				ShowWindow(window_handle, SW_SHOW);

				if (PeekMessageA(&msg, window_handle, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				ImGuiIO& io = ImGui::GetIO();
				io.ImeWindowHandle = window_handle;
				io.DeltaTime = 1.0f / 60.0f;

				POINT p_cursor;
				GetCursorPos(&p_cursor);
				io.MousePos.x = p_cursor.x;
				io.MousePos.y = p_cursor.y;

				if (GetAsyncKeyState(VK_LBUTTON)) {
					io.MouseDown[0] = true;
					io.MouseClicked[0] = true;
					io.MouseClickedPos[0].x = io.MousePos.x;
					io.MouseClickedPos[0].x = io.MousePos.y;
				}
				else
					io.MouseDown[0] = false;

				draw();
			}
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			DestroyWindow(window_handle);

			return true;
		}

		void Overlay()
		{
		    hijack();

			imgui();

			render();
		};
	};
} static n_render::c_render* g_render = new n_render::c_render();