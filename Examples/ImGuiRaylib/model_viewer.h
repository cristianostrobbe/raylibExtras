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
#include <fstream>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

class ModelViewWindow : public DocumentWindow
{
public:
    FPCamera Camera;
    Model model = { 0 };
    Model floor = { 0 };
    Light light = { 0 };
    Shader shader = { 0 };
    Material material = { 0 };
    Texture2D GridTexture = { 0 };
    Texture2D model_texture = { 0 };
    Texture2D model_texture_metalness = { 0 };

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
        Camera.Setup(45, Vector3Zero());

        Camera.MoveSpeed.z = 10;
        Camera.MoveSpeed.x = 10;

        shader = LoadShader(TextFormat("../raylib/examples/models/resources/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                            TextFormat("../raylib/examples/models/resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));

        // Get some required shader loactions
        shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
        // Ambient light level (some basic lighting)
        int ambientLoc = GetShaderLocation(shader, "ambient");
        SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

        model = LoadModel("../resources/Opel_1967/Opel_1967.gltf");
        model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch - 90.0f), DEG2RAD*yaw, DEG2RAD*roll });

        floor = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
        floor.materials[0].shader = shader;

        light = CreateLight(LIGHT_POINT, (Vector3){ 0, 10, 0 }, Vector3Zero(), (Color){150, 120, 120, 120}, shader);

    }

    void Shutdown() override
    {
        UnloadRenderTexture(ViewTexture);
        UnloadTexture(GridTexture);
        // UnloadTexture(model_texture);
        UnloadModel(model); // Unload model data
    }

    void Show() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSizeConstraints(ImVec2(1600, 900), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

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

        // Update light values (actually, only enable/disable them)
        UpdateLightValues(shader, light);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        Vector3 cam_pos = Camera.GetCameraPosition();
        float cameraPos[3] = { cam_pos.x, cam_pos.y, cam_pos.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Plane roll (z-axis) controls
        if (IsKeyDown(KEY_LEFT)) yaw += 1.0f;
        else if (IsKeyDown(KEY_RIGHT)) yaw -= 1.0f;

        model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch - 90.0f), DEG2RAD*yaw, DEG2RAD*roll });

        DrawText("Use left and right keys to rotate model", 10, 40, 20, DARKGRAY);

        BeginTextureMode(ViewTexture);
            ClearBackground(SKYBLUE);
            // ClearBackground(BLACK);

            if (Focused && IsMouseButtonDown(1))
                Camera.Update();

            Camera.BeginMode3D();

                DrawModel(floor, Vector3Zero(), 1.0f, LIGHTGRAY);
                DrawGrid(10, 1.0f); // Draw a grid
                DrawModel(model, (Vector3){ x, y, z }, 0.01f, WHITE);   // Draw 3d model with texture

            Camera.EndMode3D();

        EndTextureMode();
    }
};