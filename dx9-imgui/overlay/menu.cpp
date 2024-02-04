#include "menu.h"
#include "elements.h"
#include "fonts.h"
#include "drawing.h"
#include "../config/config.h"

LPCSTR Menu::lpWindowName = "Moonlight autoclicker";
ImVec2 Menu::vWindowSize = { 350, 75 };
ImGuiWindowFlags Menu::WindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
bool Menu::bDraw = true;
Overlay::WindowItem Menu::lpSelectedWindow = { nullptr, "", "" };
LPDIRECT3DDEVICE9 Menu::pD3DDevice = nullptr;


enum heads {
	HEAD_1,
	HEAD_2,
	HEAD_3,
	HEAD_4
};





bool Menu::isActive()
{
	return bDraw == true;
}



void Menu::Draw()
{

	if (GetAsyncKeyState(VK_INSERT) & 1)
		bDraw = !bDraw;


	// Watermark

	// Change if true by a config option
	if (config.overlay.show_watermark)
	{
		static int location = 0;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (location >= 0)
		{
			const float PAD = 30.0f;
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;
			window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			window_flags |= ImGuiWindowFlags_NoMove;
		}
		else if (location == -2)
		{
			// Center window
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			window_flags |= ImGuiWindowFlags_NoMove;
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("watermark", nullptr, window_flags))
		{
			ImGui::Text("Moonlight | %.0f FPS", io.Framerate);
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
				if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
				if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
				if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}


	if (isActive())
	{


		ImGui::ShowDemoWindow();

		ImGuiIO io = ImGui::GetIO();



		/*
		HERE Some basic variable to test some ImGui elements
		Replace this with a config system
		*/
		static heads head_selected = HEAD_1;

		static int combobox, sliderscalar = 0;
		const char* combobox_items[3] = { "Option 1", "Option 2", "Option 3" };
		/*
		END of basic variable
		REMOVE this
		*/


		ImGui::SetNextWindowSize({ 500, 370 });

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));




		ImGui::Begin("dx9-overlay", nullptr, ImGuiWindowFlags_NoDecoration);
		{
			auto draw = ImGui::GetWindowDrawList();

			auto pos = ImGui::GetWindowPos();
			auto size = ImGui::GetWindowSize();

			draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + 51), ImColor(24, 24, 24), 9.0f, ImDrawFlags_RoundCornersTop);
			draw->AddRectFilledMultiColorRounded(pos, ImVec2(pos.x + 55, pos.y + 51), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.05f), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.00f), ImColor(1.0f, 1.0f, 1.0f, 0.05f), 9.0f, ImDrawFlags_RoundCornersTopLeft);

			draw->AddText(fonts::logo, 17.0f, ImVec2(pos.x + 25, pos.y + 17), ImColor(192, 203, 229), "A");
			draw->AddText(fonts::semibold, 17.0f, ImVec2(pos.x + 49, pos.y + 18), ImColor(192, 203, 229), "dx9-overlay");

			ImGui::SetCursorPos({ 125, 19 });
			ImGui::BeginGroup(); {
				if (elements::tab("Head 1", head_selected == HEAD_1)) head_selected = HEAD_1;
				ImGui::SameLine();
				if (elements::tab("Head 2", head_selected == HEAD_2)) head_selected = HEAD_2;
				ImGui::SameLine();
				if (elements::tab("Head 3", head_selected == HEAD_3)) head_selected = HEAD_3;
			}
			ImGui::EndGroup();

			switch (head_selected) {
			case HEAD_1:
				draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 25, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Aim");

				ImGui::SetCursorPos({ 25, 85 });
				ImGui::BeginChild("##container", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
					ImGui::Checkbox("Aimbot", &config.aimbot.enabled);


					ImGui::Combo("Mode", &combobox, combobox_items, IM_ARRAYSIZE(combobox_items));

					ImGui::SliderInt("Fov", &config.aimbot.fov, 0, 400, "%d", ImGuiSliderFlags_NoInput);

					ImGui::Checkbox("Draw Fov", &config.aimbot.draw_fov);

				}
				ImGui::EndChild();

				draw->AddText(fonts::medium, 14.0f, ImVec2(pos.x + 285, pos.y + 60), ImColor(1.0f, 1.0f, 1.0f, 0.6f), "Tab header 2");

				ImGui::SetCursorPos({ 285, 85 });
				ImGui::BeginChild("##container1", ImVec2(190, 275), false, ImGuiWindowFlags_NoScrollbar); {
					ImGui::Text("Body text");
				}
				ImGui::EndChild();
				break;
			}
		}


		ImGui::End();

		ImGui::PopStyleVar();
		
	}
}





/**
	@brief : Function that set the D3D9 device and execute user D3D9 rendering.
	@param pCurrentD3DDevice : current D3D9 device.
**/
void Menu::DXDraw(const LPDIRECT3DDEVICE9 pCurrentD3DDevice)
{
	RECT rect = Overlay::GetTargetWindowRect();


	Drawing::SetDevice(pCurrentD3DDevice);
	if (config.aimbot.draw_fov)
	{
		int center_x = (rect.right-rect.left) / 2;
		int center_y = (rect.bottom-rect.top) / 2;
		Drawing::DrawCircle(center_x, center_y, config.aimbot.fov, RGB(255, 255, 255));
	}

		

	return;
}
