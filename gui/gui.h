#pragma once
#include "../External/ImGui/imgui.h"
#include "../External/ImGui/imgui_impl_glfw.h"
#include "../External/ImGui/imgui_impl_opengl3.h"

namespace GUI
{
	int Setup(int (*OnGui)());
    int RenderFrame();
    void ShutDown();

    inline GLFWwindow* window;
}
