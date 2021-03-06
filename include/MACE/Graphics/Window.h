/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_WINDOW_H
#define MACE__GRAPHICS_WINDOW_H

#include <MACE/Core/Instance.h>
#include <MACE/Core/Constants.h>
#include <MACE/Graphics/Entity.h>

#include <thread>
#include <string>

//forward declaration to prevent including glfw.h
struct GLFWwindow;

namespace mc {
	namespace gfx {
		class GraphicsContext;

		/**
		Thrown when GLFW (windowing library) throws an error and no other `Error` subclass is more specific.
		*/
		MACE__DECLARE_ERROR(Window);

		/**
		Thrown when the requested `Renderer` is unavailable on the
		system or it does not support a feature that was used, such
		as a `Painter` function.
		*/
		MACE__DECLARE_ERROR(UnsupportedRenderer);

		/**
		Thrown when a function that requires a renderer context can not
		find one.
		<p>
		Most functions that require a renderer context will be marked in the docs.
		<p>
		All `Entity` callbacks except for Entity::update are called with a renderer
		context.
		
		@remark Due to limitations by graphics APIs, functions that require a `Renderer` context may not throw this error immediately, or the error line and file will not be correct. Using the `gfx::getCurrentWindow()` function will always throw the correct error immediately if no context is found.
		@see gfx::getCurrentWindow()
		@see gfx::Enums::ContextType
		*/
		MACE__DECLARE_ERROR(NoRendererContext);

		namespace Enums {
			/**
			Hints to the `WindowModule` which `GraphicsContext` to create.
			
			@remark Some contexes are not available on certain platforms. An `UnsupportedRendererError` is thrown in these cases.
			@see NoRendererContextError
			*/
			enum class ContextType {
				AUTOMATIC,
				BEST_OGL,
				OGL33
			};
		}

		/**
		@todo fix fps timer
		*/
		class WindowModule: public Module, public gfx::Entity {
		public:
			struct LaunchConfig {
				typedef void(*WindowCallback)(WindowModule& window);
				typedef void(*ScrollCallback)(WindowModule& window, double x, double y);
				typedef void(*MouseMoveCallback)(WindowModule& window, int x, int y);

				LaunchConfig(const int w, const int h, const char* t);

				const char* title;
				const int width;
				const int height;

				unsigned int fps = 30;

				Enums::ContextType contextType = Enums::ContextType::AUTOMATIC;

				WindowCallback onCreate = [](WindowModule&) {};
				WindowCallback onClose = [](WindowModule&) {};
				ScrollCallback onScroll = [](WindowModule&, double, double) {};
				MouseMoveCallback onMouseMove = [](WindowModule&, int, int) {};

				/**
				Whether this window should terminate the MACE loop when destroyed
				*/
				bool terminateOnClose = true;

				bool decorated = true;
				bool fullscreen = false;
				bool resizable = false;
				bool vsync = false;

				bool operator==(const LaunchConfig& other) const;
				bool operator!=(const LaunchConfig& other) const;
			};

			WindowModule(const LaunchConfig& config);

#ifdef MACE_EXPOSE_GLFW
			/**
			@internal
			*/
			GLFWwindow* getGLFWWindow() {
				return window;
			}

			/**
			@internal
			*/
			const GLFWwindow* getGLFWWindow() const {
				return window;
			}
#endif//MACE_EXPOSE_GLFW

			const LaunchConfig& getLaunchConfig() const;

			void setTitle(const std::string& newTitle);

			std::string getName() const override;

			bool isDestroyed() const;

			Vector<int, 2> getFramebufferSize() const;

			GraphicsContext* getContext();
			const GraphicsContext* getContext() const;
		private:
			enum Properties: Byte {
				DESTROYED = 0,
			};

			std::thread windowThread;

			GLFWwindow* window;

			const LaunchConfig config;

			Byte properties;

			std::unique_ptr<gfx::GraphicsContext> context;

			void create();

			void configureThread();

			void init() override;
			void update() override;
			void destroy() override;

			void clean() final;

			//these are for the Entity inheritence
			void onUpdate() final;
			void onRender() final;
			void onDestroy() final;
			void onInit() final;

			void threadCallback();
		};//WindowModule

		/**
		Retrieves the `WindowModule` from the `Renderer` context in this thread if exists, throws an error otherwise.

		@opengl
		@return The `WindowModule` in this thread. Will never be `nullptr`.
		@throw NoRendererContext if this thread does not contain a `Renderer` context.
		*/
		WindowModule* getCurrentWindow();

#ifdef MACE_EXPOSE_GLFW
		/**
		Grabs the `WindowModule` that owns a `GLFWwindow` if it exists.
		<p>
		The `GLFWwindow` stores a custom user pointer to the `WindowModule` that created it.
		<p>
		This can be called from any thread.

		@param win `GLFWwindow` to convert. Must not be null. Must have been created by a `WindowModule`
		@return The `WindowModule` that owns `win`. Guarenteed to never be `nullptr`
		@throw NullPointer if `win` is `nullptr`
		@throw NoRendererContext if `win` was not created by a `WindowModule`
		@internal
		*/
		WindowModule* convertGLFWWindowToModule(GLFWwindow* win);
#endif

		namespace Input {
			/*
			the reason we are using not-strong enums is because we want it to expand to a number for GLFW
			Normally, this should be a macro, but those don't follow scope. Weak enums are very similar to macros
			but they follow scope (even though they don't create one themselves)
			*/
			enum Action: Byte {
				PRESSED = 0x01,
				REPEATED = 0x02,
				RELEASED = 0x04,
				MODIFIER_SHIFT = 0x08,
				MODIFIER_CONTROL = 0x10,
				MODIFIER_ALT = 0x20,
				MODIFIER_SUPER = 0x40
			};//Action

			//it is a short int to save memory as no value exceeds MAX_SHORT_INT
			enum Key: short int {
				UNKNOWN = -1,

				MOUSE_1 = 1,

				MOUSE_2 = 2,

				MOUSE_3 = 3,

				MOUSE_4 = 4,

				MOUSE_5 = 5,

				MOUSE_6 = 6,

				MOUSE_7 = 7,

				MOUSE_8 = 8,

				MOUSE_LEFT = MOUSE_1,

				MOUSE_RIGHT = MOUSE_2,

				MOUSE_MIDDLE = MOUSE_3,

				MOUSE_LAST = MOUSE_8,

				MOUSE_FIRST = MOUSE_1,

				//the values for the following keys are equal to their values in GLFW

				//The first keys are direct ASCII mappings
				SPACE = 32,

				APOSTROPHE = 39, /* ' */

				COMMA = 44, /* , */

				MINUS = 45, /* - */

				PERIOD = 46, /* . */

				SLASH = 47, /* / */

				NUM_0 = 48,

				NUM_1 = 49,

				NUM_2 = 50,

				NUM_3 = 51,

				NUM_4 = 52,

				NUM_5 = 53,

				NUM_6 = 54,

				NUM_7 = 55,

				NUM_8 = 56,

				NUM_9 = 57,

				SEMICOLON = 59, /* , */

				EQUAL = 61, /* = */

				A = 65,

				B = 66,

				C = 67,

				D = 68,

				E = 69,

				F = 70,

				G = 71,

				H = 72,

				I = 73,

				J = 74,

				K = 75,

				L = 76,

				M = 77,

				N = 78,

				O = 79,

				P = 80,

				Q = 81,

				R = 82,

				S = 83,

				T = 84,

				U = 85,

				V = 86,

				W = 87,

				X = 88,

				Y = 89,

				Z = 90,

				LEFT_BRACKET = 91, /* [ */

				BACKSLASH = 92, /* \ */

				RIGHT_BRACKET = 93, /* ] */

				GRAVE_ACCENT = 96, /*` */

				WORLD_1 = 161, /* non-US #1 */

				WORLD_2 = 162, /* non-US #2 */

				//non-ASCII characters have values above 255 and are defined as following

				ESCAPE = 256,

				ENTER = 257,

				TAB = 258,

				BACKSPACE = 259,

				INSERT = 260,

				/**
				Delete key. Named DEL to prevent naming conflict with DELETE
				*/
				DEL = 261,

				RIGHT = 262,

				LEFT = 263,

				DOWN = 264,

				UP = 265,

				PAGE_UP = 266,

				PAGE_DOWN = 267,

				HOME = 268,

				END = 269,

				CAPS_LOCK = 280,

				SCROLL_LOCK = 281,

				NUM_LOCK = 282,

				PRINT_SCREEN = 283,

				PAUSE = 284,

				F1 = 290,

				F2 = 291,

				F3 = 292,

				F4 = 293,

				F5 = 294,

				F6 = 295,

				F7 = 296,

				F8 = 297,

				F9 = 298,

				F10 = 299,

				F11 = 300,

				F12 = 301,

				F13 = 302,

				F14 = 303,

				F15 = 304,

				F16 = 305,

				F17 = 306,

				F18 = 307,

				F19 = 308,

				F20 = 309,

				F21 = 310,

				F22 = 311,

				F23 = 312,

				F24 = 313,

				F25 = 314,

				KP_0 = 320,

				KP_1 = 321,

				KP_2 = 322,

				KP_3 = 323,

				KP_4 = 324,

				KP_5 = 325,

				KP_6 = 326,

				KP_7 = 327,

				KP_8 = 328,

				KP_9 = 329,

				KP_DECIMAL = 330,

				KP_DIVIDE = 331,

				KP_MULTIPLY = 332,

				KP_SUBTRACT = 333,

				KP_ADD = 334,

				KP_ENTER = 335,

				KP_EQUAL = 336,

				LEFT_SHIFT = 340,

				LEFT_CONTROL = 341,

				LEFT_ALT = 342,

				LEFT_SUPER = 343,

				RIGHT_SHIFT = 344,

				RIGHT_CONTROL = 345,

				RIGHT_ALT = 346,

				RIGHT_SUPER = 347,

				MENU = 348,

				//LAST and FIRST allows you to iterate through the keys because FIRST is the first value and LAST is the last value

				LAST = MENU,

				FIRST = SPACE
			};//Key

			const Byte& getKey(const short int key);

			bool isKeyDown(const short int key);
			bool isKeyRepeated(const short int key);
			bool isKeyReleased(const short int key);

			int getMouseX() noexcept;
			int getMouseY() noexcept;

			double getScrollVertical() noexcept;
			double getScrollHorizontal() noexcept;
		}//Input
	}//os
}//mc

#endif
