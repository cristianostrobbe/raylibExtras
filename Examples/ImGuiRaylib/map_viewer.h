#include "common.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class MapViewWindow : public DocumentWindow
{
public:
    FPCamera Camera;
    Texture2D GridTexture = { 0 };

    std::vector<Vector3> my_cloud;
    std::vector<Vector3> my_trajectory;

    void Setup() override
    {
        ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

        Camera.HideCursor = false;
        Camera.Setup(45, Vector3{ 0,10,0 });

        // Camera.ViewCamera.position

        Camera.MoveSpeed.z = 10;
        Camera.MoveSpeed.x = 10;

        // Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
        // GridTexture = LoadTextureFromImage(img);
        // UnloadImage(img);
        // SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
        // SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);

        json map_file;
        try
        {
            std::ifstream f("../data/maps/map.json");
            f >> map_file;
            f.close();
        }
        catch(const std::exception& e)
        {
            exit(0);
        }

        // std::vector<Vector3> my_cloud;
        LoadMapFromJson(map_file, my_cloud);

        json map_traj_file;
        try
        {
            std::ifstream f1("../data/trajectory/map_traj.json");
            f1 >> map_traj_file;
            f1.close();
        }
        catch(const std::exception& e)
        {
            exit(0);
        }

        // std::vector<Vector3> my_trajectory;
        LoadTrajectoryFromJson(map_traj_file, my_trajectory);   
    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(GridTexture);
    }

    void Show() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("Map viewer", &Open, ImGuiWindowFlags_NoScrollbar))
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
            ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        }

        BeginTextureMode(ViewTexture);
        // ClearBackground(SKYBLUE);
        ClearBackground(BLACK);

        if (Focused && IsMouseButtonDown(1))
            Camera.Update();

        Camera.BeginMode3D();

        // grid of cube trees on a plane to make a "world"
        // DrawPlane(Vector3 { 0, 0, 0 }, Vector2{ 50, 50 }, DARKBLUE); // simple world plane
        DrawGrid(100, 2.5f); // Draw a grid
        DrawLines(my_trajectory);
        DrawPoints(my_cloud);

        // DrawFPS(10, 10);

        Camera.EndMode3D();
        EndTextureMode();
 
    }

private:
    void DrawLines(std::vector<Vector3> &points)
    {
        for(int i=0; i<(int)points.size()-1; i++)
        {
            DrawLine3D(points[i], points[i+1], RED);
        }
    }

    void DrawPoints(std::vector<Vector3> &points)
    {
        for(Vector3 point : points)
        {
            DrawCircle3D(point, 0.03f, (Vector3){ 0.0f, 0.0f, 0.0f }, 0.0f, GOLD);
            // DrawSphere(point, 0.02f, GOLD); // too heavy
            // DrawSphereEx(point, 0.02f, 3, 3, GOLD); // too heavy
        }
    }

    void LoadMapFromJson(json &map_file, std::vector<Vector3> &map)
    {
        map.clear();
        for (int i=0; i<(int)map_file["map"]["points"]["x"].size(); i++)
        {
            map.push_back((Vector3){map_file["map"]["points"]["x"][i], map_file["map"]["points"]["y"][i], map_file["map"]["points"]["z"][i]});
        }
    }

    void LoadTrajectoryFromJson(json &trajectory_file, std::vector<Vector3> &trajectory)
    {
        trajectory.clear();
        for (int i=0; i<(int)trajectory_file["trajectory"]["points"]["x"].size(); i++)
        {
            trajectory.push_back((Vector3){trajectory_file["trajectory"]["points"]["x"][i], trajectory_file["trajectory"]["points"]["y"][i], trajectory_file["trajectory"]["points"]["z"][i]});
        }
    }

};