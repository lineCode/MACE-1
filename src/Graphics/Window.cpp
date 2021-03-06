/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Constants.h>
#include <MACE/Core/System.h>

#define MACE_EXPOSE_GLFW
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/OGL33Renderer.h>
#include <MACE/Graphics/OGL/OGL33Context.h>

#include <mutex>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <sstream>

//so we can use glew
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace mc {
	namespace gfx {
		namespace {
			std::unordered_map< short int, Byte > keys = std::unordered_map< short int, Byte >();

			int mouseX = -1;
			int mouseY = -1;

			double scrollX = 0;
			double scrollY = 0;

			void pushKeyEvent(const short int& key, const Byte action) {
				keys[key] = action;
			}

			GLFWwindow* createWindow(const WindowModule::LaunchConfig& config) {
				if (config.fullscreen) {
					GLFWmonitor* mon = glfwGetPrimaryMonitor();

					const GLFWvidmode* mode = glfwGetVideoMode(mon);
					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

					return glfwCreateWindow(mode->width, mode->height, config.title, mon, nullptr);
				} else {
					return glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
				}
			}

			///GLFW callback functions

			void onGLFWError(int id, const char* desc) {
				if (id == GLFW_NOT_INITIALIZED) {
					MACE__THROW(InvalidState, "Windowing manager was not initialized/initialization was invalid: NOT_INITIALIZED: " + std::string(desc));
				} else if (id == GLFW_NO_CURRENT_CONTEXT) {
					MACE__THROW(NoRendererContext, "No Renderer context in this thread: NO_CURRENT_CONTEXT: " + std::string(desc));
				} else if (id == GLFW_FORMAT_UNAVAILABLE) {
					MACE__THROW(BadFormat, "The system does not support the required format: FORMAT_UNAVAIBLE: " + std::string(desc));
				} else if (id == GLFW_NO_WINDOW_CONTEXT) {
					MACE__THROW(NoRendererContext, "No window in this thread: NO_WINDOW_CONTEXT: " + std::string(desc));
				} else if (id == GLFW_API_UNAVAILABLE) {
					MACE__THROW(UnsupportedRenderer, "Context unavailable on this system: API_UNAVAILABLE" + std::string(desc));
				} else if (id == GLFW_VERSION_UNAVAILABLE) {
					MACE__THROW(UnsupportedRenderer, "Context unavailable on this system: VERSION_UNAVAILABLE: " + std::string(desc));
				} else if (id == GLFW_PLATFORM_ERROR) {
					MACE__THROW(System, "An error occured in the window platform: PLATFORM_ERROR: " + std::string(desc));
				} else if (id == GLFW_OUT_OF_MEMORY) {
					MACE__THROW(OutOfMemory, "Out of memory: OUT_OF_MEMORY: " + std::string(desc));
				} else {
					MACE__THROW(Window, desc);
				}
			}

			void onWindowClose(GLFWwindow* window) {
				WindowModule* mod = convertGLFWWindowToModule(window);
				mod->makeDirty();

				const WindowModule::LaunchConfig& config = mod->getLaunchConfig();
				config.onClose(*mod);

				if (config.terminateOnClose) {
					mod->getInstance()->requestStop();
				}
			}

			void onWindowKeyButton(GLFWwindow*, int key, int, int action, int mods) {
				Byte actions = 0x00;
				if (action == GLFW_PRESS) {
					actions |= Input::PRESSED;
				}
				if (action == GLFW_REPEAT) {
					actions |= Input::REPEATED;
				}
				if (action == GLFW_RELEASE) {
					actions |= Input::RELEASED;
				}
				if (mods & GLFW_MOD_SHIFT) {
					actions |= Input::MODIFIER_SHIFT;
				}
				if (mods & GLFW_MOD_CONTROL) {
					actions |= Input::MODIFIER_CONTROL;
				}
				if (mods & GLFW_MOD_ALT) {
					actions |= Input::MODIFIER_ALT;
				}
				if (mods & GLFW_MOD_SUPER) {
					actions |= Input::MODIFIER_SUPER;
				}

				pushKeyEvent(static_cast<short int>(key), actions);
			}

			void onWindowMouseButton(GLFWwindow*, int button, int action, int mods) {
				Byte actions = 0x00;
				if (action == GLFW_PRESS) {
					actions |= Input::PRESSED;
				}
				if (action == GLFW_REPEAT) {
					actions |= Input::REPEATED;
				}
				if (action == GLFW_RELEASE) {
					actions |= Input::RELEASED;
				}
				if (mods & GLFW_MOD_SHIFT) {
					actions |= Input::MODIFIER_SHIFT;
				}
				if (mods & GLFW_MOD_CONTROL) {
					actions |= Input::MODIFIER_CONTROL;
				}
				if (mods & GLFW_MOD_ALT) {
					actions |= Input::MODIFIER_ALT;
				}
				if (mods & GLFW_MOD_SUPER) {
					actions |= Input::MODIFIER_SUPER;
				}

				//in case that we dont have it mapped the same way that GLFW does, we add MOUSE_FIRST which is the offset to the mouse bindings.
				pushKeyEvent(static_cast<short int>(button) + Input::MOUSE_FIRST, actions);
			}

			void onWindowCursorPosition(GLFWwindow* window, double xpos, double ypos) {
				mouseX = static_cast<int>(mc::math::floor(xpos));
				mouseY = static_cast<int>(mc::math::floor(ypos));

				WindowModule* win = convertGLFWWindowToModule(window);
				win->getLaunchConfig().onMouseMove(*win, mouseX, mouseY);
			}

			void onWindowScrollWheel(GLFWwindow* window, double xoffset, double yoffset) {
				scrollY = yoffset;
				scrollX = xoffset;

				WindowModule* win = convertGLFWWindowToModule(window);
				win->getLaunchConfig().onScroll(*win, scrollX, scrollY);
			}

			void onWindowFramebufferResized(GLFWwindow* window, int, int) {
				WindowModule* win = convertGLFWWindowToModule(window);
				win->getContext()->getRenderer()->flagResize();
				win->makeChildrenDirty();
			}

			void onWindowDamaged(GLFWwindow* window) {
				convertGLFWWindowToModule(window)->makeDirty();
			}
		}//anon namespace

		WindowModule::WindowModule(const LaunchConfig& c) : config(c) {}

		void WindowModule::create() {
			glfwSetErrorCallback(&onGLFWError);

			if (!glfwInit()) {
				MACE__THROW(InitializationFailed, "GLFW failed to initialize!");
			}

			os::clearError(__LINE__, __FILE__);//glfwInit sometimes sets LastError to non-zero. we can ignore it

			//just in case someone changed some before creating the window module
			glfwDefaultWindowHints();

			switch (config.contextType) {
				case Enums::ContextType::AUTOMATIC:
				case Enums::ContextType::BEST_OGL:
				case Enums::ContextType::OGL33:
				default:
					context = std::unique_ptr<gfx::GraphicsContext>(new gfx::ogl::OGL33Context(this));

					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

					glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

					glfwWindowHint(GLFW_STENCIL_BITS, 8);
					glfwWindowHint(GLFW_DEPTH_BITS, GLFW_DONT_CARE);
#ifdef MACE_DEBUG_OPENGL
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
					break;
			}

#ifdef MACE_DEBUG_INTERNAL_ERRORS
			if (context == nullptr) {
				MACE__THROW(UnsupportedRenderer, "Internal Error: GraphicsContext is nullptr");
			}
#endif

			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_DECORATED, config.decorated);

			window = createWindow(config);

			if (config.contextType == Enums::ContextType::BEST_OGL || config.contextType == Enums::ContextType::AUTOMATIC) {
				int versionMajor = 3;

				//this checks every available context until 1.0. the window is hidden so it won't cause spazzing
				for (int versionMinor = 3; versionMinor >= 0 && !window; --versionMinor) {
					std::cout << os::consoleColor(os::ConsoleColor::YELLOW) << "Trying OpenGL ";
					std::cout << os::consoleColor(os::ConsoleColor::LIGHT_YELLOW) << versionMajor << "." << versionMinor << std::endl << os::consoleColor();
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);

					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

					window = createWindow(config);
					if (versionMinor == 0 && versionMajor > 1 && !window) {
						glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
						versionMinor = 3;
						--versionMajor;
					}
				}
			}

			if (window == nullptr) {
				MACE__THROW(InitializationFailed, "OpenGL context was unable to be created. This graphics card may not be supported or the graphics drivers are installed incorrectly");
			}
		}//create 

		void WindowModule::configureThread() {
			glfwMakeContextCurrent(window);

			context->init();

			if (config.vsync) {
				glfwSwapInterval(1);
			} else {
				glfwSwapInterval(0);
			}

			glfwSetWindowUserPointer(window, this);

			glfwSetWindowCloseCallback(window, &onWindowClose);
			glfwSetKeyCallback(window, &onWindowKeyButton);
			glfwSetMouseButtonCallback(window, &onWindowMouseButton);
			glfwSetCursorPosCallback(window, &onWindowCursorPosition);
			glfwSetScrollCallback(window, &onWindowScrollWheel);

			glfwSetFramebufferSizeCallback(window, &onWindowFramebufferResized);
			glfwSetWindowRefreshCallback(window, &onWindowDamaged);

			//int width = 0, height = 0;

			//glfwGetFramebufferSize(window, &width, &height);

			//if (width != config.width || height != config.height) {
			//	context->getRenderer()->resize(this, width, height);
			//}
			context->getRenderer()->resize(this, config.width, config.height);

			config.onCreate(*this);
		}

		const WindowModule::LaunchConfig & WindowModule::getLaunchConfig() const {
			return config;
		}

		void WindowModule::setTitle(const std::string & newTitle) {
			if (!getProperty(gfx::Entity::INIT)) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			glfwSetWindowTitle(window, newTitle.c_str());
		}

		void WindowModule::threadCallback() {
			try {
				os::clearError(__LINE__, __FILE__);

				//typedefs for chrono for readibility purposes
				using Clock = std::chrono::steady_clock;
				using TimeStamp = std::chrono::time_point<Clock>;
				using Duration = std::chrono::microseconds;

				//mutex for this function.
				std::mutex mutex;

				//now is set to be now() every loop, and the delta is calculated from now and last frame.
				TimeStamp now = Clock::now();
				//each time the frame is swapped, lastFrame is updated with the new time
				TimeStamp lastFrame = Clock::now();

				//this stores how many milliseconds it takes for the frame to swap.
				Duration windowDelay = Duration::zero();

				try {
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

					configureThread();

					Entity::init();

					if (config.fps != 0) {
						windowDelay = Duration(std::chrono::seconds(1)) / static_cast<long long>(config.fps);
					}

					os::clearError(__LINE__, __FILE__);
				} catch (const std::exception& e) {
					Error::handleError(e, instance);
				} catch (...) {
					MACE__THROW(Unknown, "An unknown error has occured trying to initalize MACE");
				}

				//this is the main rendering loop.
				//we loop infinitely until break is called. break is called when an exception is thrown or MACE::isRunning is false
				for (;;) {//( ;_;)
					try {
						{
							//thread doesn't own window, so we have to lock the mutex
							const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex

							if (getProperty(Entity::DIRTY)) {
								context->getRenderer()->setUp(this);
								Entity::render();
								context->getRenderer()->tearDown(this);
							}

							context->render();

							if (!instance->isRunning()) {
								break; // while (!MACE::isRunning) would require a lock on destroyed or have it be an atomic varible, both of which are undesirable. while we already have a lock, set a stack variable to false.that way, we only read it, and we dont need to always lock it
							}

						}

						if (windowDelay != Duration::zero()) {
							now = Clock::now();

							std::this_thread::sleep_for(windowDelay - (now - lastFrame));

							lastFrame = Clock::now();
						}
					} catch (const std::exception& e) {
						Error::handleError(e, instance);
						break;
					} catch (...) {
						MACE__THROW(Unknown, "An unknown error occured trying to render a frame");
					}
				}

				os::checkError(__LINE__, __FILE__, "A system error occurred during the window loop");

				{
					const std::unique_lock<std::mutex> guard(mutex);//in case there is an exception, the unique lock will unlock the mutex
					try {
						Entity::destroy();

						context->destroy();

						//the window will be destroyed on the main thread, need to detach this one
						glfwMakeContextCurrent(nullptr);

						os::checkError(__LINE__, __FILE__, "A system error occurred destroying the window");
					} catch (const std::exception& e) {
						Error::handleError(e, instance);
					} catch (...) {
						MACE__THROW(Unknown, "An unknown error occured trying to destroy the rendering thread");
					}
				}

				os::checkError(__LINE__, __FILE__, "A system error occured while running MACE");
			} catch (const std::exception& e) {
				Error::handleError(e, instance);
			} catch (...) {
				Error::handleError(MACE__GET_ERROR_NAME(Unknown) ("An unknown error occured while running MACE", __LINE__, __FILE__), instance);
			}
		}//threadCallback

		void WindowModule::init() {
			//GLFW needs to be created from main thread
			//we create a window in the main thread, then switch its context to the render thread
			create();
			
			os::clearError(__LINE__, __FILE__);//sometimes an error comes from GLFW that we can ignore

			//release context on this thread, it wll be owned by the seperate rendering thread
			glfwMakeContextCurrent(nullptr);

			windowThread = std::thread(&WindowModule::threadCallback, this);

			os::checkError(__LINE__, __FILE__, "A system error occured while trying to init the WindowModule");
		}

		void WindowModule::update() {
			std::mutex mutex;
			const std::unique_lock<std::mutex> guard(mutex);

			glfwPollEvents();

			Entity::update();
		}//update

		void WindowModule::destroy() {
			{
				std::mutex mutex;
				const std::unique_lock<std::mutex> guard(mutex);
				setProperty(WindowModule::DESTROYED, true);
			}

			windowThread.join();

			os::checkError(__LINE__, __FILE__, "A system error occured while trying to destroy the WindowModule");

			glfwDestroyWindow(window);
			glfwMakeContextCurrent(nullptr);

			glfwTerminate();
			os::clearError(__LINE__, __FILE__);//https://github.com/glfw/glfw/issues/1053
		}//destroy

		std::string WindowModule::getName() const {
			return "MACE/Window#" + std::string(config.title);
		}//getName()

		bool WindowModule::isDestroyed() const {
			return properties & WindowModule::DESTROYED;
		}

		Vector<int, 2> WindowModule::getFramebufferSize() const {
			if (!getProperty(gfx::Entity::INIT)) {
				MACE__THROW(InvalidState, "WindowModule not initialized! Must call MACE::init() first!");
			}

			Vector<int, 2> out = {};

			glfwGetFramebufferSize(window, &out[0], &out[1]);

			return out;
		}

		GraphicsContext* WindowModule::getContext() {
			return context.get();
		}

		const GraphicsContext* WindowModule::getContext() const {
			return context.get();
		}

		void WindowModule::onInit() {}

		void WindowModule::onUpdate() {}

		void WindowModule::onRender() {}

		void WindowModule::onDestroy() {}

		void WindowModule::clean() {
			setProperty(Entity::DIRTY, false);
		}//clean()

		namespace Input {
			const Byte & getKey(const short int key) {
				return keys[key];
			}

			bool isKeyDown(const short int key) {
				return keys[key] & Input::PRESSED || keys[key] & Input::REPEATED;
			}

			bool isKeyRepeated(const short int key) {
				return keys[key] & Input::REPEATED;
			}

			bool isKeyReleased(const short int key) {
				return keys[key] & Input::RELEASED;
			}
			int getMouseX() noexcept {
				return mouseX;
			}
			int getMouseY() noexcept {
				return mouseY;
			}
			double getScrollVertical() noexcept {
				return scrollY;
			}
			double getScrollHorizontal() noexcept {
				return scrollX;
			}
		}//Input

		WindowModule::LaunchConfig::LaunchConfig(const int w, const int h, const char * t) : title(t), width(w), height(h) {}

		bool WindowModule::LaunchConfig::operator==(const LaunchConfig & other) const {
			return title == other.title && width == other.width && height == other.height
				&& fps == other.fps && contextType == other.contextType
				&& onCreate == other.onCreate && onClose == other.onClose
				&& onScroll == other.onScroll && onMouseMove == other.onMouseMove
				&& terminateOnClose == other.terminateOnClose
				&& decorated == other.decorated && fullscreen == other.fullscreen
				&& resizable == other.resizable && vsync == other.vsync;
		}

		bool WindowModule::LaunchConfig::operator!=(const LaunchConfig & other) const {
			return !operator==(other);
		}

		WindowModule * getCurrentWindow() {
			GLFWwindow* win = glfwGetCurrentContext();
			if (win == nullptr) {
				MACE__THROW(NoRendererContext, "No renderer context in this thread");
			}

			return convertGLFWWindowToModule(win);
		}

		WindowModule * convertGLFWWindowToModule(GLFWwindow * win) {
			WindowModule* windowModule = static_cast<WindowModule*>(glfwGetWindowUserPointer(win));
			if (windowModule == nullptr) {
				MACE__THROW(NoRendererContext, "No window module found in window");
			}

			return windowModule;
		}

	}//os
}//mc
