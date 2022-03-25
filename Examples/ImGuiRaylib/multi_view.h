
#include "common.h"

#include "FollowCamera.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class MultiViewWindow : public DocumentWindow
{
public:

    FollowCamera Camera;
    FollowCamera Camera1;
    Texture2D GridTexture = { 0 };
    RenderTexture ViewTexture1;
    Rectangle splitScreenRect;

    Vector3 vechicle_position  = { 0.0f, 0.0f, 0.0f };
	Vector3 relative_position  = { 0, 10.0f, -10.0f };
    Vector3 relative_position1 = { 0, 10.0f, -10.0f };
	Vector3 euler_angles       = { 0.0f, 0.0f, 0.0f }; // pitch , yaw, roll

    void Setup() override
    {
        ViewTexture = LoadRenderTexture(GetScreenWidth()/2, GetScreenHeight()/2);
        ViewTexture1 = LoadRenderTexture(GetScreenWidth()/2, GetScreenHeight()/2);

        Camera.HideCursor = false;
        Camera.Setup(45,vechicle_position, relative_position, euler_angles);

        Camera1.HideCursor = false;
        Camera1.Setup(45, vechicle_position, relative_position1, euler_angles);

        // Build a flipped rectangle the size of the split view to use for drawing later
        splitScreenRect = { 0.0f, 0.0f, (float)ViewTexture.texture.width, (float)-ViewTexture.texture.height };
    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadRenderTexture(ViewTexture1);
        UnloadTexture(GridTexture);
    }

    void Show() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("3D View - Left and right view", &Open, ImGuiWindowFlags_NoScrollbar))
        {
            Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

            ImVec2 size = ImGui::GetContentRegionAvail();

            Rectangle viewRect = { 0 };
            viewRect.x = ViewTexture.texture.width / 2 - size.x / 2;
            viewRect.y = ViewTexture.texture.height / 2 - size.y / 2;
            viewRect.width = size.x;
            viewRect.height = -size.y;

            // draw the view
            RLImGuiImageRect(&ViewTexture.texture, (int)size.x, (int)size.y, viewRect);

            Rectangle viewRect1 = { 0 };
            viewRect1.x = ViewTexture1.texture.width / 2 - size.x / 2;
            viewRect1.y = ViewTexture1.texture.height / 2 - size.y / 2;
            viewRect1.width = size.x;
            viewRect1.height = -size.y;

            // draw the view
            RLImGuiImageRect(&ViewTexture1.texture, (int)size.x, (int)size.y, viewRect);

            ImGui::SetCursorPos(ImVec2(0, 0));

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
            UnloadRenderTexture(ViewTexture1);
            ViewTexture = LoadRenderTexture(GetScreenWidth()/2, GetScreenHeight()/2);
            ViewTexture1 = LoadRenderTexture(GetScreenWidth()/2, GetScreenHeight()/2);
        }

        BeginTextureMode(ViewTexture);
            ClearBackground(SKYBLUE);

            if (Focused && IsMouseButtonDown(1))
                Camera.Update(vechicle_position, euler_angles);

            Camera.BeginMode3D();

            // grid of cube trees on a plane to make a "world"
            DrawPlane(Vector3 { 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE); // simple world plane

            Camera.EndMode3D();
        EndTextureMode();
 
        BeginTextureMode(ViewTexture1);
            ClearBackground(SKYBLUE);

            if (Focused && IsMouseButtonDown(1))
                Camera1.Update(vechicle_position, euler_angles);

            Camera1.BeginMode3D();

            // grid of cube trees on a plane to make a "world"
            DrawPlane(Vector3 { 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE); // simple world plane

            Camera1.EndMode3D();
        EndTextureMode();

        // Draw both views render textures to the screen side by side
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureRec(ViewTexture.texture, splitScreenRect, (Vector2){ 0, 0 }, WHITE);
            DrawTextureRec(ViewTexture1.texture, splitScreenRect, (Vector2){ GetScreenWidth()/2.0f, 0 }, WHITE);
        EndDrawing();
    }
};