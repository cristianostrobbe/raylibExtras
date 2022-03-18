#pragma once
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