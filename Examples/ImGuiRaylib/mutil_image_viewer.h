#include "common.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class MultipleImageViewerWindow : public DocumentWindow
{
public:
    Texture cam1texture; // IR left
    Texture cam2texture; // IR right
    Texture cam3texture; // Color
    Camera2D Camera = { 0 };

    Vector2 LastMousePos = { 0 };
    Vector2 LastTarget = { 0 };
    bool Dragging = false;

    bool DirtyScene = false;

    enum class ViewerMode
    {
        None,
        Play,
        Pause,
    };

    enum class ToolMode
    {
        None,
        Move,
    };

    ToolMode CurrentToolMode = ToolMode::None;
    ViewerMode CurrentViewerMode = ViewerMode::None;

	void Setup() override
	{
        Camera.zoom = 1;
        Camera.target.x = 0;
        Camera.target.y = 0;
        Camera.rotation = 0;
        Camera.offset.x = GetScreenWidth() / 2.0f;
        Camera.offset.y = GetScreenHeight() / 2.0f;

		ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        cam1texture = LoadTexture("../Examples/resources/parrots.png");
        cam2texture = LoadTexture("../Examples/resources/parrots.png");
        cam3texture = LoadTexture("../Examples/resources/parrots.png");

		UpdateRenderTexture();
	}

	void Show() override
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("Camera Viewer", &Open, ImGuiWindowFlags_NoScrollbar))
        {
            Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            ImVec2 size = ImGui::GetContentRegionAvail();

            // center the scratchpad in the view
            Rectangle viewRect = { 0 };
            viewRect.x = ViewTexture.texture.width / 2 - size.x / 2;
            viewRect.y = ViewTexture.texture.height / 2 - size.y / 2;
            viewRect.width = size.x;
            viewRect.height = -size.y;

            if (ImGui::BeginChild("Toolbar", ImVec2(ImGui::GetContentRegionAvail().x, 25)))
            {
                ImGui::SetCursorPosX(2);
                ImGui::SetCursorPosY(3);

                if (ImGui::Button("Start"))
                {
                    CurrentViewerMode = ViewerMode::Play;
                }
                ImGui::SameLine();

                if (ImGui::Button("Stop"))
                {
                    CurrentViewerMode = ViewerMode::Pause;
                }

                ImGui::SameLine();
                
                if (ImGui::Button("Move"))
                {
                    CurrentToolMode = ToolMode::Move;
                }

                ImGui::SameLine();

                switch (CurrentViewerMode)
                {
                case ViewerMode::None:
                    ImGui::TextUnformatted("Pause");
                    break;
                case ViewerMode::Pause:
                    ImGui::TextUnformatted("Pause");
                    break;                    
                case ViewerMode::Play:
                    ImGui::TextUnformatted("Playing...");
                    break;
                default:
                    break;
                }

                switch (CurrentToolMode)
                {
                case ToolMode::None:
                    ImGui::TextUnformatted("Move disabled");
                    break;
                case ToolMode::Move:
                    ImGui::TextUnformatted("Move enabled");
                    break;
                default:
                    break;
                }

                ImGui::SameLine();
                ImGui::TextUnformatted(TextFormat("Camera resolution: X%i Y%i", (int)size.x, (int)size.y));
                ImGui::EndChild();
            }

            RLImGuiImageRect(&ViewTexture.texture, (int)size.x, (int)size.y, viewRect);
            // RLImGuiImageRect(&ViewTexture.texture, (int)size.x, (int)size.y, viewRect);
            // RLImGuiImageRect(&ViewTexture.texture, (int)size.x, (int)size.y, viewRect);

            ImGui::End();
        }
        ImGui::PopStyleVar();
	}

	void Update() override
	{
        if (!Open)
            return;

        if (IsWindowResized())
        {
            UnloadRenderTexture(ViewTexture);
			ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

            Camera.offset.x = GetScreenWidth() / 2.0f;
            Camera.offset.y = GetScreenHeight() / 2.0f;
        }

		if (Focused)
		{
            if (CurrentToolMode == ToolMode::Move)
            {
                if (IsMouseButtonDown(0))
                {
                    if (!Dragging)
                    {
                        LastMousePos = GetMousePosition();
                        LastTarget = Camera.target;
                    }
                    Dragging = true;
                    Vector2 mousePos = GetMousePosition();
                    Vector2 mouseDelta = Vector2Subtract(LastMousePos, mousePos);

                    mouseDelta.x /= Camera.zoom;
                    mouseDelta.y /= Camera.zoom;
                    Camera.target = Vector2Add(LastTarget, mouseDelta);

                    DirtyScene = true;
                    
                }
                else
                {
                    Dragging = false;
                }
            }
		}
		else
		{
			Dragging = false;
		}

        if (DirtyScene)
        {
            DirtyScene = false;
            UpdateRenderTexture();
        }
	}

    void UpdateRenderTexture()
    {
        BeginTextureMode(ViewTexture);
        ClearBackground(BLUE);
        BeginMode2D(Camera);
        DrawTexture(cam1texture, cam1texture.width / -2, cam1texture.height / -2, WHITE);
        DrawTexture(cam2texture, cam2texture.width * 2 / -2, cam2texture.height / -2, WHITE);
        DrawTexture(cam3texture, cam3texture.width * 3 / -2, cam3texture.height / -2, WHITE);
        EndMode2D();
        EndTextureMode();
    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(cam1texture);
    }
};