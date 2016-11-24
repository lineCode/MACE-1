/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_WINDOW_H
#define MACE_GRAPHICS_WINDOW_H

//so we can use glew
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <MACE/System/Module.h>
#include <MACE/Graphics/Entity.h>
#include <thread>
#include <string>

namespace mc {
	namespace gfx {
		class Window {
			friend class WindowModule;
		public:
			Window(const int width, const int height, const char* title);
			GLFWwindow* getGLFWWindow();

			const unsigned int& getFPS() const;
			void setFPS(const unsigned int& FPS);

			void create();
			void poll();
			void destroy();

			const int getOriginalWidth() const;
			const int getOriginalHeight() const;

			std::string getTitle();
			const std::string getTitle() const;
			void setTitle(const std::string& newTitle);
		protected:
			unsigned int fps = 0;

			GLFWwindow* window;
			int originalWidth;
			int originalHeight;
			std::string title;
		};//Window

		class GraphicsContext {
		public:
			virtual void setUpWindow(Window* win) = 0;
			virtual void init(Window* win) = 0;
			virtual void render(Window* win) = 0;
			virtual void destroy(Window* win) = 0;
			virtual void update() = 0;
		};//GraphicsContext

		class WindowModule: public Module {
		public:
			WindowModule(Window* window);

			void init();
			void update();
			void destroy();

			void setContext(GraphicsContext* con);
			GraphicsContext* getContext();
			const GraphicsContext* getContext() const;

			std::string getName() const;
		private:
			Window* window;

			bool destroyed = false;

			std::thread windowThread;

			GraphicsContext* context = nullptr;//initailize an empty context

			void threadCallback();
		};//WindowModule

		class OpenGLContext: public Entity, public GraphicsContext {
		public:
			OpenGLContext();

			void update();

			void setUpWindow(Window* win);
			void init(Window* win);
			void render(Window* win);
			void destroy(Window* win);

			void setVSync(const bool& sync);
		private:
			bool vsync = false;

			//these are for the Entity inheritence
			void customUpdate();
			void customRender();
			void customDestroy();
			void customInit();
		};//OpenGLContext

		class InputManager {

		};
	}
}

#endif