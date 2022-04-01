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

#include "raylib.h"
#include "rcamera.h"
#include "FollowCamera.h"
#include "rlgl.h"

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1024;
	int screenHeight = 768;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "raylibExtras [camera] example - Follow camera");
	SetTargetFPS(30);

	Vector3 vechicle_position = { 0.0f, 0.0f, 0.0f };
	Vector3 relative_position = { 0.0f, 10.0f, -10.0f };
	Vector3 euler_angles      = { 0.0f, 0.0f, 0.0f }; // pitch , yaw, roll

	Model model = LoadModel("../resources/Opel_1967/Opel_1967.gltf");
	model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(-90.0f), DEG2RAD*0.0f, DEG2RAD*0.0f });

	FollowCamera cam;
	cam.Setup(45, vechicle_position, relative_position, euler_angles);
	cam.HideCursor = false;
	cam.FarPlane   = 5000;

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
        if (IsKeyDown(KEY_A)) vechicle_position.x += 0.1f;
        if (IsKeyDown(KEY_D)) vechicle_position.x -= 0.1f;
        if (IsKeyDown(KEY_S)) vechicle_position.z -= 0.1f;
        if (IsKeyDown(KEY_W)) vechicle_position.z += 0.1f;
		if (IsKeyDown(KEY_Q)) {euler_angles.y += 0.1f; vechicle_position.x -= 0.01f;};
		if (IsKeyDown(KEY_E)) {euler_angles.y -= 0.1f; vechicle_position.x += 0.01f;};

		if (IsKeyDown(KEY_UP)) {
			relative_position.y += 0.5f;
			cam.SetCameraRelativePosition(relative_position);
		};

		if (IsKeyDown(KEY_DOWN)) {
			relative_position.y -= 0.5f;
			cam.SetCameraRelativePosition(relative_position);
		};

		if (IsKeyDown(KEY_LEFT)) {
			relative_position.z -= 0.5f;
			cam.SetCameraRelativePosition(relative_position);
		};

		if (IsKeyDown(KEY_RIGHT)) {
			relative_position.z += 0.5f;
			cam.SetCameraRelativePosition(relative_position);
		};		

		// Update camera
		cam.Update(vechicle_position, euler_angles);
		// Update model rotation
		model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(euler_angles.x-90.0f), DEG2RAD*(euler_angles.y), DEG2RAD*(euler_angles.z) });

		BeginDrawing();
			ClearBackground(SKYBLUE);

			cam.BeginMode3D();
				DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 100, 100 }, BEIGE); // simple world plane
				DrawModel(model, vechicle_position, 0.01f, WHITE);   // Draw 3d model with texture
				DrawGrid(100, 1.0f);
			cam.EndMode3D();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	UnloadModel(model);
	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}