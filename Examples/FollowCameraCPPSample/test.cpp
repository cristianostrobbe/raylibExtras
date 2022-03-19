// From: https://www.reddit.com/r/raylib/comments/gdqy0h/some_maths_about_camera_in_3d/

#include "raylib.h"
#include "rcamera.h"
// #include "FollowCamera.h"
#include "raymath.h"
#include "math.h"
#include "rlgl.h"

#define MAX_STEERING_ANGLE 0.8f // rad
#define MAX_THROTTLE       1.0f // #
#define VEHICLE_LENGTH     2.5f // m
#define MAX_TURNING_RADIUS 10000000000000.0f // m

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1024;
	int screenHeight = 768;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "raylibExtras [camera] example - Follow camera");
	SetTargetFPS(30);

	Vector3 vechicle_position = { 0.0f, 0.0f, 0.0f };
	Camera3D camera = { 0 };
	camera.position = { 0.0f, 10.0f, 10.0f }; // Camera position
	camera.target   = vechicle_position; // Camera looking at point
	camera.up       = { 0.0f, 2.0f, 0.0f }; // Camera up vector (rotation towards target)
	camera.fovy     = 45.0f; // Camera field-of-view Y

	// float distance           = 15.0f; // Set the distance from camera.target to camera.position
	// float verticalAngle      = 45.0f; // The Y/XZ angle
	// float horizontalAngle    = 90.0f; // The X/Z angle, y is considered as 0
	// float horizontalDistance = distance * cosf(verticalAngle * PI / 180.0f); // Horizontal distance, the magnitude

	// camera.position.x = horizontalDistance * cosf(horizontalAngle * PI / 180.0f); // Calculate the position of camera.position x based on distance etc..
	// camera.position.z = horizontalDistance * sinf(horizontalAngle * PI / 180.0f);
	// camera.position.y = distance * sinf(verticalAngle * PI / 180.0f);

	// Vector3 selectedTarget = cubePosition;

	Model model = LoadModel("../resources/Testarossa/Testarossa.gltf");

	float pitch = 0.0f;
    float roll  = 0.0f; 
    float yaw   = 0.0f;

	float steering_angle = 0.0f;
	float steering_angle_speed = 0.1f;
	float throttle       = 0.0f;
	float throttle_speed = 0.1f;
	float turning_radius = 0.0f;


	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// if (IsKeyDown(KEY_A)) steering_angle = fmod(steering_angle + steering_angle_speed, MAX_STEERING_ANGLE);
        // else if (IsKeyDown(KEY_D)) steering_angle -= steering_angle_speed/4.0f;
        // else
        // {
        //     if (steering_angle > steering_angle_speed/2.0f) steering_angle -= steering_angle_speed/2.0f;
        //     else if (steering_angle < -steering_angle_speed/2.0f) steering_angle += steering_angle_speed/2.0f;
        // }

		// if (IsKeyDown(KEY_W)) throttle = fmod(throttle + throttle_speed, MAX_THROTTLE);
        // else if (IsKeyDown(KEY_D)) throttle -= throttle_speed/4.0;
        // else
        // {
        //     if (throttle > throttle_speed/2.0f) throttle -= throttle_speed/2.0f;
        //     else if (throttle < -throttle_speed/2.0f) throttle += throttle_speed/2.0f;
        // }

		// if(steering_angle < 0.001f && steering_angle >= 0.0f)
		// 	turning_radius = MAX_TURNING_RADIUS;
		// else if (steering_angle > -0.001f && steering_angle <= 0.0f)
		// 	turning_radius = MAX_TURNING_RADIUS;
		// else
		// 	turning_radius = steering_angle / VEHICLE_LENGTH; // Pure Ackermann

		// if (IsKeyDown(KEY_W)) {
		// 	vechicle_position.x += turning_radius * sin(steering_angle) * throttle; 
		// 	vechicle_position.z += turning_radius * (1.0f - cos(steering_angle)) * throttle;
		// }

		// if (IsKeyDown(KEY_S)) {
		// 	vechicle_position.x -= turning_radius * sin(steering_angle) * throttle; 
		// 	vechicle_position.z -= turning_radius * (1.0f - cos(steering_angle)) * throttle;
		// }	







        // if (IsKeyDown(KEY_S)) selectedTarget.z -= 0.1f; // Decelerate
        // if (IsKeyDown(KEY_W)) selectedTarget.z += 0.1f; // Accelerate

        // if (IsKeyDown(KEY_D)) horizontalAngle -= 0.1f;  // Steer
        // if (IsKeyDown(KEY_S)) selectedTarget.z -= 0.1f; // Decelerate
        // if (IsKeyDown(KEY_W)) selectedTarget.z += 0.1f; // Accelerate

		// if (IsKeyDown(KEY_A)){
		// 	camera.position.x = horizontalDistance * cosf(horizontalAngle * PI / 180.0f);
		// 	camera.position.z = horizontalDistance * sinf(horizontalAngle * PI / 180.0f);
		// 	horizontalAngle -= 1.0f;
		// }

		// else if (IsKeyDown(KEY_D)){
		// 	camera.position.x = horizontalDistance * cosf(horizontalAngle * PI / 180.0f);
		// 	camera.position.z = horizontalDistance * sinf(horizontalAngle * PI / 180.0f);
		// 	horizontalAngle += 1.0f;
		// }

		// else if (IsKeyDown(KEY_W) && verticalAngle < 88.0f){
		// 	horizontalDistance = distance * cosf(verticalAngle * PI / 180.0f);
		// 	camera.position.x = horizontalDistance * cosf(horizontalAngle * PI / 180.0f);
		// 	camera.position.z = horizontalDistance * sinf(horizontalAngle * PI / 180.0f);
		// 	camera.position.y = distance * sinf(verticalAngle * PI / 180.0f);
		// 	verticalAngle += 1.0f;
		// 	selectedTarget.z += 0.1f;
		// }

		// else if (IsKeyDown(KEY_S) && verticalAngle > 2.0f){
		// 	horizontalDistance = distance * cosf(verticalAngle * PI / 180.0f);
		// 	camera.position.x = horizontalDistance * cosf(horizontalAngle * PI / 180.0f);
		// 	camera.position.z = horizontalDistance * sinf(horizontalAngle * PI / 180.0f);
		// 	camera.position.y = distance * sinf(verticalAngle * PI / 180.0f);
		// 	verticalAngle -= 1.0f;
		// 	selectedTarget.z -= 0.1f;
		// }

		// if (IsKeyDown(KEY_RIGHT)){
		// 	camera.position.x += 1.0f;
		// 	selectedTarget.x += 1.0f;
		// }

		// else if (IsKeyDown(KEY_LEFT)){
		// 	camera.position.x -= 1.0f;
		// 	selectedTarget.x -= 1.0f;
		// }

		// if (IsKeyDown(KEY_UP)){
		// 	camera.position.z -= 1.0f;
		// 	selectedTarget.z -= 1.0f;
		// }

		// else if (IsKeyDown(KEY_DOWN)){
		// 	camera.position.z += 1.0f;
		// 	selectedTarget.z += 1.0f;
		// }

		// if (IsKeyPressed(KEY_SPACE)){
		// 	// cubeList.push_back(selectedTarget);
		// }

		// Move model
        if (IsKeyDown(KEY_A)) vechicle_position.x += 0.1f;
        if (IsKeyDown(KEY_D)) vechicle_position.x -= 0.1f;
        if (IsKeyDown(KEY_S)) vechicle_position.z -= 0.1f;
        if (IsKeyDown(KEY_W)) vechicle_position.z += 0.1f;
		if (IsKeyDown(KEY_E)) {yaw += 0.1f; vechicle_position.x -= 0.01f;};
		if (IsKeyDown(KEY_Q)) {yaw -= 0.1f; vechicle_position.x += 0.01f;};

		Matrix rot_matrix = MatrixRotateXYZ((Vector3){ DEG2RAD*pitch, DEG2RAD*yaw, DEG2RAD*roll });
		Vector3 new_cam_position = Vector3Transform(Vector3{0, 10.0f, -10.0f}, rot_matrix);
		Vector3 new_pos = Vector3Add(new_cam_position, vechicle_position);

		camera.target   = vechicle_position;
		camera.position = new_pos;

		model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch-90.0f), DEG2RAD*yaw, DEG2RAD*roll });;

		BeginDrawing();
		ClearBackground(SKYBLUE);

		BeginMode3D(camera);

		// grid of cube trees on a plane to make a "world"
		DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE); // simple world plane

		// DrawCubeTexture(tx, Vector3{ x, y, z }, 1, 1, 1, GREEN);
		DrawModel(model, (Vector3){ vechicle_position.x, vechicle_position.y, vechicle_position.z }, 1.0f, WHITE);   // Draw 3d model with texture

		DrawGrid(100, 1.0f);

		EndMode3D();

		DrawFPS(0, 0);
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}