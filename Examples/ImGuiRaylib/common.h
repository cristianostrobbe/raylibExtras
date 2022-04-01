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

#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"

#include "FPCamera.h"

class DocumentWindow
{
public:
	bool Open = false;

	RenderTexture ViewTexture;

    virtual void Setup() = 0;
    virtual void Shutdown() = 0;
	virtual void Show() = 0;
	virtual void Update() = 0;

	bool Focused = false;
};