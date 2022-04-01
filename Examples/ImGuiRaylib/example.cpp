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

#include "model_viewer.h"
#include "image_viewer.h"
#include "scene_viewer.h"

#include <vector>
#include <fstream>

bool Quit = false;

bool ImGuiDemoOpen = false;

ImageViewerWindow ImageViewer;
SceneViewWindow SceneView;
ModelViewWindow ModelViewer;

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
            ImGui::MenuItem("Model Viewer", nullptr, &ModelViewer.Open);

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

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Demo");
	SetTargetFPS(30);

	// SetupRLImGui(true); // Default
    // Custom ImGui setup
    InitRLGLImGui();
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    FinishRLGLImguSetup();

	ImageViewer.Setup();
    ImageViewer.Open = true;

    SceneView.Setup();
    SceneView.Open = true;

    ModelViewer.Setup();
    ModelViewer.Open = true;

	// Main game loop
	while (!WindowShouldClose() && !Quit)    // Detect window close button or ESC key
	{
		ImageViewer.Update();
        SceneView.Update();
        ModelViewer.Update();

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

        if (ModelViewer.Open)
            ModelViewer.Show();                  

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
    ModelViewer.Shutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}