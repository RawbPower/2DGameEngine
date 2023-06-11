#include "rbpch.h"
#include "WindowsInput.h"

// It's fine to this as we a inside a platform specific file
#include "Rhombus/Core/Application.h"
#include <GLFW/glfw3.h>
#include <SDL.h>

namespace rhombus 
{

	Input* Input::s_Instance = new WindowsInput();

	bool  WindowsInput::IsKeyPressedImpl(int keycode)
	{
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
						
		return currentKeyStates[SDL_GetScancodeFromKey((SDL_Keycode)keycode)];
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();

		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();

		return y;
	}

}