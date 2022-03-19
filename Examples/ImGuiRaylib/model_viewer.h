#pragma once
#include "common.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class ModelViewWindow : public DocumentWindow
{
public:
    FPCamera Camera;
    Model model = { 0 };
    Material material = { 0 };
    Texture2D GridTexture = { 0 };
    // Model orientation
    float pitch = 0.0f;
    float roll  = 0.0f; 
    float yaw   = 0.0f;
    float x     = 0.0f;
    float y     = 0.0f; 
    float z     = 0.0f;

    void Setup() override
    {
        ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

        Camera.HideCursor = false;
        Camera.Setup(45, Vector3{ 0,5,10 });

        Camera.MoveSpeed.z = 10;
        Camera.MoveSpeed.x = 10;

        // Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
        // GridTexture = LoadTextureFromImage(img);
        // UnloadImage(img);
        // SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
        // SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);

        // model = LoadModel("../resources/plane.obj");                  // Load model
        // Texture2D model_texture = LoadTexture("../resources/plane_diffuse.png");  // Load model texture
        // model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = model_texture; // Set map diffuse texture

        // model = LoadModel("../resources/SimpleCar/simple_car.gltf");
        // GridTexture = LoadTexture("../resources/SimpleCar/simple_car.png");  // Load model texture
        
        // model = LoadModel("../resources/SimpleCar/simple_car.obj");
        // Texture2D model_texture = LoadTexture("../resources/SimpleCar/simple_car.png");  // Load model texture
        // model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = model_texture; // Set map diffuse texture

        model = LoadModel("../resources/Testarossa/Testarossa.gltf");
        model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch - 90.0f), DEG2RAD*yaw, DEG2RAD*roll });

        // model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = GridTexture; 
        // GridTexture = LoadTexture("resources/models/obj/plane_diffuse.png")
        // Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
        // GridTexture = LoadTextureFromImage(img);
        // UnloadImage(img);
        // SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
        // SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);
    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(GridTexture);
        UnloadModel(model); // Unload model data
    }

    void Show() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(1024, 768), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

        if (ImGui::Begin("Model viewer", &Open, ImGuiWindowFlags_NoScrollbar))
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

        // Plane pitch (x-axis) controls
        if (IsKeyDown(KEY_DOWN)) pitch += 0.6f;
        else if (IsKeyDown(KEY_UP)) pitch -= 0.6f;
        else
        {
            if (pitch > 0.3f) pitch -= 0.3f;
            else if (pitch < -0.3f) pitch += 0.3f;
        }

        // Plane roll (z-axis) controls
        if (IsKeyDown(KEY_LEFT)) roll += 1.0f;
        else if (IsKeyDown(KEY_RIGHT)) roll -= 1.0f;
        else
        {
            if (roll > 0.0f) roll -= 0.5f;
            else if (roll < 0.0f) roll += 0.5f;
        }

        // Move model
        if (IsKeyDown(KEY_A)) x += 0.1f;
        else if (IsKeyDown(KEY_D)) x -= 0.1f;
        else if (IsKeyDown(KEY_S)) z -= 0.1f;
        else if (IsKeyDown(KEY_W)) z += 0.1f;

        model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch - 90.0f), DEG2RAD*yaw, DEG2RAD*roll });

        BeginTextureMode(ViewTexture);
        ClearBackground(SKYBLUE);
        // ClearBackground(BLACK);

        if (Focused && IsMouseButtonDown(1))
            Camera.Update();

        Camera.BeginMode3D();

        DrawGrid(10, 2.5f); // Draw a grid
        DrawModel(model, (Vector3){ x, y, z }, 1.0f, WHITE);   // Draw 3d model with texture

        Camera.EndMode3D();
        EndTextureMode();
 
    }
};