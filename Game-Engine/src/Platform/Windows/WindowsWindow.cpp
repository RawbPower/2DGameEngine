#include "rbpch.h"
#include "WindowsWindow.h"

#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Events/MouseEvent.h"
#include "GameEngine/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Rhombus {

	static bool s_GLFWInitialized = false;

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	static void GLFWErrorCallback(int error, const char* description)
	{
		RB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowParams& params)
	{
		return new WindowsWindow(params);
	}

	WindowsWindow::WindowsWindow(const WindowParams& params)
	{
		Init(params);

		//The window we'll be rendering to
		m_SDLWindow = NULL;

		//The surface contained by the window
		SDL_Surface* screenSurface = NULL;

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			RB_CORE_ERROR("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Create window
			m_SDLWindow = SDL_CreateWindow(m_Data.Title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Data.Width, m_Data.Height, SDL_WINDOW_SHOWN);
			if (m_SDLWindow == NULL)
			{
				RB_CORE_ERROR("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get window surface
				screenSurface = SDL_GetWindowSurface(m_SDLWindow);

				//Fill the surface white
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 55, 55, 55));

				//Update the surface
				SDL_UpdateWindowSurface(m_SDLWindow);
			}
		}
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();

		//Destroy window
		SDL_DestroyWindow(m_SDLWindow);

		//Quit SDL subsystems
		SDL_Quit();
	}

	void WindowsWindow::Init(const WindowParams& params)
	{
		m_Data.Title = params.Title;
		m_Data.Width = params.Width;
		m_Data.Height = params.Height;

		RB_CORE_INFO("Creating window {0} ({1}, {2})", params.Title, params.Width, params.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			RB_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)params.Width, (int)params.Height, m_Data.Title.c_str(), nullptr, nullptr);
		// This could be changed from OpenGL to DirectX or Vulcan in the future
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		// A pointer to our Windows data struct
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
		
		// Set GLFW callbacks - using a lambda function
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			// The prefix asterix dereferences
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			RB_CORE_WARN("{0}, {1}", width, height);
			// Dispatch it
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}