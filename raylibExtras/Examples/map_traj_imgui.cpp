#include "raylib.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void DrawLine(std::vector<Vector3> &points)
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
        DrawCircle3D(point, 0.015f, (Vector3){ 0.0f, 0.0f, 0.0f }, 0.0f, GOLD);
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

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1024;
    const int screenHeight = 768;

    bool fullscreen = false;
    bool vsync = true;
    
    int flags = 0;
    if (fullscreen)
        flags |= FLAG_FULLSCREEN_MODE;
    if (vsync)
        flags |= FLAG_VSYNC_HINT;
    
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE | flags);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | flags);
    InitWindow(screenWidth, screenHeight, "map viewer");

    // SetExitKey(KEY_Q);
    SetExitKey(0); // Disable esc

    SetTraceLogLevel(LOG_WARNING);

    // CamTarget cam_position = {0.0f, 5.0f, 0.0f};
    CamTarget cam_position;

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 15.0f };  // Camera position
    //camera.target = (Vector3){ cam_position.x, cam_position.y, cam_position.z };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    // SetCameraMode(camera, CAMERA_FIRST_PERSON);      // Set camera mode
    // SetCameraMode(camera, CAMERA_ORBITAL);           // Camera rotates around target
    // SetCameraMode(camera, CAMERA_THIRD_PERSON);      // Disable cursor
    // SetCameraMode(camera, CAMERA_CUSTOM);            // Custom
    
    SetCameraMode(camera, CAMERA_FREE);           // 
    // SetCameraPanControl(KEY_R);              // Set camera pan key to combine with mouse movement (free camera)
    // SetCameraAltControl(KEY_RIGHT_ALT);                       // Set camera alt key to combine with mouse movement (free camera)
    // SetCameraSmoothZoomControl(int szKey);                 // Set camera smooth zoom key to combine with mouse (free camera)
    // SetCameraMoveControls(int frontKey, int backKey, int rightKey, int leftKey, int upKey, int downKey); // Set camera move controls (1st person and 3rd person cameras)


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

    std::vector<Vector3> my_cloud;
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

    std::vector<Vector3> my_trajectory;
    LoadTrajectoryFromJson(map_traj_file, my_trajectory);    

    SetTargetFPS(30); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 2;
	io.IniFilename = nullptr;
    io.Fonts->AddFontFromFileTTF("../resources/fonts/Roboto-Medium.ttf", 14.0f);
	ImGui::StyleColorsDark();

    imgui_integration::Initialize();

    bool show=false;

    bool p_open = false;
    bool spawn_on_mouse_pos = false;
    static ImGuiWindowFlags imgui_flags = ImGuiWindowFlags_None;

    bool run = true;

    // Main game loop
    // while (!WindowShouldClose()) // Detect window close button or ESC key
    while (run) //
    {
        if (imgui_integration::IsImGuiUsingMouse() == false) {
			blender_like_camera::UpdateCamera(&camera);
		}

        // if (IsKeyPressed(KEY_S)){
        //     p_open = !p_open;
        // }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
            p_open = !p_open;
            spawn_on_mouse_pos = true;
        }

        // UpdateCamera(&camera); // Update camera

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera); 

                // DrawLine(lane1);
                DrawLine(my_trajectory);

                DrawPoints(my_cloud);

                DrawGrid(1000, 1.0f); // Draw a grid

            EndMode3D();


            imgui_integration::BeginImGui();

            // static bool show_demo_window = true;
            // ImGui::ShowDemoWindow(NULL);

            if(p_open){
                if(spawn_on_mouse_pos){
                    // Raylib
                    // Vector2 mouse_pos = GetMousePosition();
                    // ImGui::SetNextWindowPos({ mouse_pos.x, mouse_pos.y }, ImGuiCond_FirstUseEver);
                    // ImGui
                    ImVec2 mouse_pos = ImGui::GetCursorPos();
                    ImGui::SetNextWindowPos({ mouse_pos.x, mouse_pos.y }, ImGuiCond_FirstUseEver);
                    spawn_on_mouse_pos = false;
                }
                ImGui::SetNextWindowSize({ 200, 200 }, ImGuiCond_Once);
                ImGui::Begin("Settings");
                ImGui::Text("fps = %d", GetFPS());

                if (ImGui::Button("Delete.."))
                    ImGui::OpenPopup("Delete?");

                if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Do you want to close the application?\nThis operation cannot be undone!\n\n");
                    ImGui::Separator();

                    static bool dont_ask_me_next_time = false;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                    ImGui::PopStyleVar();

                    if (ImGui::Button("OK", ImVec2(120, 0))) { 
                        ImGui::CloseCurrentPopup(); 
                        run = false;
                    }
                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }
                ImGui::End();
            }

            imgui_integration::EndImGui();

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}