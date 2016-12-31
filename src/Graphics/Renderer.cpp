/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Utility/Preprocessor.h>
//we need to include cstring for memcpy
#include <cstring>

#include <iostream>

namespace mc {
	namespace gfx {

		namespace {
			RenderQueue renderQueue = RenderQueue();
			std::vector<RenderImpl*> protocols = std::vector<RenderImpl*>();

			Size originalWidth = 0;
			Size originalHeight = 0;

			//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
			Index entityIndex = 0;
		}//anon namespace

		void Renderer::init(const Size width, const Size height) {
			originalWidth = width;
			originalHeight = height;

			ssl::init(width, height);
		}//init
		void Renderer::setUp(os::WindowModule* win) {
			ssl::setUp(win);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->setUp(win, &renderQueue);
			}
		}//setUp
		void Renderer::queue(GraphicsEntity * e, const Index protocol) {
			if( e == nullptr )throw NullPointerException("Input pointer to an entity must not be null in queue()");
			pushEntity(protocol, e);
		}//queue
		void Renderer::resize(const Size width, const Size height) {
			glViewport(0, 0, width, height);

			ssl::resize(width, height);

			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->resize(width, height);
			}
		}
		Size Renderer::numberOfProtocols() {
			return protocols.size();
		}
		//tearDown
		void Renderer::tearDown(os::WindowModule* win) {
			for( Index i = 0; i < protocols.size(); ++i ) {
				protocols[i]->tearDown(win, &renderQueue);
			}

			ssl::tearDown(win);
		}//tearDown
		//resize
		void Renderer::renderFrame(os::WindowModule* win) {
			setUp(win);
			for( RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end(); ++pair ) {
				GraphicsEntity* en = pair->second;
				en->setProperty(Entity::HOVERED, false);

				entityIndex = pair - renderQueue.begin();
				protocols[pair->first]->render(win, en);
			}
			tearDown(win);
		}//renderFrame

		void Renderer::checkInput() {
			ssl::checkInput();
		}//update

		void Renderer::destroy() {
			while( !protocols.empty() ) {
				RenderImpl* protocol = protocols.back();
				protocol->destroy();
				delete protocol;
				protocols.pop_back();
			}

			ssl::destroy();
		}//destroy()

		void Renderer::clearBuffers() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderQueue.clear();
		}//clearBuffers()

		void Renderer::setRefreshColor(const float r, const float g, const float b, const float a) {
			glClearColor(r, g, b, a);
		}//setRefreshColor(r,g,b,a)

		void Renderer::setRefreshColor(const Color & c) {
			setRefreshColor(c.r, c.g, c.b, c.a);
		}//setRefreshColor(Color)

		Size Renderer::getOriginalWidth() {
			return originalWidth;
		}//getOriginalWidth()

		Size Renderer::getOriginalHeight() {
			return originalHeight;
		}//getOriginalHeight()

		void Renderer::pushEntity(Index protocol, GraphicsEntity * entity) {
			renderQueue.push_back(std::pair<Index, GraphicsEntity*>(protocol, entity));
		}//pushEntity(protocol, entity)

		void Renderer::pushProtocol(RenderImpl * protocol) {
			protocols.push_back(protocol);
		}//pushProtocol(protocol)

		//ssl
		namespace ssl {
			//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

			//how many floats in the windowData uniform buffer. needs to be multiplied by sizeof(float)
#define MACE_WINDOW_DATA_BUFFER_SIZE 4
//which binding location the windowData uniform buffer should be bound to
#define MACE_WINDOW_DATA_LOCATION 14

//how many floats in the entityData uniform buffer.
#define MACE_ENTITY_DATA_BUFFER_SIZE sizeof(float)*24
//which binding location the paintdata uniform buffer should be bound to
#define MACE_ENTITY_DATA_LOCATION 15

			namespace {
				//ssl resources
				Preprocessor sslPreprocessor = Preprocessor("");

				//ssl buffer objects
				ogl::UniformBuffer windowData = ogl::UniformBuffer();

				//fbo resources
				ogl::FrameBuffer frameBuffer = ogl::FrameBuffer();
				ogl::RenderBuffer depthBuffer = ogl::RenderBuffer();

				ogl::Texture sceneTexture = ogl::Texture();
				ogl::Texture idTexture = ogl::Texture();

				SimpleQuadRenderer renderer = SimpleQuadRenderer(false);

				IncludeString vertexLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_vertex.glsl>
				}, "ssl_vertex");
				/**
				@todo Remove discard from shader
				*/
				IncludeString fragmentLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_frag.glsl>
				}, "ssl_frag");
				IncludeString positionLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_position.glsl>
				}, "ssl_position");
				IncludeString mouseLibrary = IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_window.glsl>
				}, "ssl_window");

				//this function goes into the anonymous namespace because it technically doesn't belong in the ssl namespace. it should remain to this source file
				void generateFramebuffer(const Size& width, const Size& height) {

					sceneTexture.setData(nullptr, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

					idTexture.setData(nullptr, width, height, GL_UNSIGNED_INT, GL_RED_INTEGER, GL_R32UI);

					depthBuffer.init();
					depthBuffer.bind();
					depthBuffer.setStorage(GL_DEPTH_COMPONENT, width, height);

					//for our custom FBO. we render using a z-buffer to figure out which entity is clicked on
					frameBuffer.init();
					frameBuffer.attachTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture.getID(), 0);
					frameBuffer.attachTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, idTexture.getID(), 0);
					frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

					Enum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
					frameBuffer.setDrawBuffers(2, buffers);

					if( frameBuffer.checkStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
						throw InitializationError("Error initializing framebuffer! This GPU may be unsupported!");
					}

					ogl::checkGLError(__LINE__, __FILE__);

					glViewport(0, 0, width, height);
				}//generateFrambuffer
			}//anon namespace



			void init(const Size &, const Size &) {
				windowData.init();

				windowData.setLocation(MACE_WINDOW_DATA_LOCATION);
				windowData.bind();
				float defaultWindowData[MACE_WINDOW_DATA_BUFFER_SIZE] = { static_cast<float>(originalWidth), static_cast<float>(originalHeight),static_cast<float>(originalWidth),static_cast<float>(originalHeight) };
				windowData.setData(sizeof(float)*MACE_WINDOW_DATA_BUFFER_SIZE, defaultWindowData);
				windowData.unbind();

				sceneTexture.init();
				sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				idTexture.init();
				idTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				idTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				//better to access originalWidth and originalHeight directly than via a parameter.
				generateFramebuffer(originalWidth, originalHeight);

				ogl::checkGLError(__LINE__, __FILE__);

				const char* vertexShader2D = {
#	include <MACE/Graphics/Shaders/scene.v.glsl>
				};
				const char* fragmentShader2D = {
#	include <MACE/Graphics/Shaders/scene.f.glsl>
				};

				renderer.init(vertexShader2D, fragmentShader2D);

				ogl::checkGLError(__LINE__, __FILE__);

				//gl states
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}//init

			void setUp(os::WindowModule *) {
				frameBuffer.bind();
				sceneTexture.bind();
				idTexture.bind();

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}//setUp

			void tearDown(os::WindowModule * win) {
				frameBuffer.unbind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				sceneTexture.bind();

				renderer.draw();

				ogl::checkGLError(__LINE__, __FILE__);

				glfwSwapBuffers(win->getGLFWWindow());
			}//tearDown

			void destroy() {
				windowData.destroy();

				depthBuffer.destroy();
				frameBuffer.destroy();

				sceneTexture.destroy();
				idTexture.destroy();

				renderer.destroy();
			}//destroy

			void bindEntity(const GraphicsEntity * entity, ogl::ShaderProgram& prog) {
				const ogl::UniformBuffer& entityData = entity->buffer;

				entityData.bind();
				entityData.bindToUniformBlock(prog.getID(), "ssl_BaseEntityBuffer");
				entityData.bindForRender();

				windowData.bindForRender();

				prog.setUniform("ssl_EntityID", entityIndex + 1);
			}//bindEntity

			void bindShaderProgram(ogl::ShaderProgram & prog) {
				windowData.bindToUniformBlock(prog.getID(), "ssl_WindowData");
				prog.createUniform("ssl_EntityID");
			}//bindShaderProgram

			void resize(const Size & width, const Size & height) {
				//if the window is iconified, width and height will be 0. we cant create a framebuffer of size 0, so we dont do anything

				if( width > 0 && height > 0 ) {
					windowData.bind();
					float newSize[2] = { static_cast<float>(width),static_cast<float>(height) };
					windowData.setDataRange(sizeof(newSize), sizeof(newSize), newSize);
					windowData.unbind();

					depthBuffer.destroy();
					frameBuffer.destroy();

					generateFramebuffer(width, height);
				}
			}//resize

			void checkInput() {
				frameBuffer.bind();

				Index pixel = 0;
				frameBuffer.setReadBuffer(GL_COLOR_ATTACHMENT1);
				frameBuffer.readPixels(os::Input::getMouseX(), os::Input::getMouseY(), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);

				if( pixel > 0 ) {
					//the entity id stored is 1 plus the actual one, to differeniate from an error read (0) or an actual id. so we decrement it to get the actual inex
					--pixel;
					renderQueue[pixel].second->setProperty(Entity::HOVERED, true);
				}

				frameBuffer.unbind();
			}//checkInput

			void bindBuffer(ogl::UniformBuffer & buf) {
				buf.bind();
				//we set it to null, because during the actual rendering we set the data
				buf.setData(MACE_ENTITY_DATA_BUFFER_SIZE, nullptr);
				buf.unbind();
			}//bindBuffer

			void fillBuffer(GraphicsEntity * entity) {
				if( !entity->getProperty(Entity::INIT) ) {
					throw InitializationError("Entity is not initializd.");
				}

				ogl::UniformBuffer& buf = entity->buffer;
				const TransformMatrix& transform = entity->transformation;

				Vector<float, 3> translation = transform.translation;
				Vector<float, 3> scale = transform.scaler;
				Vector<float, 3> rotation = transform.rotation;
				Vector<float, 3> inheritedTranslation = { 0,0,0 };
				Vector<float, 3> inheritedScale = { 1,1,1 };
				Vector<float, 3> inheritedRotation = { 0,0,0 };

				if( entity->hasParent() ) {

					const Entity* par = entity->getParent();

					//iterate through every parent
					while( par->hasParent() ) {
						const TransformMatrix& parTransform = par->getTransformation();


						inheritedTranslation += parTransform.translation;
						inheritedScale *= parTransform.scaler;
						inheritedRotation += parTransform.rotation;


						par = par->getParent();
					}
				}

				//we want {-1, -1} to be the bottom left, not the top left that OpenGL normally has. inverting the Y does this for us
				inheritedTranslation[1] *= -1;
				translation[1] *= -1;

				//GLSL expects the boolean values as a float, because memory is stored in increments of a float.
				const float stretch_x = entity->getProperty(Entity::STRETCH_X) ? 1.0f : 0.0f;
				const float stretch_y = entity->getProperty(Entity::STRETCH_Y) ? 1.0f : 0.0f;

				//the following are containers for the flatten() call
				float flattenedData[3];

				//now we set the uniform buffer defining the transformations of the entity
				buf.bind();
				//holy crap thats a lot of flags. this is the fastest way to map the buffer. the difference is MASSIVE. try it.
				float* mappedEntityData = static_cast<float*>(buf.mapRange(0, MACE_ENTITY_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
				std::memcpy((mappedEntityData), translation.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 3;//pointer arithmetic!
				std::memcpy(mappedEntityData, &stretch_x, sizeof(stretch_x));
				++mappedEntityData;
				std::memcpy(mappedEntityData, scale.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 3;
				std::memcpy(mappedEntityData, &stretch_y, sizeof(stretch_y));
				++mappedEntityData;
				std::memcpy(mappedEntityData, rotation.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 4;
				std::memcpy(mappedEntityData, inheritedTranslation.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 4;
				std::memcpy(mappedEntityData, inheritedScale.flatten(flattenedData), sizeof(Vector<float, 3>));
				mappedEntityData += 4;
				std::memcpy(mappedEntityData, inheritedRotation.flatten(flattenedData), sizeof(Vector<float, 3>));

				buf.unmap();

				buf.setLocation(MACE_ENTITY_DATA_LOCATION);
			}//fillBuffer

			std::string processShader(const std::string & shader, const GLenum& type) {
				Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
				shaderPreprocessor.defineMacro(mc::Macro("__SHADER_TYPE__", std::to_string(type)));

				const std::string processedShader = shaderPreprocessor.preprocess();

				return processedShader;
			}//processShader

			const mc::Preprocessor& getSSLPreprocessor() {
				if( sslPreprocessor.macroNumber() == 0 ) {
					sslPreprocessor.defineOSMacros();
					sslPreprocessor.defineStandardMacros();

					sslPreprocessor.defineMacro(mc::Macro("__SSL__", "1"));

					//C-style casts are unsafe. Problem is that this is a C API. You must use a C-style cast in order to do this correctly.
					sslPreprocessor.defineMacro(mc::Macro("GL_VENDOR", (const char*) (glGetString(GL_VENDOR))));
					sslPreprocessor.defineMacro(mc::Macro("GL_RENDERER", (const char*) (glGetString(GL_RENDERER))));
					sslPreprocessor.defineMacro(mc::Macro("GL_VERSION", (const char*) (glGetString(GL_VERSION))));
					sslPreprocessor.defineMacro(mc::Macro("GL_SHADING_LANGUAGE_VERSION", (const char*) (glGetString(GL_SHADING_LANGUAGE_VERSION))));

					/*
					in order to define a bunch of opengl macros, we need to check if they exist, just in case this system doesnt support
					a certain macro. the following is a special macro which only defines a macro in sslPreprocessor if it is defined in
					reality
					*/

					//indirection is the only way to expand macros in other macros
#define MACE_NAME_STRINGIFY(name) "" #name
#define MACE_STRINGIFY(name) #name
					//the strcmp checks if the macro is defined. if the name is different from it expanded, then it is a macro. doesnt work if a macro is defined as itself, but that shouldnt happen
#define MACE_DEFINE_MACRO(name) if(std::strcmp("" #name ,MACE_NAME_STRINGIFY(name))){sslPreprocessor.defineMacro( Macro( #name , MACE_STRINGIFY( name ) ));}
					/*Shader macros*/
					MACE_DEFINE_MACRO(GL_VERTEX_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_FRAGMENT_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_GEOMETRY_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_TESS_CONTROL_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_TESS_EVALUATION_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_COMPUTE_SHADER);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_ATTRIBUTES);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_INPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_OUTPUT_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_IMAGE_UNIFORMS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS);

					MACE_DEFINE_MACRO(GL_MAX_UNIFORM_BUFFER_BINDINGS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_UNIFORM_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
					MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
					MACE_DEFINE_MACRO(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_ATOMIC_COUNTERS);
					MACE_DEFINE_MACRO(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS);
					MACE_DEFINE_MACRO(GL_MAX_IMAGE_UNITS);
					MACE_DEFINE_MACRO(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES);

					MACE_DEFINE_MACRO(GL_FALSE);
					MACE_DEFINE_MACRO(GL_TRUE);
					MACE_DEFINE_MACRO(NULL);
#undef MACE_NAME_STRINGIFY
#undef MACE_STRINGIFY
#undef MACE_DEFINE_MACRO

					sslPreprocessor.addInclude(vertexLibrary);
					sslPreprocessor.addInclude(fragmentLibrary);
					sslPreprocessor.addInclude(positionLibrary);
					sslPreprocessor.addInclude(mouseLibrary);
				}
				return sslPreprocessor;
			}//getSSLPreprocessor

#undef MACE_WINDOW_DATA_BUFFER_SIZE
#undef MACE_WINDOW_DATA_LOCATION
#undef MACE_PAINT_DATA_BUFFER_SIZE
#undef MACE_PAINT_DATA_LOCATION
#undef MACE_ENTITY_DATA_BUFFER_SIZE
#undef MACE_ENTITY_DATA_LOCATION

		}//ssl

		SimpleQuadRenderer::SimpleQuadRenderer(const bool ssl) : useSSL(ssl) {}

		void SimpleQuadRenderer::init(const char * vertexShader, const char * fragShader) {
			const float squareTextureCoordinates[8] = {
				0.0f,1.0f,
				0.0f,0.0f,
				1.0f,0.0f,
				1.0f,1.0f,
			};

			const unsigned int squareIndices[6] = {
				0,1,3,
				1,2,3
			};

			const float squareVertices[12] = {
				-1.0f,-1.0f,0.0f,
				-1.0f,1.0f,0.0f,
				1.0f,1.0f,0.0f,
				1.0f,-1.0f,0.0f
			};

			shaders2D.init();
			square.init();

			//vao loading
			if( useSSL ) {
				square.loadVertices(4, squareVertices, 15, 3);
			} else {
				square.loadVertices(4, squareVertices, 0, 3);
			}
			square.storeDataInAttributeList(4, squareTextureCoordinates, 1, 2);

			square.loadIndices(6, squareIndices);

			//shader stuff
			shaders2D.createVertex(useSSL ? ssl::processShader(vertexShader, GL_VERTEX_SHADER) : vertexShader);
			shaders2D.createFragment(useSSL ? ssl::processShader(fragShader, GL_FRAGMENT_SHADER) : fragShader);

			shaders2D.link();

			if( useSSL ) {
				ssl::bindShaderProgram(shaders2D);
			}

			ogl::checkGLError(__LINE__, __FILE__);
		}

		void SimpleQuadRenderer::init(const char * vertexShader, const std::string & fragShader) {
			init(vertexShader, fragShader.c_str());
		}

		void SimpleQuadRenderer::init(const std::string & vertexShader, const char * fragShader) {
			init(vertexShader.c_str(), fragShader);
		}

		void SimpleQuadRenderer::init(const std::string & vertexShader, const std::string & fragShader) {
			init(vertexShader.c_str(), fragShader.c_str());
		}

		void SimpleQuadRenderer::destroy() {
			shaders2D.destroy();
			square.destroy();
		}

		void SimpleQuadRenderer::draw(const GraphicsEntity * en) {
			square.bind();
			shaders2D.bind();

			if( useSSL ) {
				ssl::bindEntity(en, shaders2D);
			}

			square.draw();

			ogl::checkGLError(__LINE__, __FILE__);
		}

		void SimpleQuadRenderer::setShader(const ogl::ShaderProgram & shader) {
			shaders2D = shader;
		}

		ogl::ShaderProgram & SimpleQuadRenderer::getShader() {
			return shaders2D;
		}

		const ogl::ShaderProgram & SimpleQuadRenderer::getShader() const {
			return shaders2D;
		}

		void SimpleQuadRenderer::setVertexArray(const ogl::VertexArray & vertices) {
			square = vertices;
		}

		ogl::VertexArray & SimpleQuadRenderer::getVertexArray() {
			return square;
		}

		const ogl::VertexArray & SimpleQuadRenderer::getVertexArray() const {
			return square;
		}

		bool SimpleQuadRenderer::operator==(const SimpleQuadRenderer & other) const {
			return shaders2D == other.shaders2D&&square == other.square;
		}

		bool SimpleQuadRenderer::operator!=(const SimpleQuadRenderer & other) const {
			return !operator==(other);
		}

	}//gfx
}//mc

