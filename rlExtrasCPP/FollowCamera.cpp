/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   LICENSE: MIT
*
*   Copyright (c) 2022 Cristiano Strobbe
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

#include "FollowCamera.h"

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include <cmath>

FollowCamera::FollowCamera() : ControlsKeys{ 'W', 'S', 'D', 'A', 'E', 'Q', KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LEFT_SHIFT }
{

}

void FollowCamera::Setup(float fovY, Vector3& position, Vector3& relative_position, Vector3& angles)
{
    RelativePosition = relative_position;
    rot_matrix = MatrixRotateXYZ((Vector3){ DEG2RAD*angles.x, DEG2RAD*angles.y, DEG2RAD*angles.z });
    Vector3 rotated_vec_position = Vector3Transform(RelativePosition, rot_matrix);
    Vector3 new_cam_position = Vector3Add(rotated_vec_position, position);
    
    // Camera setup
    ViewCamera.position = new_cam_position;
    ViewCamera.target   = position;
    ViewCamera.up = { 0.0f, 1.0f, 0.0f };
    ViewCamera.fovy = fovY;
    ViewCamera.projection = CAMERA_PERSPECTIVE;

    Focused = IsWindowFocused();
    if (HideCursor && Focused && (UseMouseX || UseMouseY))
        DisableCursor();

    // TargetDistance = 1;

    ViewResized();
    PreviousMousePosition = GetMousePosition();
}

void FollowCamera::ViewResized()
{
    float width = (float)GetScreenWidth();
    float height = (float)GetScreenHeight();

    FOV.y = ViewCamera.fovy;

    if (height != 0)
        FOV.x = FOV.y * (width / height);
}

float FollowCamera::GetSpeedForAxis(CameraControls axis, float speed)
{
    if (!UseKeyboard)
        return 0;

    int key = ControlsKeys[axis];
    if (key == -1)
        return 0;

    float factor = 1.0f;
    if (IsKeyDown(ControlsKeys[SPRINT]))
        factor = 2;

    if (IsKeyDown(ControlsKeys[axis]))
        return speed * GetFrameTime() * factor;

    return 0.0f;
}

void FollowCamera::BeginMode3D()
{
    float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();

    rlDrawRenderBatchActive();			// Draw Buffers (Only OpenGL 3+ and ES2)
    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlPushMatrix();                     // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();                   // Reset current matrix (projection)

    if (ViewCamera.projection == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        double top = RL_CULL_DISTANCE_NEAR * tan(ViewCamera.fovy * 0.5 * DEG2RAD);
        double right = top * aspect;

        rlFrustum(-right, right, -top, top, NearPlane, FarPlane);
    }
    else if (ViewCamera.projection == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        double top = ViewCamera.fovy / 2.0;
        double right = top * aspect;

        rlOrtho(-right, right, -top, top, NearPlane, FarPlane);
    }

    // NOTE: zNear and zFar values are important when computing depth buffer values

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)

    // Setup Camera view
    Matrix matView = MatrixLookAt(ViewCamera.position, ViewCamera.target, ViewCamera.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();                // Enable DEPTH_TEST for 3D
    ExtractFrustum();
}

void FollowCamera::EndMode3D()
{
    ::EndMode3D();
}

void FollowCamera::Update(Vector3& position, Vector3& angles)
{
    rot_matrix = MatrixRotateXYZ((Vector3){ DEG2RAD*angles.x, DEG2RAD*angles.y, DEG2RAD*angles.z });
    Vector3 rotated_vec_position = Vector3Transform(RelativePosition, rot_matrix);
    Vector3 new_cam_position = Vector3Add(rotated_vec_position, position);
    ViewCamera.position = new_cam_position;
    ViewCamera.target = position;

    if (HideCursor && IsWindowFocused() != Focused && (UseMouseX || UseMouseY))
    {
        Focused = IsWindowFocused();
        if (Focused)
        {
            DisableCursor();
            PreviousMousePosition = GetMousePosition(); // so there is no jump on focus
        }
        else
        {
            EnableCursor();
        }
    }

    // Mouse movement detection
    Vector2 mousePositionDelta = { 0.0f, 0.0f };
    Vector2 mousePosition = GetMousePosition();
    float mouseWheelMove = GetMouseWheelMove();
}

float FollowCamera::GetFOVX() const
{
    return FOV.x;
}

Vector3 FollowCamera::GetCameraPosition() const
{
    return CameraPosition;
}

void FollowCamera::SetCameraPosition(const Vector3&& pos)
{ 
    CameraPosition = pos;
    Vector3 forward = Vector3Subtract(ViewCamera.target, ViewCamera.position);
    ViewCamera.position = CameraPosition;
    ViewCamera.target = Vector3Add(CameraPosition, forward);
}

void FollowCamera::SetCameraRelativePosition(Vector3& rel_pos)
{ 
    RelativePosition = rel_pos;
}