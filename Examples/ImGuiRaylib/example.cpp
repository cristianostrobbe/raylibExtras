#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"

#include "FPCamera.h"

#include "common.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool Quit = false;

bool ImGuiDemoOpen = false;

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

class SceneViewWindow : public DocumentWindow
{
public:

    void Setup() override
    {
        ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

        Camera.HideCursor = false;
        Camera.Setup(45, Vector3{ 0,1,0 });

        Camera.MoveSpeed.z = 10;
        Camera.MoveSpeed.x = 10;

        Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
        GridTexture = LoadTextureFromImage(img);
        UnloadImage(img);
        SetTextureFilter(GridTexture, TEXTURE_FILTER_ANISOTROPIC_16X);
        SetTextureWrap(GridTexture, TEXTURE_WRAP_CLAMP);
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

        if (ImGui::Begin("3D View - Right Drag to Move", &Open, ImGuiWindowFlags_NoScrollbar))
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
        ClearBackground(SKYBLUE);

        if (Focused && IsMouseButtonDown(1))
            Camera.Update();

        Camera.BeginMode3D();

        // grid of cube trees on a plane to make a "world"
        DrawPlane(Vector3 { 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE); // simple world plane
        float spacing = 4;
        int count = 5;

        for (float x = -count * spacing; x <= count * spacing; x += spacing)
        {
            for (float z = -count * spacing; z <= count * spacing; z += spacing)
            {
                Vector3 pos = { x, 0.5f, z };

                Vector3 min = { x - 0.5f,0,z - 0.5f };
                Vector3 max = { x + 0.5f,1,z + 0.5f };

                DrawCubeTexture(GridTexture, Vector3{ x, 1.5f, z }, 1, 1, 1, GREEN);
                DrawCubeTexture(GridTexture, Vector3{ x, 0.5f, z }, 0.25f, 1, 0.25f, BROWN);
            }
        }

        Camera.EndMode3D();
        EndTextureMode();
 
    }

    FPCamera Camera;
    Texture2D GridTexture = { 0 };
};


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


ImageViewerWindow ImageViewer;
SceneViewWindow SceneView;
MapViewWindow MapView;
MultipleImageViewerWindow CamerasViewer;

void DoMainMenu()
{
    ImGui::DockSpaceOverViewport();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				Quit = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &ImGuiDemoOpen);
			ImGui::MenuItem("Image Viewer", nullptr, &ImageViewer.Open);
            ImGui::MenuItem("3D View", nullptr, &SceneView.Open);
            ImGui::MenuItem("Map View", nullptr, &MapView.Open);
            ImGui::MenuItem("Camera Viewer", nullptr, &CamerasViewer.Open);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
    #ifdef DEBUG_MODE
    ImGui::ShowAboutWindow();
    #endif
}

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Perception system viewer");
	SetTargetFPS(30);

	// SetupRLImGui(true); // Default
    // Custom ImGui setup
    InitRLGLImGui();
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::GetIO().Fonts->AddFontFromFileTTF("../resources/fonts/Roboto-Medium.ttf", 14.0f);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    FinishRLGLImguSetup();

	ImageViewer.Setup();
    ImageViewer.Open = true;

    SceneView.Setup();
    SceneView.Open = true;

    MapView.Setup();
    MapView.Open = true;

    CamerasViewer.Setup();
    CamerasViewer.Open = true;

	// Main game loop
	while (!WindowShouldClose() && !Quit)    // Detect window close button or ESC key
	{
		ImageViewer.Update();
        SceneView.Update();
        MapView.Update();
        CamerasViewer.Update();

		BeginDrawing();
		ClearBackground(DARKGRAY);

		BeginRLImGui();
		DoMainMenu();

		if (ImGuiDemoOpen)
			ImGui::ShowDemoWindow(&ImGuiDemoOpen);

		if (ImageViewer.Open)
            ImageViewer.Show();

        if (SceneView.Open)
            SceneView.Show();

        if (MapView.Open)
            MapView.Show();    

        if (CamerasViewer.Open)
            CamerasViewer.Show();            

		EndRLImGui();

        #ifdef DEBUG_MODE
        DrawFPS(10, 20);
        #endif

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	ShutdownRLImGui();

    ImageViewer.Shutdown();
    SceneView.Shutdown();
    MapView.Shutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}