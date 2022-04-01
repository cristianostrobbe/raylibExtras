#pragma once
/*******************************************************************************************
*
*   raylib [core] example - Third Person follow camera Example
*
*   This example has been created using raylib 4.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2022 Cristiano Strobbe (@cristianostrobbe)
*
********************************************************************************************/

#include "common.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;


class ImageViewerWindow : public DocumentWindow
{
public:

	void Setup() override
	{
        Camera.zoom = 1;
        Camera.target.x = 0;
        Camera.target.y = 0;
        Camera.rotation = 0;
        Camera.offset.x = GetScreenWidth() / 2.0f;
        Camera.offset.y = GetScreenHeight() / 2.0f;

		ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        ImageTexture = LoadTexture("../Examples/resources/parrots.png");

		UpdateRenderTexture();
	}

	void Show() override
	{
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("Image Viewer", &Open, ImGuiWindowFlags_NoScrollbar))
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

                if (ImGui::Button("None"))
                {
                    CurrentToolMode = ToolMode::None;
                }
                ImGui::SameLine();

                if (ImGui::Button("Move"))
                {
                    CurrentToolMode = ToolMode::Move;
                }

                ImGui::SameLine();
                switch (CurrentToolMode)
                {
                case ToolMode::None:
                    ImGui::TextUnformatted("No Tool");
                    break;
                case ToolMode::Move:
                    ImGui::TextUnformatted("Move Tool");
                    break;
                default:
                    break;
                }

                ImGui::SameLine();
                ImGui::TextUnformatted(TextFormat("camera target X%f Y%f", Camera.target.x, Camera.target.y));
                ImGui::EndChild();
            }

            RLImGuiImageRect(&ViewTexture.texture, (int)size.x, (int)size.y, viewRect);

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

    Texture ImageTexture;
    Camera2D Camera = { 0 };

    Vector2 LastMousePos = { 0 };
    Vector2 LastTarget = { 0 };
    bool Dragging = false;

    bool DirtyScene = false;

    enum class ToolMode
    {
        None,
        Move,
    };

    ToolMode CurrentToolMode = ToolMode::None;

    void UpdateRenderTexture()
    {
        BeginTextureMode(ViewTexture);
        ClearBackground(BLUE);
        BeginMode2D(Camera);
        DrawTexture(ImageTexture, ImageTexture.width / -2, ImageTexture.height / -2, WHITE);
        EndMode2D();
        EndTextureMode();
    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(ImageTexture);
    }
};