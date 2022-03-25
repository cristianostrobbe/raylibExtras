#include "common.h"

#include "map_viewer.h"
#include "model_viewer.h"
#include "scene_viewer.h"
#include "multi_view.h"

#include "FPCamera.h"
#include "FollowCamera.h"

#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool Quit = false;

bool ImGuiDemoOpen = false;

SceneViewWindow SceneView;
MapViewWindow MapView;
ModelViewWindow ModelViewer;
MultiViewWindow MultiView;

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
            ImGui::MenuItem("3D View", nullptr, &SceneView.Open);
            ImGui::MenuItem("Map View", nullptr, &MapView.Open);
            ImGui::MenuItem("Model Viewer", nullptr, &ModelViewer.Open);
			ImGui::MenuItem("Multi Viewer", nullptr, &MultiView.Open);

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

	RenderTexture screen1 = LoadRenderTexture(screenWidth/2, screenHeight);

    RenderTexture screeN2 = LoadRenderTexture(screenWidth/2, screenHeight);

	// SetupRLImGui(true); // Default
    // Custom ImGui setup
    InitRLGLImGui();
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::GetIO().Fonts->AddFontFromFileTTF("../resources/fonts/Roboto-Medium.ttf", 14.0f);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    FinishRLGLImguSetup();

    SceneView.Setup();
    SceneView.Open = true;

    MapView.Setup();
    MapView.Open = true;

    ModelViewer.Setup();
    ModelViewer.Open = true;

    MultiView.Setup();
    MultiView.Open = true;

	// Main game loop
	while (!WindowShouldClose() && !Quit)    // Detect window close button or ESC key
	{
        SceneView.Update();
        MapView.Update();
        ModelViewer.Update();
		MultiView.Update();

		BeginDrawing();
		ClearBackground(DARKGRAY);

		BeginRLImGui();
		DoMainMenu();

		if (ImGuiDemoOpen)
			ImGui::ShowDemoWindow(&ImGuiDemoOpen);

        if (SceneView.Open)
            SceneView.Show();

        if (MapView.Open)
            MapView.Show(); 

        if (ModelViewer.Open)
            ModelViewer.Show();   

        if (MultiView.Open)
            MultiView.Show();   

		EndRLImGui();

        #ifdef DEBUG_MODE
        DrawFPS(10, 20);
        #endif

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	ShutdownRLImGui();

    SceneView.Shutdown();
    MapView.Shutdown();
    ModelViewer.Shutdown();
	MultiView.Shutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}