/*******************************************************************************************
*
*   raylib [core] example - Third Person Orbit Camera Example
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "rcamera.h"
// #include "raymath.h"
#include "FollowCamera.h"
#include "rlgl.h"

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1024;
	int screenHeight = 768;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "raylibExtras [camera] example - Follow camera");
	SetTargetFPS(30);

    Image img = GenImageChecked(256, 256, 32, 32, DARKGRAY, WHITE);
    Texture tx = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(tx, TEXTURE_FILTER_ANISOTROPIC_16X);
    SetTextureWrap(tx, TEXTURE_WRAP_CLAMP);

	Model model = LoadModel("../resources/Testarossa/Testarossa.gltf");
	// Model model = LoadModel("../resources/SimpleCar/simple_cat.gltf");

	Vector3 vechicle_position = { 0.0f, 0.0f, 0.0f };
	Vector3 relative_position = { 0, 10.0f, -10.0f };
	Vector3 euler_angles      = { 0, 0.0f, 0.0f }; // pitch , yaw, roll

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
		if (IsKeyDown(KEY_E)) {euler_angles.y += 0.1f; vechicle_position.x -= 0.01f;};
		if (IsKeyDown(KEY_Q)) {euler_angles.y -= 0.1f; vechicle_position.x += 0.01f;};

		model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(euler_angles.x - 90.0f), DEG2RAD*euler_angles.y, DEG2RAD*euler_angles.z });

		cam.Update(vechicle_position, euler_angles);

		BeginDrawing();
		ClearBackground(SKYBLUE);

		cam.BeginMode3D();

		// grid of cube trees on a plane to make a "world"
		DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 100, 100 }, BEIGE); // simple world plane

		// DrawCubeTexture(tx, Vector3{ x, y, z }, 1, 1, 1, GREEN);
		DrawModel(model, (Vector3){ vechicle_position.x, vechicle_position.y, vechicle_position.z }, 1.0f, WHITE);   // Draw 3d model with texture

		DrawGrid(100, 1.0f);

		cam.EndMode3D();

		DrawFPS(0, 0);
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	UnloadTexture(tx);
	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}