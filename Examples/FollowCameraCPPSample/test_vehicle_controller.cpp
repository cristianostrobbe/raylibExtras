// From: https://www.reddit.com/r/raylib/comments/gdqy0h/some_maths_about_camera_in_3d/

#include "raylib.h"
// #include "rcamera.h"
#include "FollowCamera.h"
#include "raymath.h"
#include "math.h"
#include "rlgl.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#define MAX_STEERING_ANGLE 0.8f // rad
#define MAX_THROTTLE       1.0f // #
#define VEHICLE_LENGTH     2.5f // m
#define MAX_TURNING_RADIUS 10000000000000.0f // m

#define MAX_POSTPRO_SHADERS         8

typedef enum {
    FX_BASE = 0,
    FX_LIGHTING,
    FX_MASK,
    FX_PREDATOR,
    FX_RAYMARCHING,
    FX_RELOAD,
    FX_SCANLINES,
    FX_SPOTLIGHT,
} PostproShader;

static const char *postproShaderText[] = {
    "FX_BASE",
    "FX_LIGHTING",
    "FX_MASK",
    "FX_PREDATOR",
    "FX_RAYMARCHING",
    "FX_RELOAD",
    "FX_SCANLINES",
    "FX_SPOTLIGHT",
};

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
	Vector3 relative_position = { 0, 5.0f, -15.0f };
	Vector3 euler_angles      = { 0, 0.0f, 0.0f }; // pitch , yaw, roll

	FollowCamera cam;
	cam.Setup(45, vechicle_position, relative_position, euler_angles);
	cam.HideCursor = false;
	cam.FarPlane   = 5000;

	Model model = LoadModel("../resources/Testarossa/Testarossa.gltf");
	
	// Shader shaders[MAX_POSTPRO_SHADERS] = { 0 };

	// shaders[FX_BASE] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/base.fs", GLSL_VERSION));
    // shaders[FX_LIGHTING] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/lighting.fs", GLSL_VERSION));
    // shaders[FX_MASK] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/mask.fs", GLSL_VERSION));
    // shaders[FX_PREDATOR] = LoadShader(0, TextFormat("../resources/Testarossa/shadersporedator.fs", GLSL_VERSION));
    // shaders[FX_RAYMARCHING] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/raymarching.fs", GLSL_VERSION));
    // shaders[FX_RELOAD] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/reload.fs", GLSL_VERSION));
    // shaders[FX_SCANLINES] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/scanlines.fs", GLSL_VERSION));
    // shaders[FX_SPOTLIGHT] = LoadShader(0, TextFormat("../resources/Testarossa/shaders/spotlight.fs", GLSL_VERSION));
	
	// int currentShader = FX_BASE;
	// model.materials[0].shader = shaders[currentShader];  // Set shader effect to 3d model
	// // Create a RenderTexture2D to be used for render to texture
    // RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

	float pitch = 0.0f;
    float roll  = 0.0f; 
    float yaw   = 0.0f;

	float steering_angle = 0.0f;
	float steering_angle_speed = 0.01f;
	float throttle       = 0.0f;
	float throttle_speed = 0.1f;
	float turning_radius = 0.0f;
	float max_angular_speed = 1.0f;

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		// -------------------------------------------------------------------- //
		// if (IsKeyPressed(KEY_M)) currentShader++;
        // else if (IsKeyPressed(KEY_N)) currentShader--;

        // if (currentShader >= MAX_POSTPRO_SHADERS) currentShader = 0;
        // else if (currentShader < 0) currentShader = MAX_POSTPRO_SHADERS - 1;
		// -------------------------------------------------------------------- //

		if (IsKeyDown(KEY_A)) {
			steering_angle += steering_angle_speed;
			if(steering_angle > MAX_STEERING_ANGLE){
				steering_angle = MAX_STEERING_ANGLE;
			}
		}
        else if (IsKeyDown(KEY_D)){
			steering_angle -= steering_angle_speed;
			if(steering_angle < -1.0f * MAX_STEERING_ANGLE){
				steering_angle = -1.0f * MAX_STEERING_ANGLE;
			}
		}

		if (IsKeyDown(KEY_W)) {throttle = fmod(throttle + throttle_speed, MAX_THROTTLE);}
        else
        {
            if (throttle > throttle_speed/2.0f) throttle -= throttle_speed/2.0f;
            // else if (throttle < -1.0f*throttle_speed/2.0f) throttle += throttle_speed/2.0f;
        }

		if(steering_angle < steering_angle_speed && steering_angle >= 0.0f)
			turning_radius = MAX_TURNING_RADIUS;
		else if (steering_angle > steering_angle_speed && steering_angle <= 0.0f)
			turning_radius = -1.0f * MAX_TURNING_RADIUS;
		else
			turning_radius = VEHICLE_LENGTH / steering_angle; // Pure Ackermann

		float angular_step = (steering_angle/30.0f) * throttle;
		// float angular_step = steering_angle;
		if (IsKeyDown(KEY_W)) {
			vechicle_position.x += turning_radius * (1.0f - cos(angular_step));
			vechicle_position.z += turning_radius * sin(angular_step); 
			if (turning_radius > 0.0f) {
				yaw += angular_step;
			}else{
				yaw -= angular_step;
			}
		}

		Matrix rot_matrix = MatrixRotateXYZ((Vector3){ DEG2RAD*pitch, DEG2RAD*yaw, DEG2RAD*roll });
		Vector3 new_cam_position = Vector3Transform(Vector3{0, 10.0f, -10.0f}, rot_matrix);
		Vector3 new_pos = Vector3Add(new_cam_position, vechicle_position);

		euler_angles = {pitch, yaw, roll};
		cam.Update(vechicle_position, euler_angles);

		model.transform = MatrixRotateXYZ((Vector3){ DEG2RAD*(pitch-90.0f), DEG2RAD*yaw, DEG2RAD*roll });

		BeginDrawing();
			ClearBackground(SKYBLUE);

			cam.BeginMode3D();

			// Ground plane
			DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 1000, 1000 }, BROWN);

			DrawModel(model, (Vector3){ vechicle_position.x, vechicle_position.y, vechicle_position.z }, 1.0f, WHITE);   // Draw 3d model with texture

			DrawText(TextFormat("Turning radius: %f Steering angle: %f", turning_radius, steering_angle), 10, 10, 10, BLACK); 

			// Draw turning radius
			// DrawCircleSector(Vector2{vechicle_position.x, vechicle_position.z}, turning_radius, float startAngle, float endAngle, int segments, Color color);
			DrawCircle3D(Vector3{vechicle_position.x+turning_radius, vechicle_position.y, vechicle_position.z}, turning_radius, Vector3{1.0f, 0.0f, 0.0f}, 90.0f, RED);  
			
			DrawGrid(1000, 1.0f);

			cam.EndMode3D();

			DrawFPS(0, 0);
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

    // UnloadTexture(texture);         // Unload texture
    UnloadModel(model);             // Unload model
    // UnloadRenderTexture(target);    // Unload render texture

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}