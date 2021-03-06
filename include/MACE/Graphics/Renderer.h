/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_RENDERER_H
#define MACE__GRAPHICS_RENDERER_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Core/Error.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Color.h>

#include <deque>
#include <stack>

namespace mc {
	namespace gfx {
		class GraphicsEntity;

		//if the container we use is ever going to be changed, we typedef
		using RenderQueue = std::deque<GraphicsEntity*>;
		using EntityID = unsigned int;

		//forward declare dependencies
		class Renderer;
		class PainterImpl;
		class Texture;
		class Model;

		namespace Enums {
			//painter stuff
			enum class Brush: Byte {
				TEXTURE = 0, COLOR = 1, MASK = 2, BLEND = 3, MASKED_BLEND = 4,
			};

			enum class RenderFeatures: Byte {
				DISCARD_INVISIBLE = 0x01,
				FILTER = 0x02,
				TEXTURE = 0x04,
				TEXTURE_TRANSFORM = 0x08,

				NONE = 0x00,
				DEFAULT = FILTER | TEXTURE | TEXTURE_TRANSFORM,
			};

			MACE_CONSTEXPR inline RenderFeatures operator|(const RenderFeatures& left, const RenderFeatures& right) {
				return static_cast<RenderFeatures>(static_cast<Byte>(left) | static_cast<Byte>(right));
			}

			MACE_CONSTEXPR inline RenderFeatures operator&(const RenderFeatures& left, const RenderFeatures& right) {
				return static_cast<RenderFeatures>(static_cast<Byte>(left) & static_cast<Byte>(right));
			}

			MACE_CONSTEXPR inline RenderFeatures operator~(const RenderFeatures& r) {
				return static_cast<RenderFeatures>(~static_cast<Byte>(r));
			}

			/**
			@todo changed Texture::bind() to use this in some form
			*/
			enum class TextureSlot: unsigned int {
				FOREGROUND = 0,
				BACKGROUND = 1,
				MASK = 2,
			};
		}

		class Painter: public Beginable, private Initializable {
			friend class GraphicsEntity;
			friend class PainterImpl;
		public:
			struct State {
				Color foregroundColor, backgroundColor, maskColor;

				Vector<float, 4> foregroundTransform, backgroundTransform, maskTransform;

				Vector<float, 4> data;

				TransformMatrix transformation;

				Matrix<float, 4, 4> filter = math::identity<float, 4>();

				bool operator==(const State& other) const;
				bool operator!=(const State& other) const;
			};

			Painter() = delete;
			Painter(const Painter& p);
			~Painter() = default;

			void drawModel(const Model& m, const Texture& img, const Enums::RenderFeatures features = Enums::RenderFeatures::DEFAULT);

			void fillModel(const Model& m, const Enums::RenderFeatures features = Enums::RenderFeatures::DEFAULT & ~Enums::RenderFeatures::DISCARD_INVISIBLE);

			void fillRect(const float x = 0.0f, const float y = 0.0f, const float w = 1.0f, const float h = 1.0f);
			void fillRect(const Vector<float, 2>& pos, const Vector<float, 2>& size);
			void fillRect(const Vector<float, 4>& dim);

			void drawImage(const Texture& img);

			void maskImage(const Texture& img, const Texture& mask);

			void blendImages(const Texture& foreground, const Texture& background, const float amount = 0.5f);
			void blendImagesMasked(const Texture& foreground, const Texture& background, const Texture& mask, const float minimumThreshold = 0.0f, const float maximumThreshold = 1.0f);

			void drawQuad(const Enums::Brush brush, const Enums::RenderFeatures features = Enums::RenderFeatures::DEFAULT);
			void draw(const Model& m, const Enums::Brush brush, const Enums::RenderFeatures features);

			const GraphicsEntity* const getEntity() const;

			void setTexture(const Texture& t, const Enums::TextureSlot& slot);

			void setForegroundColor(const Color& col);
			Color& getForegroundColor();
			const Color& getForegroundColor() const;

			void setForegroundTransform(const Vector<float, 4>& trans);
			Vector<float, 4>& getForegroundTransform();
			const Vector<float, 4>& getForegroundTransform() const;

			void setBackgroundColor(const Color& col);
			Color& getBackgroundColor();
			const Color& getBackgroundColor() const;

			void setBackgroundTransform(const Vector<float, 4>& trans);
			Vector<float, 4>& getBackgroundTransform();
			const Vector<float, 4>& getBackgroundTransform() const;

			void setMaskColor(const Color& col);
			Color& getMaskColor();
			const Color& getMaskColor() const;

			void setMaskTransform(const Vector<float, 4>& trans);
			Vector<float, 4>& getMaskTransform();
			const Vector<float, 4>& getMaskTransform() const;

			void setFilter(const float r, const float g, const float b, const float a = 1.0f);
			void setFilter(const Vector<float, 4> & col);
			void setFilter(const Matrix<float, 4, 4>& col);
			Matrix<float, 4, 4>& getFilter();
			const Matrix<float, 4, 4>& getFilter() const;

			void setData(const float a, const float b, const float c, const float d);
			void setData(const Vector<float, 4>& col);
			Vector<float, 4>& getData();
			const Vector<float, 4>& getData() const;

			void setTransformation(const TransformMatrix& trans);
			TransformMatrix& getTransformation();
			const TransformMatrix& getTransformation() const;

			void setOpacity(const float opacity);
			float getOpacity();
			const float getOpacity() const;

			void translate(const Vector<float, 3>& vec);
			void translate(const float x, const float y, const float z = 0.0f);

			void rotate(const Vector<float, 3>& vec);
			void rotate(const float x, const float y, const float z);

			void scale(const Vector<float, 3>& vec);
			void scale(const float x, const float y, const float z = 1.0f);

			void resetTransform();

			void push();
			void pop();

			void reset();

			void setState(const State& s);
			State& getState();
			const State& getState() const;

			const EntityID& getID() const;

			Painter operator=(const Painter& right);

			bool operator==(const Painter& other) const;
			bool operator!=(const Painter& other) const;
		private:
			std::shared_ptr<PainterImpl> impl = nullptr;

			Painter::State state = Painter::State();

			//for pushing/popping the state
			std::stack<Painter::State> stateStack{};

			GraphicsEntity* const entity = nullptr;

			EntityID id = 0;

			Painter(GraphicsEntity* const en);

			void begin() override;
			void end() override;

			void init() override;
			void destroy() override;

			void clean();
		};

		class PainterImpl: public Initializable, public Beginable {
			friend class Renderer;
			friend class Painter;
		public:
			virtual ~PainterImpl() noexcept = default;

			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void begin() override = 0;
			virtual void end() override = 0;

			virtual void clean() = 0;

			virtual void loadSettings(const Painter::State& state) = 0;
			virtual void draw(const Model& m, const Enums::Brush brush, const Enums::RenderFeatures features) = 0;

			bool operator==(const PainterImpl& other) const;
			bool operator!=(const PainterImpl& other) const;
		protected:
			PainterImpl(Painter* const painter);

			Painter* const painter;
		};

		/**
		@todo add function to change how many samples msaa uses
		@todo add renderers for directx, cpu, vulkan, opengl es, opengl 1.1/2.1
		*/
		class Renderer {
			friend class Painter;
			friend class GraphicsContext;
			friend class WindowModule;
		public:
			virtual ~Renderer() = default;

			virtual GraphicsEntity* getEntityAt(const int x, const int y) = 0;

			/**
			@opengl
			*/
			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			/**
			@opengl
			*/
			void setRefreshColor(const Color& c);

			Size getWidth() const;
			Size getHeight() const;

			Size getSamples() const;

			Vector<float, 2> getWindowRatios() const;

			RenderQueue getRenderQueue() const;

			bool isResized() const;

			GraphicsContext* getContext();
			const GraphicsContext* getContext() const;

			/**
			@internal
			*/
			void flagResize();
		protected:
			RenderQueue renderQueue = RenderQueue();

			Size samples = 1;

			bool resized;

			Vector<float, 2> windowRatios;

			GraphicsContext* context;

			virtual void onResize(gfx::WindowModule* win, const Size width, const Size height) = 0;
			virtual void onInit(gfx::WindowModule* win) = 0;
			virtual void onSetUp(gfx::WindowModule* win) = 0;
			virtual void onTearDown(gfx::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onQueue(GraphicsEntity* en) = 0;

			//not declared const because some of the functions require modification to an intneral buffer of impls
			virtual std::shared_ptr<PainterImpl> createPainterImpl(Painter* const  painter) = 0;
		private:
			/**
			@internal
			@opengl
			*/
			void resize(gfx::WindowModule* win, const Size width, const Size height);

			/**
			@internal
			@opengl
			*/
			void init(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void setUp(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void tearDown(gfx::WindowModule* win);

			/**
			@internal
			@opengl
			*/
			void checkInput(gfx::WindowModule* win);


			/**
			@internal
			@opengl
			*/
			void destroy();

			EntityID queue(GraphicsEntity* const e);

			void remove(const EntityID i);

			EntityID pushEntity(GraphicsEntity* const  entity);
		};//Renderer

		class GraphicsEntity: public Entity {
		public:
			GraphicsEntity() noexcept;

			virtual ~GraphicsEntity() noexcept override;

			/**
			@dirty
			*/
			Painter& getPainter();
			const Painter& getPainter() const;

			bool operator==(const GraphicsEntity& other) const noexcept;
			bool operator!=(const GraphicsEntity& other) const noexcept;
		protected:
			virtual void onRender(Painter& painter) = 0;

			void clean() override final;

			void init() override final;

			void destroy() override final;
		private:
			Painter painter = Painter(this);

			void onRender() override final;
		};//GraphicsEntity
	}//gfx
}//mc

#endif
